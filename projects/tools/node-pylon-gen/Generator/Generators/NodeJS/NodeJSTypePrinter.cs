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
using CppSharp.Types;
using System.Collections.Generic;
using CppSharp;
using NodePylonGen.Utils;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    public class NodeJSTypePrinter : CppTypePrinter
    {
        public BindingContext Context { get; private set; }

        public DriverOptions Options => Context.Options;
        public TypeMapDatabase TypeMapDatabase => Context.TypeMaps;

        public NodeJSTypePrinter(BindingContext context)
        {
            Context = context;
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

        public string GenerateParameterWrapper(NodeJSTemplate callee, IEnumerable<Parameter> parameters)
        {
            int parameterArgumentIndex = 0;
            string generatedArgumentsWrapped = string.Empty;
            NodeJSTypeCheckPrinter nodeJSTypeCheckPrinter = new NodeJSTypeCheckPrinter(Context);

            foreach (Parameter parameter in parameters)
            {
                if (nodeJSTypeCheckPrinter.ParameterIsObject(parameter))
                {
                    string parameterClassName = VisitParameter(parameter, false, false);
                    string parameterClassWrapped =  NodeJSClassHelper.GenerateClassWrapName(parameterClassName);

                    // Generate unwrap of stored object
                    callee.PushBlock(BlockKind.MethodBody);
                    callee.WriteLine("// Unwrap object");
                    callee.WriteLine("{0}* arg{1}_wrap = ObjectWrap::Unwrap<{0}>(info[{1}]->ToObject());", parameterClassWrapped, parameterArgumentIndex);
                    callee.WriteLine("{0}* arg{1} = arg{1}_wrap->GetWrapped();", parameterClassName, parameterArgumentIndex);
                    callee.PopBlock(NewLineKind.BeforeNextBlock);

                    // Store arguments for later usage
                    generatedArgumentsWrapped += parameterArgumentIndex > 0 ? ", " : string.Empty;
                    generatedArgumentsWrapped += parameter.Type is PointerType ? (generatedArgumentsWrapped += (parameter.Type as PointerType).IsReference ? "*" : string.Empty) : string.Empty;
                    generatedArgumentsWrapped += "arg" + parameterArgumentIndex;
                }
                else if (nodeJSTypeCheckPrinter.ParameterIsNumber(parameter))
                {
                    // Generate wrapper for number values
                    callee.PushBlock(BlockKind.MethodBody);
                    callee.WriteLine("// Convert from number value");
                    callee.WriteLine("{0} arg{1} = static_cast<{0}>(info[{1}]->NumberValue());", VisitParameter(parameter, false, false), parameterArgumentIndex);
                    callee.PopBlock(NewLineKind.BeforeNextBlock);

                    // Store arguments for later usage
                    generatedArgumentsWrapped += parameterArgumentIndex > 0 ? ", " : string.Empty;
                    generatedArgumentsWrapped += "arg" + parameterArgumentIndex;
                }
                else if (nodeJSTypeCheckPrinter.ParameterIsBoolean(parameter))
                {

                }
                else if (nodeJSTypeCheckPrinter.ParameterIsString(parameter))
                {

                }

                // Increment argument index
                parameterArgumentIndex++;
            }

            return generatedArgumentsWrapped;
        }
    }
}
