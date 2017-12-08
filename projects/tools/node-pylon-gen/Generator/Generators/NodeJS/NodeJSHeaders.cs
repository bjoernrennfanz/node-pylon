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
using NodePylonGen.Generators;
using NodePylonGen.Utils;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    /// <summary>
    /// Generates C++/NodeJS wrapper header files.
    /// </summary>
    internal class NodeJSHeaders : NodeJSTemplate
    {
        public NodeJSHeaders(BindingContext context, IEnumerable<TranslationUnit> units)
            : base(context, units)
        {
        }

        public override string FileExtension => "h";

        public override void Process()
        {
            GenerateLegalFilePreamble(CommentKind.BCPL);
            GenerateFilePreamble(CommentKind.BCPL);

            PushBlock(BlockKind.Includes);
            WriteLine("#pragma once");
            NewLine();

            // Generate #include for interop
            WriteLine("#include <node.h>");
            WriteLine("#include <nan.h>");
            NewLine();

            // Generate #include forward references.
            PushBlock(BlockKind.IncludesForwardReferences);
            WriteLine("#include <{0}>", TranslationUnit.IncludePath);
            GenerateIncludeForwardRefernces();
            PopBlock(NewLineKind.BeforeNextBlock);
            PopBlock(NewLineKind.Always);

            // Generate namespace for forward references.
            PushBlock(BlockKind.Usings);
            WriteLine("using namespace v8;");
            GenerateNamespaceUsings();
            PopBlock(NewLineKind.BeforeNextBlock);

            GenerateWrapperClass();

            PushBlock(BlockKind.Footer);
            PopBlock();
        }

        public void GenerateIncludeForwardRefernces()
        {
            NodeJSTypeReferenceCollector typeReferenceCollector = new NodeJSTypeReferenceCollector(Context.ConfigurationContext, Context.TypeMaps, Context.Options);
            typeReferenceCollector.Process(TranslationUnit, filterNamespaces: false);

            // Filter for needed includes
            SortedSet<string> includes = new SortedSet<string>(StringComparer.InvariantCulture);
            foreach (NodeJSTypeReference typeRef in typeReferenceCollector.TypeReferences)
            {
                if (typeRef.Include.TranslationUnit == TranslationUnit)
                    continue;

                if (typeRef.Include.File == TranslationUnit.FileName)
                    continue;

                Include include = typeRef.Include;
                TranslationUnit unit = include.TranslationUnit;

                if (unit != null && !unit.IsDeclared)
                    continue;

                if (!string.IsNullOrEmpty(include.File) && include.InHeader)
                    includes.Add(include.ToString());
            }

            // Output include lines
            foreach (string include in includes)
            {
                WriteLine(include);
            }
        }

        public void GenerateWrapperClass()
        {
            NodeJSTypeReferenceCollector typeReferenceCollector = new NodeJSTypeReferenceCollector(Context.ConfigurationContext, Context.TypeMaps, Context.Options);
            typeReferenceCollector.Process(TranslationUnit);

            List<Function> functions = typeReferenceCollector.TypeReferences
                .Where(item => ((item.Declaration is Function) && !(item.Declaration is Method)))
                .Select(item => item.Declaration as Function).ToList();

            NodeJSTypeReference classToWrapTypeReference = typeReferenceCollector.TypeReferences
                .Where(item => item.Declaration is Class)
                .Where(item => NodeJSClassHelper.GenerateTrimmedClassName(TranslationUnit.FileNameWithoutExtension).ToLower().Equals(NodeJSClassHelper.GenerateTrimmedClassName(item.Declaration.Name).ToLower()))
                .FirstOrDefault();

            string className = string.Empty;
            string classNameWrap = string.Empty;
            string classNameWrapperMember = string.Empty;

            // Generate wrapper class name
            if (classToWrapTypeReference != null)
            {
                className = (classToWrapTypeReference.Declaration as Class).Name;

                // Generate wrapper and member names
                classNameWrap = NodeJSClassHelper.GenerateClassWrapName(className);
                classNameWrapperMember = NodeJSClassHelper.GenerateClassWrapperMember(className);
            }
            else
            {
                TextInfo textInfo = new CultureInfo("en-US", false).TextInfo;
                classNameWrap = textInfo.ToTitleCase(TranslationUnit.FileNameWithoutExtension) + "Wrap";
            }

            // Generate class stub
            PushBlock(BlockKind.Class);
            WriteLine("class {0} : public node::ObjectWrap", classNameWrap);
            WriteLine("{");
            WriteLine("public:");
            PushIndent();

            // Write
            WriteLine("static NAN_MODULE_INIT(Initialize);");

            // Check if we have an class to warp
            if (!string.IsNullOrEmpty(className))
            {
                // Generate prototype templates
                WriteLine("static Nan::Persistent<v8::FunctionTemplate> prototype;");
                WriteLine("");

                // Create get wrapped method
                PushBlock(BlockKind.Method);
                WriteLine("{0}* GetWrapped() const", className);
                WriteStartBraceIndent();
                WriteLine("return {0};", classNameWrapperMember);
                PopIndent();
                WriteLine("};");
                PopBlock(NewLineKind.BeforeNextBlock);

                // Create get wrapped method
                PushBlock(BlockKind.Method);
                WriteLine("void SetWrapped({0}* {1})", className, NodeJSClassHelper.ConvertToParameterName(className));
                WriteStartBraceIndent();
                WriteLine("{0} = {1};", classNameWrapperMember, NodeJSClassHelper.ConvertToParameterName(className));
                PopIndent();
                WriteLine("};");
                PopBlock(NewLineKind.BeforeNextBlock);

                // New instance prototype
                PushBlock(BlockKind.MethodBody);
                WriteLine("static v8::Handle<v8::Value> NewInstance({0}* {1});", className, NodeJSClassHelper.ConvertToParameterName(className));
                PopBlock(NewLineKind.Always);
            }
            else
            {
                // Only add empty line when we have static functions
                if (functions.Count > 0)
                {
                    WriteLine("");
                }
            }

            // Check if we have an class to warp
            if (!string.IsNullOrEmpty(className))
            {
                // Private members prototypes
                PopIndent();
                WriteLine("private:");
                PushIndent();

                // Constructor & destructor of wrapper class
                PushBlock(BlockKind.MethodBody);
                WriteLine("static Nan::Persistent<v8::Function> constructor;");
                WriteLine("{0}(Nan::NAN_METHOD_ARGS_TYPE info);", classNameWrap);
                WriteLine("~{0}();", classNameWrap);
                WriteLine("static NAN_METHOD(New);");
                PopBlock(NewLineKind.Always);

                // Check if methods available
                if ((classToWrapTypeReference.Declaration as Class).Methods.Count > 0)
                {
                    // Methods
                    PushBlock(BlockKind.MethodBody);
                    WriteLine("// Wrapped methods");
                    SortedSet<string> methodsProcessed = new SortedSet<string>(StringComparer.InvariantCulture);
                    foreach (Method method in (classToWrapTypeReference.Declaration as Class).Methods)
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
                        WriteLine("static NAN_METHOD({0});", method.Name);
                        methodsProcessed.Add(method.Name);
                    }
                    PopBlock(NewLineKind.BeforeNextBlock);
                }
            }

            // Functions
            if (functions.Count > 0)
            {
                // Private members prototypes
                PopIndent();
                WriteLine("private:");
                PushIndent();

                PushBlock(BlockKind.MethodBody);
                WriteLine("// Wrapped functions");
                SortedSet<string> functionsProcessed = new SortedSet<string>(StringComparer.InvariantCulture);
                foreach (Function function in functions)
                {
                    // Skip on other access level than public
                    if (function.Access != AccessSpecifier.Public)
                        continue;

                    // Process method only once
                    if (functionsProcessed.Contains(function.Name))
                        continue;

                    // Output method declaration
                    WriteLine("static NAN_METHOD({0});", function.Name);
                    functionsProcessed.Add(function.Name);
                }
                PopBlock(NewLineKind.BeforeNextBlock);
            }

            // Wrapped object
            if (!string.IsNullOrEmpty(className))
            {
                // Wrapped object
                PushBlock(BlockKind.MethodBody);
                WriteLine("// Wrapped object");
                WriteLine("{0}* {1};", className, classNameWrapperMember);
                PopBlock();
            }

            PopIndent();
            WriteLine("};");

            PopBlock(NewLineKind.BeforeNextBlock);
        }
    }
}