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
using NodePylonGen.Config;
using NodePylonGen.Parser.Model;
using NodePylonGen.Generator.Rules;
using System.Collections.Generic;

namespace NodePylonGen.Generator
{
    public class BindingContext
    {
        public ConfigMapping ConfigContext { get; private set; }
        public CppModule ModuleContext { get; private set; }

        public GeneratorType GeneratorKind { get; internal set; }

        public bool IsNodeJSGenerator => GeneratorKind == GeneratorType.NodeJS;
        public bool IsJavaGenerator => GeneratorKind == GeneratorType.Java;

        public RulesBuilder<CppIncludeRule> CppIncludeRules { get; private set; }
        //public RulesBuilder<GeneratorOutputPass> GeneratorOutputRules { get; private set; }

        public BindingContext(ConfigMapping configContext, CppModule moduleContext)
        {
            ConfigContext = configContext;
            ModuleContext = moduleContext;
        }

        public void ApplyRules()
        {
       
        }

        public string GetIncludePathOfUnit(CppInclude unit)
        {
            IncludeMapping includeOfUnit = null;

            IEnumerable<ConfigMapping> configFilesLoaded = ConfigContext.ConfigFilesLoaded;
            foreach (ConfigMapping configFileLoad in configFilesLoaded)
            {
                foreach (IncludeMapping include in configFileLoad.Includes)
                {
                    if (include.Id == unit.Name)
                    {
                        includeOfUnit = include;
                        break;
                    }
                }

                if (includeOfUnit != null)
                {
                    break;
                }
            }

            return (includeOfUnit != null) ? includeOfUnit.File : "invalid";
        }
    }
}
