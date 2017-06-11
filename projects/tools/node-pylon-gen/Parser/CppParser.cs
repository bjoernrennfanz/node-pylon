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

using log4net;
using System;
using System.Collections.Generic;
using System.Reflection;
using NodePylonGen.Config;
using System.IO;
using NodePylonGen.Parser.Model;
using System.Xml.Linq;
using NodePylonGen.Utils;
using static NodePylonGen.Parser.CastXML;

namespace NodePylonGen.Parser
{
    public class CppParser
    {
        private static readonly ILog log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private const string Version = "0.0.1";

        private bool masterConfigHasChanged;
        private ConfigMapping masterConfig;
        private string masterHeaderFile;

        private CppModule masterCppModule;
        private CastXML castXML;

        private Dictionary<string, bool> includeToProcess = new Dictionary<string, bool>();
        private Dictionary<string, XElement> mapIdToXElement = new Dictionary<string, XElement>();
        private Dictionary<string, List<XElement>> mapFileToXElement = new Dictionary<string, List<XElement>>();
        private CppInclude currentCppInclude;

        public CppParser(string castXmlExecutablePath, string vcToolsPath)
        {
            CastXmlExecutablePath = castXmlExecutablePath;
            VcToolsPath = vcToolsPath;
            ForceParsing = false;
        }

        /// <summary>
        /// Gets the CastXML executable path.
        /// </summary>
        public string CastXmlExecutablePath { get; private set; }

        /// <summary>
        /// Gets or sets the path to the Visual C++ toolset
        /// </summary>
        public string VcToolsPath { get; private set; }

        /// <summary>
        /// Gets or sets a value indicating whether this <see cref="CppParser"/> should force to parse headers.
        /// </summary>
        public bool ForceParsing { get; set; }

        /// <summary>
        /// Initialize this Parser from a config file.
        /// </summary>
        public void Initialize(ConfigMapping config)
        {
            masterConfig = config;
            masterHeaderFile = masterConfig.Id + ".hpp";

            // Config is updated if ForceParsing is true
            masterConfigHasChanged = ForceParsing;

            // Create CppModule instance
            masterCppModule = new CppModule();

            // Create CastXml instance
            castXML = new CastXML();
            castXML.ExecutablePath = CastXmlExecutablePath;
            castXML.VcToolsPath = VcToolsPath;

            // Add current directory for gccxml
            castXML.IncludeDirs.Add(new IncludeDirMapping(Environment.CurrentDirectory));

            // Configure gccxml with include directory
            foreach (ConfigMapping configFile in masterConfig.ConfigFilesLoaded)
            {
                // Add all include directories
                foreach (IncludeDirMapping includeDir in configFile.IncludeDirs)
                {
                    castXML.IncludeDirs.Add(includeDir);
                }
            }

            // Check if the file has any includes related config
            List<string> filesWithIncludes = new List<string>();
            foreach (ConfigMapping configFile in masterConfig.ConfigFilesLoaded)
            {
                bool isWithInclude = false;

                // Add this config file as an include to process
                includeToProcess.Add(configFile.Id, true);

                if (configFile.IncludeDirs.Count > 0)
                {
                    isWithInclude = true;
                }

                if (configFile.Includes.Count > 0)
                {
                    isWithInclude = true;
                }

                if (configFile.References.Count > 0)
                {
                    isWithInclude = true;
                }

                // If this config file has any include rules
                if (isWithInclude)
                {
                    filesWithIncludes.Add(configFile.Id);
                }
            }

            // Write include files
            foreach (ConfigMapping configFile in masterConfig.ConfigFilesLoaded)
            {
                // Check if config have includes
                if (!filesWithIncludes.Contains(configFile.Id))
                {
                    // Skip, process next config
                    continue;
                }

                var outputConfig = new StringWriter();
                outputConfig.WriteLine("// pylon-node include config for {0} - Version {1}", configFile.Id, Version);
                outputConfig.WriteLine("// Do not edit this file, is generated by pylon-node-gen");
                outputConfig.WriteLine("//");
                outputConfig.WriteLine();

                // Write includes
                foreach (IncludeMapping includeMapping in configFile.Includes)
                {
                    CppInclude cppInclude = masterCppModule.FindInclude(includeMapping.Id);
                    if (cppInclude == null)
                    {
                        includeToProcess.Add(includeMapping.Id, true);

                        cppInclude = new CppInclude();
                        cppInclude.Name = includeMapping.Id;
                        masterCppModule.Add(cppInclude);
                    }

                    outputConfig.WriteLine("#include <{0}>", includeMapping.File);
                }

                // Write includes to references
                foreach (ConfigMapping reference in configFile.References)
                {
                    if (filesWithIncludes.Contains(reference.Id))
                    {
                        outputConfig.WriteLine("#include \"{0}\"", reference.Id + ".hpp");
                    }
                }

                outputConfig.Close();

                string outputConfigString = outputConfig.ToString();
                string outputConfigFile = configFile.Id + ".hpp";

                // Test if Last config file was generated. If not, then we need to generate it
                // If it exists, then we need to test if it is the same than previous run
                configFile.IsConfigUpdated = ForceParsing;

                if (File.Exists(outputConfigFile) && !ForceParsing)
                {
                    configFile.IsConfigUpdated = outputConfigString != File.ReadAllText(outputConfigFile);
                }
                else
                {
                    configFile.IsConfigUpdated = true;
                }

                // Just write the header file when the file is updated or new
                if (configFile.IsConfigUpdated)
                {
                    log.Info("Config file changed for C++ headers (module: " + configFile.Id + ", config: " + configFile.FilePath + ")");
                    masterConfigHasChanged = true;

                    var fileWriter = new StreamWriter(outputConfigFile);
                    fileWriter.Write(outputConfigString);
                    fileWriter.Close();
                }
            }
        }

