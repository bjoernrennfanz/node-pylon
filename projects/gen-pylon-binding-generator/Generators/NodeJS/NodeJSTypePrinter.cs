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

using CppSharp;
using CppSharp.AST;
using CppSharp.AST.Extensions;
using CppSharp.Types;
using GenPylonBinding.Core.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using BindingContext = GenPylonBinding.Generators.BindingContext;

namespace GenPylonBinding.Generator.Generators.NodeJS
{
    public class NodeJSTypePrinter : CppTypePrinter
    {
        private static string[] stringStaticCastFilter = { "unsigned", "wchar_t" };

        public BindingContext Context { get; private set; }

        public DriverOptions Options => Context.Options;
        public TypeMapDatabase TypeMapDatabase => Context.TypeMaps;

        public NodeJSTypePrinter(BindingContext context)
        {
            Context = context;
        }

        public string VisitReturnType(QualifiedType type, bool hasName, bool hasModifier)
        {
            bool savedPrintTypeModifiers = PrintTypeModifiers;
            bool savedPrintTypeQualifiers = PrintTypeQualifiers;
            string result = string.Empty;

            PrintTypeModifiers = hasModifier;
            PrintTypeQualifiers = hasModifier;

            result = type.Visit(this);

            PrintTypeModifiers = savedPrintTypeModifiers;
            PrintTypeQualifiers = savedPrintTypeQualifiers;

            return result;
        }

        public string VisitParameter(Parameter param, bool hasName, bool hasModifier)
        {
            bool savedPrintTypeModifiers = PrintTypeModifiers;
            bool savedPrintTypeQualifiers = PrintTypeQualifiers;
            string result = string.Empty;

            PrintTypeModifiers = hasModifier;
            PrintTypeQualifiers = hasModifier;

            result = VisitParameter(param, false);

            PrintTypeModifiers = savedPrintTypeModifiers;
            PrintTypeQualifiers = savedPrintTypeQualifiers;

            return result;
        }

        public override string VisitUnsupportedType(UnsupportedType type, TypeQualifiers quals)
        {
            // Generate type name and modifiers
            string result = type.Description;
            result += (quals.IsConst ? " const" : string.Empty);

            return result;
        }

        public void GenerateReturnTypeWrapper(NodeJSTemplate callee, QualifiedType returnType, string calleeToWrap)
        {
            NodeJSTypeCheckPrinter nodeJSTypeCheckPrinter = new NodeJSTypeCheckPrinter(Context);

            // Check for void return type
            if ((returnType.Type is BuiltinType) && ((returnType.Type as BuiltinType).Type == PrimitiveType.Void))
            {
                // Write wrapper for void return type
                callee.WriteLine("{0};", calleeToWrap);
                callee.WriteLine("");
                callee.WriteLine("// Set return value to undefined");
                callee.WriteLine("info.GetReturnValue().SetUndefined();");
            }
            else if (nodeJSTypeCheckPrinter.QualifiedTypeIsObject(returnType))
            {



                callee.WriteLine("{0};", calleeToWrap);

            }
            else if (nodeJSTypeCheckPrinter.QualifiedTypeIsNumber(returnType))
            {
                PointerType parameterPointerType = returnType.Type as PointerType;
                if (parameterPointerType != null && parameterPointerType.IsPointer())
                {
                    // Currently not supported!
                    throw new NotImplementedException();
                }
                else
                {
                    // Generate return type wrapper for number type
                    callee.WriteLine("{0} result = {1};", VisitReturnType(returnType, false, false), calleeToWrap);
                    callee.WriteLine("");
                    callee.WriteLine("// Set return value");
                    callee.WriteLine("info.GetReturnValue().Set(Nan::New<Number>(result));");
                }
            }
            else if (nodeJSTypeCheckPrinter.QualifiedTypeIsBoolean(returnType))
            {
                PointerType parameterPointerType = returnType.Type as PointerType;
                if (parameterPointerType != null && parameterPointerType.IsPointer())
                {
                    // Currently not supported!
                    throw new NotImplementedException();
                }
                else
                {
                    // Generate return type wrapper for boolean type
                    callee.WriteLine("{0} result = {1};", VisitReturnType(returnType, false, false), calleeToWrap);
                    callee.WriteLine("");
                    callee.WriteLine("// Set return value");
                    callee.WriteLine("info.GetReturnValue().Set(Nan::New<Boolean>(result));");
                }
            }
            else if (nodeJSTypeCheckPrinter.QualifiedTypeIsString(returnType))
            {
                callee.WriteLine("const {0}{1} result = {2};", VisitReturnType(returnType, false, false), (returnType.Type is PointerType ? ((returnType.Type as PointerType).IsPointer() ? "*" : string.Empty) : string.Empty), calleeToWrap);
                callee.WriteLine("");
                callee.WriteLine("// Set return value");
                callee.WriteLine("info.GetReturnValue().Set(pylon_v8::FromGCString({0}result).ToLocalChecked());", (returnType.Type is PointerType ? ((returnType.Type as PointerType).IsPointer() ? string.Empty : "&" ) : "&"));
            }
            else if (nodeJSTypeCheckPrinter.QualifiedTypeIsTypedBuffer(returnType))
            {
                callee.WriteLine("// TODO: Implement return value wrapper for {0}", calleeToWrap);
            }
        }

