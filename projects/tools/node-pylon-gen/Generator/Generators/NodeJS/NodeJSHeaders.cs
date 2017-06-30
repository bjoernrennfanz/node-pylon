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

using System.Collections.Generic;
using NodePylonGen.Generator.Utils;
using NodePylonGen.Generator.Model;

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
            GenerateLegalFilePreamble(CommentType.BCPL);
            GenerateFilePreamble(CommentType.BCPL);
            
            PushBlock(BlockType.Includes);
            WriteLine("#pragma once");
            NewLine();

            // Generate #include for interop
            WriteLine("#include <node.h>");
            WriteLine("#include <nan.h>");
            NewLine();

            // Generate #include forward references.
            PushBlock(BlockType.IncludesForwardReferences);
            WriteLine("#include <{0}>", TranslationUnit.IncludePath);
            GenerateIncludeForwardRefernces();
            PopBlock(NewLineType.BeforeNextBlock);
            PopBlock(NewLineType.Always);

            // using namespace v8;

            PushBlock(BlockType.Footer);
            PopBlock();
        }

        public void GenerateIncludeForwardRefernces()
        {
            
        }

    }
}