        /// <summary>
        /// Gets the name of the generated CastXML file.
        /// </summary>
        private string CastXmlFileName
        {
            get { return masterConfig.Id + "-castxml.xml"; }
        }

        /// <summary>
        /// Gets the name of the C++ parsed XML file.
        /// </summary>
        private string MasterModuleFileName
        {
            get { return masterConfig.Id + "-parsed.xml"; }
        }

        /// <summary>
        /// Gets or sets the CastXml doc.
        /// </summary>
        private XDocument CastXmlDoc { get; set; }

        /// <summary>
        /// Runs this instance.
        /// </summary>
        /// <returns></returns>
        public CppModule Run()
        {
            // If config is updated, we need to run the 
            if (masterConfigHasChanged)
            {
                log.Info("Config files changed.");
            }

            StreamReader xmlReader = null;
            try
            {
                xmlReader = castXML.Process(masterHeaderFile);
                if (xmlReader != null)
                {
                    Parse(xmlReader);
                }

                // Save back the C++ parsed includes
                // masterCppModule.Write(MasterModuleFileName);
            }
            catch (Exception ex)
            {
                log.Error("Unexpected error", ex);
            }


            return masterCppModule;
        }

        /// <summary>
        /// Parses the specified reader.
        /// </summary>
        /// <param name="reader">The reader.</param>
        private void Parse(StreamReader reader)
        {
            XDocument doc = XDocument.Load(reader);
            CastXmlDoc = doc;

            // Collects all GccXml elements and build map from their id
            foreach (var xElement in doc.Elements("GCC_XML").Elements())
            {
                XAttribute id = xElement.Attribute("id");
                if (id != null)
                {
                    mapIdToXElement.Add(id.Value, xElement);
                }

                XAttribute file = xElement.Attribute("file");
                if (file != null)
                {
                    List<XElement> elementsInFile;
                    if (!mapFileToXElement.TryGetValue(file.Value, out elementsInFile))
                    {
                        elementsInFile = new List<XElement>();
                        mapFileToXElement.Add(file.Value, elementsInFile);
                    }
                    elementsInFile.Add(xElement);
                }
            }

            // Build CastXml structure type list
            List<string> castXmlStructureTags = new List<string>();
            castXmlStructureTags.Add(StringEnum.GetStringValue(CastXMLTag.Struct));
            castXmlStructureTags.Add(StringEnum.GetStringValue(CastXMLTag.Union));
            castXmlStructureTags.Add(StringEnum.GetStringValue(CastXMLTag.Enumeration));

            // Fix all structure names
            foreach (XElement xTypedef in doc.Elements("GCC_XML").Elements())
            {
                if (xTypedef.Name.LocalName == StringEnum.GetStringValue(CastXMLTag.Typedef))
                {
                    XElement xStruct = mapIdToXElement[xTypedef.Attribute("type").Value];
                    if (castXmlStructureTags.Contains(xStruct.Name.LocalName))
                    {
                        string structName = xStruct.Attribute("name").Value;
                        // Rename all structure starting with tagXXXX to XXXX
                        if (structName.StartsWith("tag") || structName.StartsWith("_") || string.IsNullOrEmpty(structName))
                        {
                            string typeName = xTypedef.Attribute("name").Value;
                            xStruct.SetAttributeValue("name", typeName);

                            log.Debug("Use typedef to rename [" + structName + "] to [" + typeName + "]");
                        }
                    }
                }
            }

            // Find all elements that are referring to a context and attach them to 
            // the context as child elements
            foreach (XElement xElement in mapIdToXElement.Values)
            {
                XAttribute id = xElement.Attribute("context");
                if (id != null)
                {
                    xElement.Remove();
                    mapIdToXElement[id.Value].Add(xElement);
                }
            }

            ParseAllElements();
        }

