// MIT License
//
// Copyright (c) 2017 - 2018 Björn Rennfanz <bjoern@fam-rennfanz.de>
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
using BindingContext = NodePylonGen.Generators.BindingContext;
using System;
using NodePylonGen.Utils;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    /// <summary>
    /// Generates C++/NodeJS wrapper source files.
    /// </summary>
    public class NodeJSSources : NodeJSTemplate
    {
        private SortedSet<string> methodsProcessed = new SortedSet<string>(StringComparer.InvariantCulture);
        private SortedSet<string> functionsProcessed = new SortedSet<string>(StringComparer.InvariantCulture);

        private NodeJSTypeCheckPrinter nodeJSTypeCheckPrinter;
        private NodeJSTypePrinter nodeJSTypePrinter;       

        public NodeJSSources(BindingContext context, IEnumerable<TranslationUnit> units)
            : base(context, units)
        {
            nodeJSTypePrinter = new NodeJSTypePrinter(context);
            nodeJSTypePrinter.PrintScopeKind = TypePrintScopeKind.Local;
            nodeJSTypeCheckPrinter = new NodeJSTypeCheckPrinter(context);
        }

        public override string FileExtension => "cc";

        public override void Process()
        {
            GenerateLegalFilePreamble(CommentKind.BCPL, "2017 - " + DateTime.Now.ToString("yyyy"));
            GenerateFilePreamble(CommentKind.BCPL);

            // Generate of own include
            PushBlock(BlockKind.Includes);
            WriteLine("#include \"{0}.h\"", Path.GetFileNameWithoutExtension(FilePath).Replace('\\', '/'));
            WriteLine("#include \"{0}.h\"", GetMarshalHelperPath("pylon_v8"));
            PopBlock(NewLineKind.BeforeNextBlock);

            // Generate of nested includes
            PushBlock(BlockKind.IncludesForwardReferences);
            GenerateNestedIncludeImplementations();
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

            NodeJSTypeReference classToWrapTypeReference = typeReferenceCollector.TypeReferences
                .Where(item => item.Declaration is Class)
                .Where(item => NodeJSClassHelper.GenerateTrimmedClassName(TranslationUnit.FileNameWithoutExtension).ToLower().Equals(NodeJSClassHelper.GenerateTrimmedClassName(item.Declaration.Name).ToLower()))
                .FirstOrDefault();

            string className = string.Empty;
            string classNameWrap = string.Empty;

            // Generate wrapper class name
            if (classToWrapTypeReference != null)
            {
                className = nodeJSTypePrinter.VisitDeclaration(classToWrapTypeReference.Declaration);
                classNameWrap = NodeJSClassHelper.GenerateClassWrapName(className);
            }
            else
            {
                TextInfo textInfo = new CultureInfo("en-US", false).TextInfo;
                classNameWrap = textInfo.ToTitleCase(TranslationUnit.FileNameWithoutExtension) + "Wrap";
            }

            // Check if we need constructor & destructor
            if (classToWrapTypeReference != null)
            {
                // Generate NodeJS wrapper prototypes
                GenerateWrapperClassPersistentFunctions(classNameWrap);

                // Generate constructors
                GenerateWrapperClassConstructors(classToWrapTypeReference);
                GenerateWrapperClassDestructors(classToWrapTypeReference);

                // TODO: New Instance & New
            }

            // Generate initialize
            GenerateWrapperInitialize(classToWrapTypeReference, classNameWrap, typeReferenceCollector);

            // Check if we need to generate methods
            if (methodsProcessed.Count > 0)
            {
                GenerateWrapperMethods(classToWrapTypeReference, classNameWrap, typeReferenceCollector);
            }

            // Check if we need to generate functions
            if (functionsProcessed.Count > 0)
            {
                GenerateWrapperFunctions(classNameWrap, typeReferenceCollector);
            }

            // Check if translation unit is an module
            if (Context.ConfigurationContext.ConfigFilesLoaded.Where(config => ((config.Module != null) && (config.Module == TranslationUnit.FileNameWithoutExtension))).Count() > 0)
            {
                // Generate Node JS initialize for specified module
                PushBlock(BlockKind.Footer);
                WriteLine("NODE_MODULE({0}, {1}::Initialize)", TranslationUnit.FileNameWithoutExtension, classNameWrap);
                PopBlock(NewLineKind.BeforeNextBlock);
            }
        }

        private void GenerateWrapperFunctions(string classNameWrap, NodeJSTypeReferenceCollector typeReferenceCollector)
        {
            // Loop over all static functions
            foreach (string functionName in functionsProcessed)
            {
                PushBlock(BlockKind.Method);
                WriteLine("NAN_METHOD({0}::{1})", classNameWrap, functionName);
                WriteStartBraceIndent();

                // Find overloaded static functions
                IEnumerable<Function> functionOverloads = typeReferenceCollector.TypeReferences
                    .Where(item => ((item.Declaration is Function) && !(item.Declaration is Method)))
                    .Select(item => item.Declaration as Function)
                    .Where(s => s.Name == functionName).OrderByDescending(s => s.Parameters.Count);

                bool firstFunctionCreated = false;
                foreach (Function overloadFunction in functionOverloads)
                {
                    // Generate other constructors than default
                    string generatedIfStatement = (firstFunctionCreated ? "else " : string.Empty);
                    generatedIfStatement += "if " + nodeJSTypeCheckPrinter.GenerateCheckStatement(overloadFunction.Parameters);

                    // Output arguments checker
                    PushBlock(BlockKind.MethodBody);
                    WriteLine(generatedIfStatement);
                    WriteStartBraceIndent();

                    // Generate wrapper for parameter arguments
                    string generatedArgumentsWrapped = nodeJSTypePrinter.GenerateParameterWrapper(this, overloadFunction.Parameters);
                    if (!string.IsNullOrEmpty(generatedArgumentsWrapped)) WriteLine("");

                    // Call wrapped method
                    WriteLine("// Call wrapped function");
                    nodeJSTypePrinter.GenerateReturnTypeWrapper(this, overloadFunction.ReturnType, string.Format("{0}({1})", overloadFunction.Name, generatedArgumentsWrapped));
                    
                    WriteCloseBraceIndent();
                    PopBlock(NewLineKind.Never);

                    // Remember that we have created an function
                    firstFunctionCreated = true;
                }

                WriteCloseBraceIndent();
                PopBlock(NewLineKind.BeforeNextBlock);
            }
        }

        private void GenerateWrapperMethods(NodeJSTypeReference classToWrapTypeReference, string classNameWrap, NodeJSTypeReferenceCollector typeReferenceCollector)
        {
            // Find class to wrap
            Class classToWrap = classToWrapTypeReference.Declaration as Class;
            
            // Loop over all public methods
            foreach (string methodName in methodsProcessed)
            {
                PushBlock(BlockKind.Method);
                WriteLine("NAN_METHOD({0}::{1})", classNameWrap, methodName);
                WriteStartBraceIndent();

                // Generate class object wrap
                string classNameNorm = NodeJSClassHelper.GenerateTrimmedClassName(classToWrap.Name);
                string classNameWrapper = NodeJSClassHelper.GenerateWrapperClassName(classToWrap.Name);
                string classNameNormLower = classNameNorm.Substring(0, 1).ToLower() + classNameNorm.Substring(1);

                PushBlock(BlockKind.MethodBody);
                WriteLine("{0}* {1} = ObjectWrap::Unwrap<{0}>(info.This());", classNameWrap, classNameWrapper);
                WriteLine("{0}* {1} = {2}->GetWrapped();", classToWrap.Name, classNameNormLower, classNameWrapper);
                PopBlock(NewLineKind.BeforeNextBlock);

                // Find overloaded methods
                IEnumerable<Method> overloadMethods = classToWrap.Methods
                    .Where(s => s.Name == methodName)
                    .OrderByDescending(s => s.Parameters.Count);

                bool firstMethodCreated = false;
                foreach (Method overloadMethod in overloadMethods)
                {
                    // Generate other constructors than default
                    string generatedIfStatement = (firstMethodCreated ? "else " : string.Empty);
                    generatedIfStatement += "if " + nodeJSTypeCheckPrinter.GenerateCheckStatement(overloadMethod.Parameters);

                    // Output arguments checker
                    PushBlock(BlockKind.MethodBody);
                    WriteLine(generatedIfStatement);
                    WriteStartBraceIndent();

                    // Generate wrapper for parameter arguments
                    string generatedArgumentsWrapped = nodeJSTypePrinter.GenerateParameterWrapper(this, overloadMethod.Parameters);
                    if (!string.IsNullOrEmpty(generatedArgumentsWrapped)) WriteLine("");

                    // Call wrapped method
                    WriteLine("// Call wrapped method");
                    nodeJSTypePrinter.GenerateReturnTypeWrapper(this, overloadMethod.ReturnType, string.Format("{0}->{1}({2})", classNameNormLower, overloadMethod.Name, generatedArgumentsWrapped));

                    WriteCloseBraceIndent();
                    PopBlock(NewLineKind.Never);

                    // Remember that we have created an method
                    firstMethodCreated = true;
                }

                WriteCloseBraceIndent();
                PopBlock(NewLineKind.BeforeNextBlock);
            }
        }

        private void GenerateWrapperClassConstructors(NodeJSTypeReference classToWrapTypeReference)
        {
            Class classToWrap = classToWrapTypeReference.Declaration as Class;

            string classNameWrap = NodeJSClassHelper.GenerateClassWrapName(classToWrap.Name);
            string classNameWrapperMember = NodeJSClassHelper.GenerateClassWrapperMember(classToWrap.Name);

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

            // Generate check for constructor arguments
            WriteLine("// Check constructor arguments");
            foreach (Method constructor in constructors.OrderBy(s => s.Parameters.Count))
            {
                // Generate other constructors than default
                string generatedCheckStatement = (firstConstructorCreated ? "else " : string.Empty);
                generatedCheckStatement += "if " + nodeJSTypeCheckPrinter.GenerateCheckStatement(constructor.Parameters);

                // Output arguments checker
                WriteLine(generatedCheckStatement);
                WriteStartBraceIndent();

                // Generate wrapper for parameter arguments
                string generatedArgumentsWrapped = nodeJSTypePrinter.GenerateParameterWrapper(this, constructor.Parameters);

                // Generate construction of wrapped member
                PushBlock(BlockKind.MethodBody);
                WriteLine("// {0}({1})", classToWrap.Name, nodeJSTypePrinter.VisitParameters(constructor.Parameters, true));
                WriteLine("{0} = new {1}({2});", classNameWrapperMember, classToWrap.Name, generatedArgumentsWrapped);
                PopBlock(NewLineKind.Never);
                WriteCloseBraceIndent();

                // Remember that we have created an constructor
                firstConstructorCreated = true;
            }

            WriteCloseBraceIndent();
            PopBlock(NewLineKind.BeforeNextBlock);
        }

        private void GenerateWrapperClassDestructors(NodeJSTypeReference classToWrapTypeReference)
        {
            Class classToWrap = classToWrapTypeReference.Declaration as Class;
            string classNameWrap = NodeJSClassHelper.GenerateClassWrapName(classToWrap.Name);
            string classNameWrapperMember = NodeJSClassHelper.GenerateClassWrapperMember(classToWrap.Name);

            // Destructor for wrapped class
            PushBlock(BlockKind.Method);
            WriteLine("{0}::~{0}()", classNameWrap);
            WriteStartBraceIndent();
            WriteLine("delete {0};", classNameWrapperMember);
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

        private void GenerateWrapperInitialize(NodeJSTypeReference classToWrapTypeReference, string classNameWrap, NodeJSTypeReferenceCollector typeReferenceCollector)
        {
            PushBlock(BlockKind.Method);
            WriteLine("NAN_MODULE_INIT({0}::Initialize)", classNameWrap);
            WriteStartBraceIndent();

            // Generate constructor template code
            if (classToWrapTypeReference != null)
            {
                // Find class to wrap
                Class classToWrap = classToWrapTypeReference.Declaration as Class;

                PushBlock(BlockKind.MethodBody);
                WriteLine("// Prepare constructor template");
                WriteLine("Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);");
                WriteLine("tpl->SetClassName(Nan::New(\"{0}\").ToLocalChecked());", classNameWrap);
                WriteLine("tpl->InstanceTemplate()->SetInternalFieldCount(1);");
                PopBlock(NewLineKind.BeforeNextBlock);

                // Generate methods prototypes
                PushBlock(BlockKind.MethodBody);
                WriteLine("// Register prototypes to template");

                foreach (Method method in classToWrap.Methods)
                {
                    // Skip constructors
                    if (method.IsConstructor)
                        continue;

                    // Skip on other access level than public
                    if (method.Access != AccessSpecifier.Public)
                        continue;

                    // Process method only once
                    if (methodsProcessed.Contains(method.Name))
                        continue;

                    // Output method declaration
                    string methodNameNorm = method.Name;
                    string methodNameLower = methodNameNorm.Substring(0, 1).ToLower() + methodNameNorm.Substring(1);
                    WriteLine("Nan::SetPrototypeMethod(tpl, \"{0}\", {1});", methodNameLower, methodNameNorm);
                    methodsProcessed.Add(method.Name);
                }

                PopBlock(NewLineKind.BeforeNextBlock);

                PushBlock(BlockKind.MethodBody);
                WriteLine("// Register template in Node JS");
                WriteLine("prototype.Reset(tpl);");
                WriteLine("Local<Function> function = Nan::GetFunction(tpl).ToLocalChecked();");
                WriteLine("constructor.Reset(function);");
                WriteLine("Nan::Set(target, Nan::New(\"{0}\").ToLocalChecked(), function);", classToWrap.Name);
                PopBlock(NewLineKind.BeforeNextBlock);
            }

            // Find static functions
            List<Function> functions = typeReferenceCollector.TypeReferences
                .Where(item => ((item.Declaration is Function) && !(item.Declaration is Method)))
                .Select(item => item.Declaration as Function).ToList();

            // Generate static methods
            if (functions.Count > 0)
            {
                PushBlock(BlockKind.MethodBody);
                WriteLine("// Register static functions in Node JS");

                foreach (Function function in functions)
                {
                    // Skip on other access level than public
                    if (function.Access != AccessSpecifier.Public)
                        continue;

                    // Process method only once
                    if (functionsProcessed.Contains(function.Name))
                        continue;

                    // Output function declaration
                    string functionNameNorm = function.Name;
                    string functionNameLower = functionNameNorm.Substring(0, 1).ToLower() + functionNameNorm.Substring(1);
                    WriteLine("Nan::Set(target, Nan::New<String>(\"{0}\").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>({1}::{2})).ToLocalChecked());", functionNameLower, classNameWrap, functionNameNorm);
                    functionsProcessed.Add(function.Name);
                }

                PopBlock(NewLineKind.BeforeNextBlock);
            }

            // Generate nested class initialize
            IEnumerable<string> nestedIncludes = GetNestedIncludes(classNameWrap);
            if (nestedIncludes.Count() > 0)
            {
                PushBlock(BlockKind.MethodBody);
                WriteLine("// Initialize dynamic classes");

                foreach (string include in nestedIncludes)
                {
                    string includeClassNameWrap = string.Empty;
                    TranslationUnit includeUnit = Context.ASTContext.TranslationUnits.Find(unit => unit.FileNameWithoutExtension == include);

                    NodeJSTypeReferenceCollector includeReferenceCollector = new NodeJSTypeReferenceCollector(Context.ConfigurationContext, Context.TypeMaps, Context.Options);
                    includeReferenceCollector.Process(includeUnit);

                    // Find own class to wrap
                    NodeJSTypeReference includeToWrapTypeReference = includeReferenceCollector.TypeReferences
                        .Where(item => item.Declaration is Class)
                        .Where(item => NodeJSClassHelper.GenerateTrimmedClassName(includeUnit.FileNameWithoutExtension).ToLower().Equals(NodeJSClassHelper.GenerateTrimmedClassName(item.Declaration.Name).ToLower()))
                        .FirstOrDefault();

                    // Check if nested class was found
                    if (includeToWrapTypeReference != null)
                    {
                        string className = (includeToWrapTypeReference.Declaration as Class).Name;
                        includeClassNameWrap = NodeJSClassHelper.GenerateClassWrapName(className);
                    }
                    else
                    {
                        TextInfo textInfo = new CultureInfo("en-US", false).TextInfo;
                        includeClassNameWrap = NodeJSClassHelper.GenerateClassWrapName(textInfo.ToTitleCase(includeUnit.FileNameWithoutExtension));
                    }

                    WriteLine("{0}::Initialize(target);", includeClassNameWrap);
                }

                PopBlock(NewLineKind.BeforeNextBlock);
            }

            WriteCloseBraceIndent();
            PopBlock(NewLineKind.BeforeNextBlock);
        }

        private void GenerateNestedIncludeImplementations()
        {
            NodeJSTypeReferenceCollector typeReferenceCollector = new NodeJSTypeReferenceCollector(Context.ConfigurationContext, Context.TypeMaps, Context.Options);
            typeReferenceCollector.Process(TranslationUnit);

            // Find own class to wrap
            NodeJSTypeReference classToWrapTypeReference = typeReferenceCollector.TypeReferences
                .Where(item => item.Declaration is Class)
                .Where(item => NodeJSClassHelper.GenerateTrimmedClassName(TranslationUnit.FileNameWithoutExtension).ToLower().Equals(NodeJSClassHelper.GenerateTrimmedClassName(item.Declaration.Name).ToLower()))
                .FirstOrDefault();

            string className = string.Empty;
            string classNameWrap = string.Empty;

            // Generate wrapper class name
            if (classToWrapTypeReference != null)
            {
                className = nodeJSTypePrinter.VisitDeclaration(classToWrapTypeReference.Declaration);
                classNameWrap = NodeJSClassHelper.GenerateClassWrapName(className);
            }
            else
            {
                TextInfo textInfo = new CultureInfo("en-US", false).TextInfo;
                classNameWrap = NodeJSClassHelper.GenerateClassWrapName(textInfo.ToTitleCase(TranslationUnit.FileNameWithoutExtension));
            }

            // Generate nested class initialize
            IEnumerable<string> nestedIncludes = GetNestedIncludes(classNameWrap);
            if (nestedIncludes.Count() > 0)
            {
                foreach (string include in nestedIncludes)
                {
                    //string includeId = string.IsNullOrEmpty(include.Alias) ? include.Id : include.Alias.ToLower();
                    WriteLine("#include \"{0}/{1}.h\"", TranslationUnit.FileNameWithoutExtension.ToLower(), include);
                }
            }
        }

        private IEnumerable<string> GetNestedIncludes(string classNameWrap)
        {
            IEnumerable<ConfigMapping> configFilesLoaded = Context.ConfigurationContext.ConfigFilesLoaded;
            foreach (ConfigMapping configFileLoad in configFilesLoaded)
            {
                string configFileLoadedName = (configFileLoad.Module != null ? NodeJSClassHelper.GenerateTrimmedClassName(configFileLoad.Module) : string.Empty);
                if (!(string.IsNullOrEmpty(configFileLoadedName)))
                {
                    TextInfo textInfo = new CultureInfo("en-US", false).TextInfo;
                    string configFileLoadedWrappedName = textInfo.ToTitleCase(configFileLoadedName) + "Wrap";
                    if (configFileLoadedWrappedName == classNameWrap)
                    {
                        // Workaround for GeniCam files
                        if (configFileLoadedName.ToLower().Contains("genicam"))
                        {
                            // Search all GeniCam related files and remove doubles
                            return Context.ASTContext.TranslationUnits.GetGenerated()
                                .Where(unit => !unit.FileRelativeDirectory.ToLower().Contains("pylon"))
                                .Where(unit => !string.IsNullOrEmpty(unit.FileRelativeDirectory))
                                .Select(unit => unit.FileNameWithoutExtension).Distinct().ToList();
                        }
                        else
                        {
                            // Search all pylon related files and remove doubles
                            return Context.ASTContext.TranslationUnits.GetGenerated()
                                .Where(unit => unit.FileRelativeDirectory.ToLower().Contains(configFileLoadedName.ToLower()))
                                .Select(unit => unit.FileNameWithoutExtension).Distinct().ToList();
                        }
                    }
                }
            }

            // Return empty list
            return new List<string>();
        }
    }
}