using NodePylonGen.Config;
using NodePylonGen.Parser.Model;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System;

namespace NodePylonGen.Generator.Model
{
    public class TranslationUnitContext : CppElement
    {
        // List of translation units
        public List<TranslationUnit> TranslationUnits
        {
            get { return Iterate<TranslationUnit>().ToList(); }
        }

        /// <summary>
        /// Constructor for an translation unit
        /// </summary>
        public TranslationUnitContext(ConfigMapping configurationContext, CppModule moduleContext)
        {
            // Create root element
            Name = "translationunitscontext";
            Description = "Translation units context";
            Items = new List<CppElement>();

            // Convert CppIncludes to TranslationUnits
            foreach (CppInclude include in moduleContext.Includes)
            {
                // Convert CppInclude to TranslationUnit
                TranslationUnit translationUnit = new TranslationUnit(include)
                {
                    IncludePath = GetIncludePathOfCppInclude(configurationContext, include),
                    FilePath = GetFilePathOfCppInclude(configurationContext, include)
                };

                translationUnit.Parent = this;
                Items.Add(translationUnit);
            }
        }

        /// <summary>
        /// Get file path of cpp include
        /// </summary>
        private string GetFilePathOfCppInclude(ConfigMapping configurationContext, CppInclude unit)
        {
            ConfigMapping configOfUnit = null;
            IncludeMapping includeOfUnit = null;

            IEnumerable<ConfigMapping> configFilesLoaded = configurationContext.ConfigFilesLoaded;
            foreach (ConfigMapping configFileLoad in configFilesLoaded)
            {
                foreach (IncludeMapping include in configFileLoad.Includes)
                {
                    if (include.Id == unit.Name)
                    {
                        includeOfUnit = include;
                        configOfUnit = configFileLoad;
                        break;
                    }


                    if ((configOfUnit != null) && (includeOfUnit != null))
                    {
                        break;
                    }
                }
            }


            return (((configOfUnit != null) && (includeOfUnit != null)) ? System.IO.Path.Combine(System.IO.Path.GetDirectoryName(configOfUnit.AbsoluteFilePath), (string.IsNullOrEmpty(includeOfUnit.Alias) ? unit.Name : includeOfUnit.Alias.ToLower())) : "invalid") + ".gen";
        }

        /// <summary>
        /// Get include path for cpp include
        /// </summary>
        private string GetIncludePathOfCppInclude(ConfigMapping configurationContext, CppInclude unit)
        {
            IncludeMapping includeOfUnit = null;
            
            IEnumerable<ConfigMapping> configFilesLoaded = configurationContext.ConfigFilesLoaded;
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

        public override T Visit<T>(ICppElementVisitor<T> visitor)
        {
            return visitor.VisitCppElement(this);
        }
    }
}