        /// <summary>
        /// Gets the include id from the file id.
        /// </summary>
        private string GetIncludeIdFromFileId(string fileId)
        {
            XAttribute filePath = mapIdToXElement[fileId].Attribute("name");
            if (filePath != null)
            {
                if (!File.Exists(filePath.Value))
                {
                    return "";
                }

                return Path.GetFileNameWithoutExtension(filePath.Value).ToLower();
            }

            return "";
        }

        /// <summary>
        /// Parses all C++ elements. This is the main method that iterates on all types.
        /// </summary>
        private void ParseAllElements()
        {
            foreach (string includeCastXmlId in mapFileToXElement.Keys)
            {
                // Process only files listed inside the config files
                string includeId = GetIncludeIdFromFileId(includeCastXmlId);
                if (!includeToProcess.ContainsKey(includeId))
                {
                    continue;
                }

                // Log current include being processed
                log.Info("Process include: " + includeId + ".h");

                // Try to find CppInclude instance
                currentCppInclude = masterCppModule.FindInclude(includeId);
                if (currentCppInclude == null)
                {
                    currentCppInclude = new CppInclude() { Name = includeId };
                    masterCppModule.Add(currentCppInclude);
                }

                foreach (XElement xElement in mapFileToXElement[includeCastXmlId])
                {
                    // If incomplete flag, than element cannot be parsed
                    if (xElement.Attribute("incomplete") != null)
                    {
                        continue;
                    }

                    CppElement cppElement = null;
                    string name = xElement.Name.LocalName;
                    string elementName = xElement.Attribute("name").Value;

                    if (name == StringEnum.GetStringValue(CastXMLTag.Enumeration))
                    {
                        cppElement = ParseEnum(xElement);
                    }
                    else if (name == StringEnum.GetStringValue(CastXMLTag.Function))
                    {
                        cppElement = ParseFunction(xElement);
                    }
                    else if (name == StringEnum.GetStringValue(CastXMLTag.Struct))
                    {
                        if (xElement.Attribute("abstract") != null)
                        {
                            cppElement = ParseInterface(xElement);
                        }
                        else
                        {
                            cppElement = ParseStructOrUnion(xElement);
                        }
                    }
                    else if (name == StringEnum.GetStringValue(CastXMLTag.Union))
                    {
                        cppElement = ParseStructOrUnion(xElement);
                    }
                    else if (name == StringEnum.GetStringValue(CastXMLTag.Variable))
                    {
                        if (xElement.Attribute("init") != null)
                        {
                            cppElement = ParseVariable(xElement);
                        }
                    }
                    else if (name == StringEnum.GetStringValue(CastXMLTag.Class))
                    {
                        cppElement = ParseClass(xElement);
                    }

                    // Check if cppElement was parsed
                    if (cppElement != null)
                    {
                        currentCppInclude.Add(cppElement);
                    }
                }
            }
        }

        private CppElement ParseClass(XElement xElement)
        {
            // If element is already transformed, return it
            CppClass cppClass = xElement.Annotation<CppClass>();
            if (cppClass != null)
            {
                return cppClass;
            }

            // Else, create a new CppInterface
            cppClass = new CppClass();
            cppClass.Name = xElement.Attribute("name").Value;
            xElement.AddAnnotation(cppClass);

            return cppClass;
        }

        private CppElement ParseVariable(XElement xElement)
        {
            return null;
        }

        private CppElement ParseStructOrUnion(XElement xElement)
        {
            return null;
        }

        private CppElement ParseInterface(XElement xElement)
        {
            return null;
        }

        private CppElement ParseFunction(XElement xElement)
        {
            return null;
        }

        private CppElement ParseEnum(XElement xElement)
        {
            return null;
        }
    }
}