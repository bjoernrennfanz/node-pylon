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

using CppSharp.AST;
using NodePylonGen.Generators;
using System;
using System.Collections.Generic;
using System.Linq;

using CodeGenerator = NodePylonGen.Generator.Generators.CodeGenerator;

namespace NodePylonGen.Generator
{
    /// <summary>
    /// Kinds of language generators.
    /// </summary>
    public enum GeneratorType
    {
        NodeJS = 1,
        Java
    }

    /// <summary>
    /// Output generated by each back end generator.
    /// </summary>
    public struct GeneratorOutput
    {
        /// <summary>
        /// Cpp include associated with output.
        /// </summary>
        public TranslationUnit TranslationUnit;

        /// <summary>
        /// Code generators with generated output.
        /// </summary>
        public List<CodeGenerator> Outputs;
    }

    public abstract class Generator : CppSharp.Generators.Generator
    {
        /// <summary>
        /// Gets the binding context of this generator instance.
        /// </summary>
        public new BindingContext Context { get; private set; }

        /// <summary>
        /// Called when a translation unit is generated.
        /// </summary>
        public new Action<GeneratorOutput> OnUnitGenerated = delegate { };

        /// <summary>
        /// Construct an new <see cref="Generator"/> instance. 
        /// </summary>
        protected Generator(BindingContext context)
            : base(context)
        {
            Context = context;
        }

        /// <summary>
        /// Generate the outputs for given context.
        /// </summary>
        public void ProcessCode()
        {
            Context.RunPasses();
            Process();
        }

        /// <summary>
        /// Generates the source files
        /// </summary>
        /// <returns></returns>
        public List<GeneratorOutput> GenerateCode()
        {
            List<GeneratorOutput> outputs = new List<GeneratorOutput>();
            List<TranslationUnit> units = Context.ASTContext.TranslationUnits.GetGenerated().ToList();

            if (Context.Options.IsJavaGenerator)
            {
               GenerateSingleTemplate(outputs, units);
            }
            else
            {
               GenerateTemplates(outputs, units);
            }
            
            return outputs;
        }

        public override List<CppSharp.Generators.CodeGenerator> Generate(IEnumerable<TranslationUnit> units)
        {
            List<CodeGenerator> codeGenerators = GenerateCode(units);
            List<CppSharp.Generators.CodeGenerator> result = new List<CppSharp.Generators.CodeGenerator>();

            foreach (CodeGenerator generator in codeGenerators)
            {
                result.Add(generator as CppSharp.Generators.CodeGenerator);
            }

            return result;
        }

        public abstract List<CodeGenerator> GenerateCode(IEnumerable<TranslationUnit> units);

        /// <summary>
        /// Generate templates for output
        /// </summary>
        private void GenerateTemplates(List<GeneratorOutput> outputs, IEnumerable<TranslationUnit> units)
        {
            foreach (TranslationUnit unit in units)
            {
                List<CodeGenerator> templates = GenerateCode(new[] { unit });
                if (templates.Count == 0)
                {
                    return;
                }

                foreach (CodeGenerator template in templates)
                {
                    template.Process();
                }

                GeneratorOutput output = new GeneratorOutput
                {
                    TranslationUnit = unit,
                    Outputs = templates
                };

                outputs.Add(output);
                OnUnitGenerated(output);
            }
        }

        /// <summary>
        /// Generate single template for output
        /// </summary>
        private void GenerateSingleTemplate(List<GeneratorOutput> outputs, IEnumerable<TranslationUnit> units)
        {
            foreach (TranslationUnit unit in units)
            {
                GeneratorOutput output = new GeneratorOutput
                {
                    TranslationUnit = unit,
                    Outputs = GenerateCode(new[] { unit })
                };

                output.Outputs.FirstOrDefault().Process();
                outputs.Add(output);

                OnUnitGenerated(output);
            }
        }
    }
}