        public string GenerateParameterWrapper(NodeJSTemplate callee, IEnumerable<Parameter> parameters)
        {
            int parameterArgumentIndex = 0;
            string generatedArgumentsWrapped = string.Empty;
            NodeJSTypeCheckPrinter nodeJSTypeCheckPrinter = new NodeJSTypeCheckPrinter(Context);

            foreach (Parameter parameter in parameters)
            {
                if (nodeJSTypeCheckPrinter.QualifiedTypeIsObject(parameter.QualifiedType))
                {
                    string parameterClassName = VisitParameter(parameter, false, false);
                    string parameterClassWrapped = NamingHelper.GenerateClassWrapName(parameterClassName);

                    // Generate unwrap of stored object
                    callee.PushBlock(BlockKind.MethodBody);
                    callee.WriteLine("// Unwrap object");
                    callee.WriteLine("{0}* arg{1}_wrap = ObjectWrap::Unwrap<{0}>(info[{1}]->ToObject());", parameterClassWrapped, parameterArgumentIndex);
                    callee.WriteLine("{0}* arg{1} = arg{1}_wrap->GetWrapped();", parameterClassName, parameterArgumentIndex);
                    callee.PopBlock(NewLineKind.BeforeNextBlock);
                }
                else if (nodeJSTypeCheckPrinter.QualifiedTypeIsNumber(parameter.QualifiedType))
                {
                    // Generate wrapper for number values
                    callee.PushBlock(BlockKind.MethodBody);

                    PointerType parameterPointerType = parameter.Type as PointerType;
                    if (parameterPointerType != null && parameterPointerType.IsPointer())
                    {
                        callee.WriteLine("// Convert from number value to pointer");
                        callee.WriteLine("{0} arg{1}_value = static_cast<{0}>(info[{1}]->NumberValue());", VisitParameter(parameter, false, false), parameterArgumentIndex);
                        callee.WriteLine("{0}* arg{1} = &arg{1}_value;", VisitParameter(parameter, false, false), parameterArgumentIndex);
                    }
                    else
                    {
                        callee.WriteLine("// Convert from number value");
                        callee.WriteLine("{0} arg{1} = static_cast<{0}>(info[{1}]->NumberValue());", VisitParameter(parameter, false, false), parameterArgumentIndex);
                    }

                    callee.PopBlock(NewLineKind.BeforeNextBlock);
                }
                else if (nodeJSTypeCheckPrinter.QualifiedTypeIsBoolean(parameter.QualifiedType))
                {
                    // Generate wrapper for number values
                    callee.PushBlock(BlockKind.MethodBody);

                    PointerType parameterPointerType = parameter.Type as PointerType;
                    if (parameterPointerType != null && parameterPointerType.IsPointer())
                    {
                        callee.WriteLine("// Convert from boolean value to pointer");
                        callee.WriteLine("{0} arg{1}_value = info[{1}]->BooleanValue();", VisitParameter(parameter, false, false), parameterArgumentIndex);
                        callee.WriteLine("{0}* arg{1} = &arg{1}_value;", VisitParameter(parameter, false, false), parameterArgumentIndex);
                    }
                    else
                    {
                        callee.WriteLine("// Convert from boolean value");
                        callee.WriteLine("{0} arg{1} = info[{1}]->BooleanValue();", VisitParameter(parameter, false, false), parameterArgumentIndex);
                    }

                    callee.PopBlock(NewLineKind.BeforeNextBlock);
                }
                else if (nodeJSTypeCheckPrinter.QualifiedTypeIsString(parameter.QualifiedType))
                {
                    string parameterToWrap = string.Format("{0}{1}", VisitParameter(parameter, false, false), (parameter.Type is PointerType ? ((parameter.Type as PointerType).IsPointer() ? "*" : string.Empty) : string.Empty));
                    string generatedStringWrapperLine = string.Format("{0} arg{1} = ", parameterToWrap, parameterArgumentIndex);
                    // Generate wrapper for string values
                    callee.PushBlock(BlockKind.MethodBody);
                    callee.WriteLine("// Convert from string value");

                    // Check if cast is needed
                    generatedStringWrapperLine += stringStaticCastFilter.Any(filter => parameterToWrap.Contains(filter)) ? string.Format("static_cast<{0}>(", parameterToWrap) : string.Empty;

                    // Generate conversation
                    generatedStringWrapperLine += string.Format("pylon_v8::ToGCString(info[{0}]->ToString())", parameterArgumentIndex);
                    generatedStringWrapperLine += parameterToWrap.Contains("wchar_t") ? ".w_str()" : ".c_str()";
                    generatedStringWrapperLine += parameterToWrap.Contains("*") ? "" : "[0]";

                    // Add closing tag if cast was needed
                    generatedStringWrapperLine += stringStaticCastFilter.Any(filter => parameterToWrap.Contains(filter)) ? ");" : ";";

                    callee.WriteLine(generatedStringWrapperLine);
                    callee.PopBlock(NewLineKind.BeforeNextBlock);
                }
                else if (nodeJSTypeCheckPrinter.QualifiedTypeIsTypedBuffer(parameter.QualifiedType))
                {
                    callee.PushBlock(BlockKind.MethodBody);
                    callee.WriteLine("// TODO: Implement wrapper for {0}", VisitParameter(parameter, false, false));
                    callee.PopBlock(NewLineKind.BeforeNextBlock);
                }

                // Store arguments for later usage
                generatedArgumentsWrapped += parameterArgumentIndex > 0 ? ", " : string.Empty;
                generatedArgumentsWrapped += parameter.Type is PointerType ? ((parameter.Type as PointerType).IsReference ? "*" : string.Empty) : string.Empty;
                generatedArgumentsWrapped += "arg" + parameterArgumentIndex;

                // Increment argument index
                parameterArgumentIndex++;
            }

            return generatedArgumentsWrapped;
        }
    }
}
