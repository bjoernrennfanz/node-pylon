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
using BindingContext = NodePylonGen.Generators.BindingContext;
using CppSharp.AST;
using NodePylonGen.Config;
using NodePylonGen.Parser.Model;
using System.Linq;
using NodePylonGen.Utils;
using System.Collections;
using System.IO;
using System.Reflection;

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

        public override void Process()
        {
            SortedList<string, TranslationUnit> missingTranslationUnits = new SortedList<string, TranslationUnit>();

            // Get list of generated units
            List<TranslationUnit> units = Context.ASTContext.TranslationUnits.GetGenerated().ToList();
            foreach(TranslationUnit unit in units)
            {
                NodeJSTypeReferenceCollector typeReferenceCollector = new NodeJSTypeReferenceCollector(Context.ConfigurationContext, Context.TypeMaps, Context.Options);
                typeReferenceCollector.Process(unit);

                NodeJSTypeReference classToWrapTypeReference = typeReferenceCollector.TypeReferences
                    .Where(item => item.Declaration is Class)
                    .Where(item => unit.FileNameWithoutExtension.ToLower().Contains(NodeJSClassHelper.GenerateTrimmedClassName(item.Declaration.Name).ToLower()))
                    .FirstOrDefault();

                IEnumerable<NodeJSTypeReference> classToWrapTypeReferences = typeReferenceCollector.TypeReferences
                    .Where(item => item.Declaration is Class);

                // Check if no class reference where found
                if ((classToWrapTypeReference == null) && (classToWrapTypeReferences.Count() > 0))
                {
                    foreach (NodeJSTypeReference currentReference in classToWrapTypeReferences)
                    {
                        // Check if missing unit was already added
                        if (missingTranslationUnits.ContainsKey(currentReference.Declaration.Name))
                            continue;

                        // Check if missing unit exists with trimmed name
                        if (units.Exists(u => NodeJSClassHelper.GenerateTrimmedClassName(u.FileNameWithoutExtension).ToLower().Contains(NodeJSClassHelper.GenerateTrimmedClassName(currentReference.Declaration.Name).ToLower())))
                            continue;

                        // Make deep copy of current unit
                        TranslationUnit missingTranslationUnit = unit.Copy();

                        // Change names of copy
                        missingTranslationUnit.Name = currentReference.Declaration.Name;
                        missingTranslationUnit.OriginalName = currentReference.Declaration.Name;

                        // Change public fields
                        string missingTranslationUnitFileName = NodeJSClassHelper.GenerateTrimmedClassName(currentReference.Declaration.Name).ToLower() + ".gen";
                        string missingTranslationUnitFileRelativePath = unit.FileRelativePath;

                        // Update public fields
                        missingTranslationUnit.FilePath = Path.Combine(Path.GetDirectoryName(unit.FilePath), missingTranslationUnitFileName);

                        // Update private fields with reflection
                        FieldInfo fileNameFieldInfo = missingTranslationUnit.GetType().GetField("fileName", BindingFlags.NonPublic | BindingFlags.Instance);
                        fileNameFieldInfo.SetValue(missingTranslationUnit, missingTranslationUnitFileName);

                        FieldInfo fileNameWithoutExtensionFieldInfo = missingTranslationUnit.GetType().GetField("fileNameWithoutExtension", BindingFlags.NonPublic | BindingFlags.Instance);
                        fileNameWithoutExtensionFieldInfo.SetValue(missingTranslationUnit, Path.GetFileNameWithoutExtension(missingTranslationUnitFileName));

                        FieldInfo fileRelativeDirectoryFieldInfo = missingTranslationUnit.GetType().GetField("fileRelativeDirectory", BindingFlags.NonPublic | BindingFlags.Instance);
                        fileRelativeDirectoryFieldInfo.SetValue(missingTranslationUnit, Path.GetDirectoryName(missingTranslationUnitFileRelativePath));

                        FieldInfo fileRelativePathFieldInfo = missingTranslationUnit.GetType().GetField("fileRelativePath", BindingFlags.NonPublic | BindingFlags.Instance);
                        fileRelativePathFieldInfo.SetValue(missingTranslationUnit, Path.Combine(Path.GetDirectoryName(missingTranslationUnitFileRelativePath), missingTranslationUnitFileName));

                        // Add to collection for later processing
                        missingTranslationUnits.Add(currentReference.Declaration.Name, missingTranslationUnit);
                    }
                }
            }

            // Insert missing items
            Context.ASTContext.TranslationUnits.AddRange(missingTranslationUnits.Values);
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
