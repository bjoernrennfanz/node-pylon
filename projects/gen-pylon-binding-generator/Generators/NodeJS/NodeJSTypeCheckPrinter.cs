﻿// MIT License
//
// Copyright (c) 2017 Björn Rennfanz <bjoern@fam-rennfanz.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

using CppSharp.AST;
using CppSharp.AST.Extensions;
using CppSharp.Generators;
using CppSharp.Types;
using GenPylonBinding.Core.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using BindingContext = GenPylonBinding.Generators.BindingContext;
using Type = CppSharp.AST.Type;

namespace GenPylonBinding.Generator.Generators.NodeJS
{
    class NodeJSTypeCheckPrinter : TypePrinter
    {
        private const string NodeV8IsObject = "IsObject()";
        private const string NodeV8IsString = "IsString()";
        private const string NodeV8IsBoolean = "IsBoolean()";
        private const string NodeV8IsNumber = "IsNumber()";
        private const string NodeV8IsArray = "IsArray()";

        // Special cases
        private const string NodeV8IsTypedBuffer = "TypedBuffer";

        public BindingContext Context { get; private set; }

        public DriverOptions Options => Context.Options;
        public TypeMapDatabase TypeMapDatabase => Context.TypeMaps;

        public NodeJSTypeCheckPrinter(BindingContext context)
        {
            Context = context;
        }

        public override TypePrinterResult VisitClassDecl(Class @class)
        {
            return NodeV8IsObject;
        }

        public override TypePrinterResult VisitPointerType(PointerType pointer, TypeQualifiers quals)
        {
            Type pointee = pointer.GetFinalPointee();
            if (pointee != null)
            {
                if ((pointee is BuiltinType) && pointer.IsPointer())
                {
                    if ((pointee as BuiltinType).Type == PrimitiveType.Void)
                    {
                        return NodeV8IsTypedBuffer;
                    }
                }

                TypePrinterResult pointeeType = pointee.Visit(this);
                return pointeeType;
            }

            throw new NotSupportedException();
        }

        public override TypePrinterResult VisitPrimitiveType(PrimitiveType primitive, TypeQualifiers quals)
        {
            switch (primitive)
            {
                case PrimitiveType.Bool: return NodeV8IsBoolean;
                case PrimitiveType.Void: return NodeV8IsObject;
                case PrimitiveType.Char16: return NodeV8IsString;
                case PrimitiveType.Char32: return NodeV8IsString;
                case PrimitiveType.WideChar: return NodeV8IsString;
                case PrimitiveType.Char: return NodeV8IsString;
                case PrimitiveType.SChar: return NodeV8IsString;
                case PrimitiveType.UChar: return NodeV8IsString;
                case PrimitiveType.Short: return NodeV8IsNumber;
                case PrimitiveType.UShort: return NodeV8IsNumber;
                case PrimitiveType.Int: return NodeV8IsNumber;
                case PrimitiveType.UInt: return NodeV8IsNumber;
                case PrimitiveType.Long: return NodeV8IsNumber;
                case PrimitiveType.ULong: return NodeV8IsNumber;
                case PrimitiveType.LongLong: return NodeV8IsNumber;
                case PrimitiveType.ULongLong: return NodeV8IsNumber;
                case PrimitiveType.Int128: return NodeV8IsNumber;
                case PrimitiveType.UInt128: return NodeV8IsNumber;
                case PrimitiveType.Half: return NodeV8IsNumber;
                case PrimitiveType.Float: return NodeV8IsNumber;
                case PrimitiveType.Double: return NodeV8IsNumber;
                case PrimitiveType.LongDouble: return NodeV8IsNumber;
                case PrimitiveType.IntPtr: return NodeV8IsNumber;
                case PrimitiveType.UIntPtr: return NodeV8IsNumber;
                case PrimitiveType.Null: return NodeV8IsObject;
                case PrimitiveType.String: return NodeV8IsString;
            }

            throw new NotSupportedException();
        }

