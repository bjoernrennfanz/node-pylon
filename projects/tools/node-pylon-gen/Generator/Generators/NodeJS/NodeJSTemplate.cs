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

using CppSharp.AST;
using System.Collections.Generic;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    public struct Include
    {
        public enum IncludeType
        {
            Angled,
            Quoted
        }

        public string File;
        //public TranslationUnit TranslationUnit;

        public IncludeType Type;
        public bool InHeader;

        public override string ToString()
        {
            return string.Format(Type == IncludeType.Angled ? "#include <{0}>" : "#include \"{0}\"", File);
        }
    }

    public abstract class NodeJSTemplate : CodeGenerator
    {
        public ISet<Include> Includes;

        protected NodeJSTemplate(BindingContext context, IEnumerable<TranslationUnit> units)
            : base(context, units)
        {
            Includes = new HashSet<Include>();
        }

        public abstract override string FileExtension { get; }

        public abstract override void Process();

    }
}