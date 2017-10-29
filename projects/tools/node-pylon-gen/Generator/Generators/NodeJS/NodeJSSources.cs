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


using CppSharp;
using CppSharp.AST;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using NodePylonGen.Config;
using CppSharp.Generators;
using BindingContext = NodePylonGen.Generators.BindingContext;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    /// <summary>
    /// Generates C++/NodeJS wrapper source files.
    /// </summary>
    public class NodeJSSources : NodeJSTemplate
    {
        public NodeJSSources(BindingContext context, IEnumerable<TranslationUnit> units)
            : base(context, units)
        {
        }

        public override string FileExtension => "cc";

        public override void Process()
        {
            GenerateLegalFilePreamble(CommentKind.BCPL);
            GenerateFilePreamble(CommentKind.BCPL);

            // Generate name of own include
            PushBlock(BlockKind.Includes);
            WriteLine("#include \"{0}.h\"", Path.GetFileNameWithoutExtension(FilePath).Replace('\\', '/'));
            WriteLine("#include \"{0}.h\"", GetMarshalHelperPath("pylon_v8"));
            PopBlock(NewLineKind.BeforeNextBlock);

            // Generate namespace for forward references.
            PushBlock(BlockKind.Usings);
            WriteLine("using namespace v8;");
            GenerateNamespaceUsings();
            PopBlock(NewLineKind.BeforeNextBlock);

            // Generate wrapper class implementations
            GenerateWrapperClassImplementations();

            PushBlock(BlockKind.Footer);
            PopBlock();
        }

        private string GetMarshalHelperPath(string wrapperHeader)
        {
            ConfigMapping pylonNodeConfig = Context.ConfigurationContext.ConfigFilesLoaded.Where(config => config.Id == "pylon-node").FirstOrDefault();
            string relativNodePylonRootPath = PathHelpers.GetRelativePath(Path.GetDirectoryName(TranslationUnit.FilePath), Path.GetDirectoryName(pylonNodeConfig.AbsoluteFilePath));

            return Path.Combine(relativNodePylonRootPath, wrapperHeader);
        }

        private void GenerateWrapperClassImplementations()
        {
            NodeJSTypeReferenceCollector typeReferenceCollector = new NodeJSTypeReferenceCollector(Context.ConfigurationContext, Context.TypeMaps, Context.Options);
            typeReferenceCollector.Process(TranslationUnit);

            // Find own class to wrap
            NodeJSTypeReference classToWrapTypeReference = typeReferenceCollector.TypeReferences
                .Where(item => item.Declaration is Class)
                .Where(item => item.Declaration.Name.ToLower().Contains(TranslationUnit.Name.ToLower()))
                .FirstOrDefault();

            string className = string.Empty;
            string classNameWrap = string.Empty;

            // Generate wrapper class name
            if (classToWrapTypeReference != null)
            {
                className = (classToWrapTypeReference.Declaration as Class).Name;
                classNameWrap = GenerateTrimmedClassName(className) + "Wrap";
            }
            else
            {
                TextInfo textInfo = new CultureInfo("en-US", false).TextInfo;
                classNameWrap = textInfo.ToTitleCase(TranslationUnit.FileNameWithoutExtension) + "Wrap";
            }

            // Generate NodeJS wrapper prototypes
            GenerateWrapperClassPersistentFunctions(classNameWrap);

            if (classToWrapTypeReference != null)
            {
                // Generate constructors
                GenerateWrapperClassConstructors(classToWrapTypeReference, typeReferenceCollector);
            }
        }

        private void GenerateWrapperClassConstructors(NodeJSTypeReference classToWrapTypeReference, NodeJSTypeReferenceCollector typeReferenceCollector)
        {
            NodeJSTypePrinter nodeJSTypePrinter = new NodeJSTypePrinter(Context);
            NodeJSTypeCheckPrinter nodeJSTypeCheckPrinter = new NodeJSTypeCheckPrinter(Context);
            nodeJSTypePrinter.PrintScopeKind = TypePrintScopeKind.Local;

            Class classToWrap = classToWrapTypeReference.Declaration as Class;
            string classNameWrap = GenerateTrimmedClassName(classToWrap.Name) + "Wrap";
            string classNameWrapperMember = "m_" + GenerateTrimmedClassName(classToWrap.Name);

            IEnumerable<Method> constructors = classToWrap.Constructors.OrderByDescending(s => s.Parameters.Count);

            // Supported implementations comment
            PushBlock(BlockKind.Method);
            WriteLine("// Supported implementations");
            foreach (Method constructor in constructors.OrderBy(s => s.Parameters.Count))
            {                    
                WriteLine("// {0}({1})", classToWrap.Name, nodeJSTypePrinter.VisitParameters(constructor.Parameters, true));
            }

            // Constructor for wrapped class
            WriteLine("{0}::{0}(Nan::NAN_METHOD_ARGS_TYPE info)", classNameWrap);
            WriteLine("  : {0}(NULL)", classNameWrapperMember);
            WriteStartBraceIndent();

            bool firstConstructorCreated = false;
            int constructorArgumentIndex = 0;

            // Generate check for constructor arguments
            WriteLine("// Check constructor arguments");
            foreach (Method constructor in constructors.OrderBy(s => s.Parameters.Count))
            {
                // Generate default constructor here when needed
                if (constructor.Parameters.Count == 0 && !firstConstructorCreated)
                {
                    WriteLine("if (info.Length() == 0)");
                    WriteStartBraceIndent();

                    WriteLine("// {0}()", classToWrap.Name);
                    WriteLine("{0} = new {1}();", classNameWrapperMember, classToWrap.Name);

                    WriteCloseBraceIndent();
                    firstConstructorCreated = true;
                }
                else
                {
                    // Generate other constructors than default
                    string generatedCheckStatement = (firstConstructorCreated ? "else " : string.Empty);
                    generatedCheckStatement += "if " + (constructor.Parameters.Count > 1 ? "(" : string.Empty);

                    constructorArgumentIndex = 0;
                    foreach (Parameter parameter in constructor.Parameters)
                    {
                        // Generate argument check foreach parameter
                        generatedCheckStatement += constructorArgumentIndex > 0 ? " && " : string.Empty;
                        generatedCheckStatement += "(info[" + constructorArgumentIndex + "]->" + nodeJSTypeCheckPrinter.VisitParameter(parameter) + ")";

                        // Increment argument index
                        constructorArgumentIndex++;
                    }

                    generatedCheckStatement += (constructor.Parameters.Count > 1 ? ")" : string.Empty);

                    // Output arguments checker
                    WriteLine(generatedCheckStatement);
                    WriteStartBraceIndent();
                    WriteLine("// {0}({1})", classToWrap.Name, nodeJSTypePrinter.VisitParameters(constructor.Parameters, true));

                    constructorArgumentIndex = 0;
                    foreach (Parameter parameter in constructor.Parameters)
                    {
                        if (nodeJSTypeCheckPrinter.ParameterIsObject(parameter))
                        {
                            string parameterClassName = nodeJSTypePrinter.VisitParameter(parameter, false, false);

                            WriteLine("GENICAM_NAMESPACE::gcstring info{0}_constructor = pylon_v8::ToGCString(info[{0}]->ToObject()->GetConstructorName());", constructorArgumentIndex);
                            WriteLine("if (info{0}_constructor != \"{1}\")", constructorArgumentIndex, parameterClassName);
                            WriteStartBraceIndent();
                            WriteLine("ThrowException(Exception::TypeError(String::New(\"{0}::{0}: bad argument\")));", classToWrap.Name);
                            WriteCloseBraceIndent();
                            WriteLine(string.Empty);

                            WriteLine("// Unwrap obj");

                        }
                        else if (nodeJSTypeCheckPrinter.ParameterIsNumber(parameter))
                        {

                        }

                        WriteLine("// Unwrap obj");

                    }



                    WriteCloseBraceIndent();
                    firstConstructorCreated = true;
                }
            }
        
            WriteCloseBraceIndent();
            PopBlock(NewLineKind.BeforeNextBlock);
        }

        private void GenerateWrapperClassPersistentFunctions(string classNameWrap)
        {
            PushBlock(BlockKind.Template);
            WriteLine("Nan::Persistent<FunctionTemplate> {0}::prototype;", classNameWrap);
            WriteLine("Nan::Persistent<Function> {0}::constructor;", classNameWrap);
            PopBlock(NewLineKind.BeforeNextBlock);
        }
    }
}