        public override TypePrinterResult VisitUnsupportedType(UnsupportedType type, TypeQualifiers quals)
        {
            // Mapping Enums to Number type
            if (type.Description.StartsWith("E")) return NodeV8IsNumber;
            if (type.Description.EndsWith("Enums")) return NodeV8IsNumber;

            // Search for known objects
            if (Context.ASTContext.TranslationUnits.GetGenerated().ToList().Exists(u => NamingHelper.GenerateTrimmedClassName(u.FileNameWithoutExtension).ToLower().Equals(NamingHelper.GenerateTrimmedClassName(type.Description).ToLower())))
                return NodeV8IsObject;

            // Mapping special Windows types
            if (type.Description.StartsWith("wchar_t")) return NodeV8IsString;

            // Map other unknown always as buffer type
            return NodeV8IsTypedBuffer;
        }

        public bool QualifiedTypeIsArray(QualifiedType qualifiedType)
        {
            return qualifiedType.Visit(this).ToString() == NodeV8IsArray;
        }

        public bool QualifiedTypeIsObject(QualifiedType qualifiedType)
        {
            return qualifiedType.Visit(this).ToString() == NodeV8IsObject;
        }

        public bool QualifiedTypeIsNumber(QualifiedType qualifiedType)
        {
            return qualifiedType.Visit(this).ToString() == NodeV8IsNumber;
        }

        public bool QualifiedTypeIsBoolean(QualifiedType qualifiedType)
        {
            return qualifiedType.Visit(this).ToString() == NodeV8IsBoolean;
        }

        public bool QualifiedTypeIsString(QualifiedType qualifiedType)
        {
            return qualifiedType.Visit(this).ToString() == NodeV8IsString;
        }

        public bool QualifiedTypeIsTypedBuffer(QualifiedType qualifiedType)
        {
            return qualifiedType.Visit(this).ToString() == NodeV8IsTypedBuffer;
        }


    public string GenerateCheckStatement(IEnumerable<Parameter> parameters)
        {
            NodeJSTypePrinter nodeJSTypePrinter = new NodeJSTypePrinter(Context);
            nodeJSTypePrinter.PrintScopeKind = TypePrintScopeKind.Local;

            string generatedCheckStatement = (parameters.Count() >= 1 ? "(" : string.Empty);
            generatedCheckStatement += "(info.Length() == " + parameters.Count() + ")";
            generatedCheckStatement += parameters.Count() > 0 ? " && " : string.Empty;

            int methodArgumentIndex = 0;
            foreach (Parameter parameter in parameters)
            {
                string parameterTypeName = nodeJSTypePrinter.VisitParameter(parameter, false, false);
                generatedCheckStatement += methodArgumentIndex > 0 ? " && " : string.Empty;

                if (QualifiedTypeIsObject(parameter.QualifiedType))
                {
                    generatedCheckStatement += "(info[" + methodArgumentIndex + "]->IsObject() && ";
                    generatedCheckStatement += "(pylon_v8::ToGCString(info[" + methodArgumentIndex + "]->ToObject()->GetConstructorName()) == \"" + parameterTypeName + "\"))";
                }
                else if (QualifiedTypeIsNumber(parameter.QualifiedType))
                {
                    generatedCheckStatement += "info[" + methodArgumentIndex + "]->IsNumber()";
                }
                else if (QualifiedTypeIsBoolean(parameter.QualifiedType))
                {
                    generatedCheckStatement += "info[" + methodArgumentIndex + "]->IsBoolean()";
                }
                else if (QualifiedTypeIsString(parameter.QualifiedType))
                {
                    generatedCheckStatement += "info[" + methodArgumentIndex + "]->IsString()";
                }
                else if (QualifiedTypeIsArray(parameter.QualifiedType))
                {
                    generatedCheckStatement += "info[" + methodArgumentIndex + "]->IsArray()";
                }
                else if (QualifiedTypeIsTypedBuffer(parameter.QualifiedType))
                {
                    generatedCheckStatement += "info[" + methodArgumentIndex + "]->IsObject()";
                }

                methodArgumentIndex++;
            }

            generatedCheckStatement += parameters.Count() >= 1 ? ")" : string.Empty;
            return generatedCheckStatement;
        }
    }
}
