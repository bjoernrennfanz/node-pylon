// MIT License
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

using BindingContext = NodePylonGen.Generators.BindingContext;
using CppSharp.AST;
using CppSharp.AST.Extensions;
using CppSharp.Generators;
using CppSharp.Types;
using System;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    class NodeJSTypeCheckPrinter : TypePrinter
    {
        private const string NodeV8IsObject = "IsObject()";
        private const string NodeV8IsString = "IsString()";
        private const string NodeV8IsBoolean = "IsBoolean()";
        private const string NodeV8IsNumber = "IsNumber()";

        public BindingContext Context { get; private set; }

        public DriverOptions Options => Context.Options;
        public TypeMapDatabase TypeMapDatabase => Context.TypeMaps;

        public NodeJSTypeCheckPrinter(BindingContext context)
        {
            Context = context;
        }
        public override TypePrinterResult VisitParameter(Parameter param, bool hasName = true)
        {
            return param.QualifiedType.Visit(this);
        }

        public override TypePrinterResult VisitPointerType(PointerType pointer, TypeQualifiers quals)
        {
            return NodeV8IsObject;
        }

        public override TypePrinterResult VisitPrimitiveType(PrimitiveType primitive, TypeQualifiers quals)
        {
            switch (primitive)
            {
                case PrimitiveType.Bool: return NodeV8IsBoolean;
                case PrimitiveType.Void: return NodeV8IsObject;
                case PrimitiveType.Char16: return NodeV8IsObject;
                case PrimitiveType.Char32: return NodeV8IsObject;
                case PrimitiveType.WideChar: return NodeV8IsObject;
                case PrimitiveType.Char: return NodeV8IsObject;
                case PrimitiveType.SChar: return NodeV8IsObject;
                case PrimitiveType.UChar: return NodeV8IsObject;
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

            throw new NotSupportedException();
        }

        public bool ParameterIsObject(Parameter param)
        {
            return VisitParameter(param).ToString() == NodeV8IsObject;
        }

        public bool ParameterIsNumber(Parameter param)
        {
            return VisitParameter(param).ToString() == NodeV8IsNumber;
        }

        public bool ParameterIsBoolean(Parameter param)
        {
            return VisitParameter(param).ToString() == NodeV8IsBoolean;
        }
    }
}
