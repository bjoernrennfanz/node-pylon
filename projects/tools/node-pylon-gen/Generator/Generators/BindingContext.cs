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

using NodePylonGen.Config;
using NodePylonGen.Generator.Model;
using NodePylonGen.Parser.Model;

namespace NodePylonGen.Generator
{
    public class BindingContext : CppSharp.Generators.BindingContext
    {
        public ConfigMapping ConfigurationContext { get; private set; }
        public CppModule ParserContext { get; private set; }

        /// <summary>
        /// Gets the driver options of this instance
        /// </summary>
        public new DriverOptions Options { get; private set; }

        public BindingContext(ConfigMapping configurationContext, DriverOptions driverOptions, CppModule moduleContext)
            : base(driverOptions)
        {
            ConfigurationContext = configurationContext;
            ParserContext = moduleContext;
            Options = driverOptions;

            ASTConverter converter = new ASTConverter(configurationContext, moduleContext);
            ASTContext = converter.Convert();
            
        }
    }
}
