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

using System;
using System.Collections.Generic;
using CppSharp.AST;
using CppSharp.Generators;
using NodePylonGen.Config;
using NodePylonGen.Parser.Model;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    /// <summary>
    /// C++/NodeJS wrapper generator responsible for 
    /// driving the generation of source and header files.
    /// </summary>
    public class NodeJSGenerator : Generator
    {
        public NodeJSGenerator(ConfigMapping config, DriverOptions driverOptions, CppModule mainModule)
            : base(new BindingContext(config, driverOptions, mainModule))
        {
            // Setup generator type
            Context.Options.GeneratorKind = GeneratorType.NodeJS;
        }

        public override bool SetupPasses()
        {
            return true;
        }

        public override List<CodeGenerator> GenerateCode(IEnumerable<TranslationUnit> units)
        {
            List<CodeGenerator> outputs = new List<CodeGenerator>();

            NodeJSHeaders header = new NodeJSHeaders(Context, units);
            outputs.Add(header);

            NodeJSSources source = new NodeJSSources(Context, units);
            outputs.Add(source);

            return outputs;
        }

        protected override string TypePrinterDelegate(CppSharp.AST.Type type)
        {
            throw new NotImplementedException();
        }
    }
}
