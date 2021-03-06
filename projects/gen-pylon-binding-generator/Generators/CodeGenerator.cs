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
using GenPylonBinding.Generators;
using System;
using System.Collections.Generic;
using System.IO;

namespace GenPylonBinding.Generator.Generators
{
    public abstract class CodeGenerator : CppSharp.Generators.CodeGenerator
    {
        public new BindingContext Context { get; private set; }

        public new virtual string FilePath
        {
            get
            {
                string path = Path.GetDirectoryName(TranslationUnit.FilePath);
                string file = TranslationUnit.FileNameWithoutExtension + "." + FileExtension;
                string filePath = Path.Combine(path, file);

                return filePath;
            }
        }

        protected CodeGenerator(BindingContext context, TranslationUnit unit)
            : this(context, new List<TranslationUnit> { unit })
        {
        }

        protected CodeGenerator(BindingContext context, IEnumerable<TranslationUnit> units)
            : base(context, units)
        {
            Context = context;
            // TranslationUnits = new List<TranslationUnit>(units);
        }

        public override string Generate()
        {
            // Check if generator type is Java
            if (Context.Options.IsJavaGenerator)
            {
                return GenerateUnformatted();
            }

            return base.Generate();
        }

        public override void GenerateFilePreamble(CommentKind kind, string generatorName = "pylon-node-gen")
        {
            var lines = new List<string>
            {
                "----------------------------------------------------------------------------",
                $"This is auto generated code by {generatorName}.",
                "Do not edit this file or all your changes will be lost after re-generation.",
                "----------------------------------------------------------------------------",
            };

            PushBlock(BlockKind.Header);
            GenerateMultiLineComment(lines, kind);
            PopBlock(NewLineKind.BeforeNextBlock);
        }

        public virtual void GenerateLegalFilePreamble(CommentKind kind, string creatorYear = "", string creatorName = "Björn Rennfanz <bjoern@fam-rennfanz.de>")
        {
            // Check if year is empty
            if (string.IsNullOrEmpty(creatorYear))
            {
                // Set copyright year to today
                creatorYear = DateTime.Now.ToString("yyyy");
            }

            var lines = new List<string>
            {
                "MIT License",
                "",
                $"Copyright (c) {creatorYear} {creatorName}",
                "",
                "Permission is hereby granted, free of charge, to any person obtaining a copy",
                "of this software and associated documentation files (the \"Software\"), to deal",
                "in the Software without restriction, including without limitation the rights",
                "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell",
                "copies of the Software, and to permit persons to whom the Software is",
                "furnished to do so, subject to the following conditions:",
                "",
                "The above copyright notice and this permission notice shall be included in all",
                "copies or substantial portions of the Software.",
                "",
                "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR",
                "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,",
                "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE",
                "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER",
                "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,",
                "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE",
                "SOFTWARE.",
                ""
            };

            PushBlock(BlockKind.Header);
            GenerateMultiLineComment(lines, kind);
            PopBlock();
        }
    }
}