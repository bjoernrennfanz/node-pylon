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
using System.IO;

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
            string file = Path.GetFileNameWithoutExtension(FilePath).Replace('\\', '/');

            PushBlock(BlockKind.Includes);
            WriteLine("#include \"{0}.h\"", file);
            GenerateForwardReferenceHeaders();

            NewLine();
            PopBlock();

            PushBlock(BlockKind.Usings);
            WriteLine("using namespace v8;");
            GenerateNamespaceUsings();
            NewLine();
            PopBlock();

            GenerateCppIncludeContext(TranslationUnit);

            PushBlock(BlockKind.Footer);
            PopBlock();
        }

        private void GenerateCppIncludeContext(TranslationUnit unit)
        {
            
        }

        private void GenerateForwardReferenceHeaders()
        {
        }

        private void GenerateNamespaceUsings()
        {
            
        }
    }
}