using NodePylonGen.Config;
using NodePylonGen.Parser.Model;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NodePylonGen.Generator.Model
{
    public class TranslationUnitContext
    {
        public List<TranslationUnit> TranslationUnits;

        public TranslationUnitContext(ConfigMapping configurationContext, CppModule moduleContext)
        {
            TranslationUnits = new List<TranslationUnit>();

            // Convert CppIncludes to TranslationUnits
            foreach (CppInclude include in moduleContext.Includes)
            {
                TranslationUnit translationUnit = new TranslationUnit(include)
                {
                    IncludePath = GetIncludePathOfCppInclude(configurationContext, include),
                    FilePath = GetFilePathOfCppInclude(configurationContext, include)
                };

                TranslationUnits.Add(translationUnit);
            }
        }

        private string GetFilePathOfCppInclude(ConfigMapping configurationContext, CppInclude unit)
        {
            ConfigMapping configOfUnit = null;
            IEnumerable<ConfigMapping> configFilesLoaded = configurationContext.ConfigFilesLoaded;

            foreach (ConfigMapping configFileLoad in configFilesLoaded)
            {
                foreach (IncludeMapping include in configFileLoad.Includes)
                {
                    if (include.Id == unit.Name)
                    {
                        configOfUnit = configFileLoad;
                        break;
                    }


                    if (configOfUnit != null)
                    {
                        break;
                    }
                }
            }

            return ((configOfUnit != null) ? Path.Combine(Path.GetDirectoryName(configOfUnit.AbsoluteFilePath), unit.Name) : "invalid") + ".gen";
        }

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

    }
}
