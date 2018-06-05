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
using CppSharp.Passes;
using GenPylonBinding.Core.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using BindingContext = GenPylonBinding.Generators.BindingContext;

namespace GenPylonBinding.Generator.Generators.NodeJS
{
    public class AddMissingModulePass : TranslationUnitPass
    {
        // Hide base member context
        private new BindingContext Context;

        public AddMissingModulePass(BindingContext context)
        {
            // Set NodeJS binding context
            Context = context;
        }

        public override bool VisitASTContext(ASTContext context)
        {
            SortedList<string, TranslationUnit> missingTranslationUnits = new SortedList<string, TranslationUnit>();
                
            // Get list of generated units
            List<TranslationUnit> units = Context.ASTContext.TranslationUnits.GetGenerated().ToList();
            foreach (TranslationUnit unit in units)
            {
                NodeJSTypeReferenceCollector typeReferenceCollector = new NodeJSTypeReferenceCollector(Context.ConfigurationContext, Context.TypeMaps, Context.Options);
                typeReferenceCollector.Process(unit);

                IEnumerable<NodeJSTypeReference> classToWrapTypeReferences = typeReferenceCollector.TypeReferences
                    .Where(item => item.Declaration is Class);

                // Check if no class reference where found
                if (classToWrapTypeReferences.Count() > 0)
                {
                    foreach (NodeJSTypeReference currentReference in classToWrapTypeReferences)
                    {
                        // Check if missing unit was already added
                        if (missingTranslationUnits.ContainsKey(currentReference.Declaration.Name))
                            continue;

                        // Check if missing unit exists with trimmed name
                        if (units.Exists(u => NamingHelper.GenerateTrimmedClassName(u.FileNameWithoutExtension).ToLower().Contains(NamingHelper.GenerateTrimmedClassName(currentReference.Declaration.Name).ToLower())))
                            continue;

                        // Make deep copy of current unit
                        TranslationUnit missingTranslationUnit = unit.Copy();

                        // Change names of copy
                        missingTranslationUnit.Name = currentReference.Declaration.Name;
                        missingTranslationUnit.OriginalName = currentReference.Declaration.Name;

                        // Change public fields
                        string missingTranslationUnitFileName = NamingHelper.GenerateTrimmedClassName(currentReference.Declaration.Name).ToLower() + ".gen";
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

            return true;
        }
    }
}
