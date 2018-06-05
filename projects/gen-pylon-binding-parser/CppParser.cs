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

using GenPylonBinding.Core.Config;
using GenPylonBinding.Core.Utils;
using GenPylonBinding.Parser.Model;
using log4net;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using static GenPylonBinding.Parser.CastXML;

namespace GenPylonBinding.Parser
{
    public class CppParser
    {
        private static readonly ILog log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private const string Version = "0.0.1";

        private bool masterConfigHasChanged;
        private ConfigMapping masterConfig;
        private CppModule masterCppModule;
        private string masterHeaderFile;
                
        private Dictionary<string, bool> includeToProcess = new Dictionary<string, bool>();
        private Dictionary<string, int> mapIncludeToAnonymousEnumCount = new Dictionary<string, int>();
        private Dictionary<string, XElement> mapIdToXElement = new Dictionary<string, XElement>();
        private Dictionary<string, List<XElement>> mapFileToXElement = new Dictionary<string, List<XElement>>();

        private CastXML castXML;
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


                StreamReader xmlReader = null;
                try
                {
                    xmlReader = castXML.Process(masterHeaderFile);
                    if (xmlReader != null)
                    {
                        Parse(xmlReader);
                    }

                    // Save back the C++ parsed includes
                    masterCppModule.Write(MasterModuleFileName);
                }
                catch (Exception ex)
                {
                    log.Error("Unexpected error", ex);
                }
                finally
                {
                    if (xmlReader != null)
                    {
                        xmlReader.Close();
                    }

                    // Write back GCCXML document on the disk
                    if (CastXmlDoc != null)
                    {
                        CastXmlDoc.Save(CastXmlFileName);
                    }

                    log.Info("Parsing headers is finished.");
                }
            }
            else
            {
                masterCppModule = CppModule.Read(MasterModuleFileName);
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

                    // Parse current element
                    CppElement cppElement = ParseElement(xElement);

                    // Check if cppElement was parsed
                    if (cppElement != null)
                    {
                        // Parse complete namespace
                        XAttribute contextAttribute = xElement.Attribute("context");
                        if ((contextAttribute != null) && string.IsNullOrEmpty(cppElement.Namespace))
                        {
                            XElement parentContext = mapIdToXElement[contextAttribute.Value];
                            if (parentContext != null)
                            {
                                cppElement.Namespace = ParseNamespace(parentContext);
                            }
                        }

                        currentCppInclude.Add(cppElement);
                    }
                }
            }
        }

        private string ParseNamespace(XElement xElement, Stack<string> namespaceNames = null)
        {
            if (namespaceNames == null)
            {
                namespaceNames = new Stack<string>();
            }

            XAttribute contextAttribute = xElement.Attribute("context");
            if (contextAttribute != null)
            {
                XElement parentContext = mapIdToXElement[contextAttribute.Value];
                string parentElementName = ((parentContext != null) ? parentContext.Name.LocalName : string.Empty);
                if (parentElementName == StringEnum.GetStringValue(CastXMLTag.Namespace))
                {
                    ParseNamespace(parentContext, namespaceNames);
                }
            }

            if (xElement.Name.LocalName == StringEnum.GetStringValue(CastXMLTag.Namespace))
            {
                string nameSpaceName = xElement.Attribute("name").Value;
                if (nameSpaceName != "::")
                {
                    namespaceNames.Push(nameSpaceName);
                }
            }

            return "::" + string.Join("::", namespaceNames.Reverse().ToArray());
        }

        /// <summary>
        /// Parse C++ element from given <see cref="XElement"/> 
        /// </summary>
        private CppElement ParseElement(XElement xElement)
        {
            CppElement cppElement = null;
            string name = xElement.Name.LocalName;

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

            return cppElement;
        }

        /// <summary>
        /// Parse C++ class from given <see cref="XElement"/> 
        /// </summary>
        private CppElement ParseClass(XElement xElement)
        {
            return ParseClassOrInterface<CppClass>(xElement);
        }

        /// <summary>
        /// Parse C++ interface from given <see cref="XElement"/> 
        /// </summary>
        private CppElement ParseInterface(XElement xElement)
        {
            return ParseClassOrInterface<CppInterface>(xElement);
        }

        /// <summary>
        /// Parse C++ class or interface from given <see cref="XElement"/> 
        /// </summary>
        private T ParseClassOrInterface<T>(XElement xElement) where T : CppBase, new()
        {
            // If element is already transformed, return it
            T cppBaseType = xElement.Annotation<T>();
            if (cppBaseType != null)
            {
                return cppBaseType;
            }

            // Else, create a new CppInterface
            cppBaseType = new T();
            cppBaseType.Name = xElement.Attribute("name").Value;
            cppBaseType.Id = xElement.Attribute("id").Value;
            xElement.AddAnnotation(cppBaseType);

            // Calculate offsets using inheritance
            int offsetMethod = 0;
            int offsetConstructor = 0;

            XAttribute basesAttribute = xElement.Attribute("bases");
            IEnumerable<string> bases = basesAttribute != null ? basesAttribute.Value.Split(' ') : Enumerable.Empty<string>();
            foreach (string xElementBaseId in bases)
            {
                if (string.IsNullOrEmpty(xElementBaseId))
                {
                    continue;
                }
                    
                XElement xElementBase = mapIdToXElement[xElementBaseId];
                CppElement cppElementBase = ParseElement(xElementBase);

                if (string.IsNullOrEmpty(cppBaseType.ParentName))
                {
                    cppBaseType.ParentName = cppElementBase.Name;
                }

                // Get methods count from base class or interface
                offsetMethod += ((cppElementBase is CppBase) ? ((CppBase)cppElementBase).TotalMethodCount : 0);
                offsetConstructor += ((cppElementBase is CppBase) ? ((CppBase)cppElementBase).TotalConstructorCount : 0);
            }

            List<CppConstructor> constructors = new List<CppConstructor>();
            List<CppMethod> methods = new List<CppMethod>();

            // Parse methods
            foreach (XElement element in xElement.Elements())
            {
                string overrides = String.Empty;
                XAttribute overridesAttribute = element.Attribute("overrides");
                if (overridesAttribute != null)
                {
                    overrides = overridesAttribute.Value;
                }

                string pureVirtual = String.Empty;
                XAttribute pureVirtualAttribute = element.Attribute("pure_virtual");
                if (pureVirtualAttribute != null)
                {
                    pureVirtual = pureVirtualAttribute.Value;
                }

                string access = String.Empty;
                XAttribute accessAttribute = element.Attribute("access");
                if (accessAttribute != null)
                {
                    access = accessAttribute.Value;
                }

                // Parse method with pure virtual (=0) and that do not override any other methods
                if (element.Name.LocalName == "Method")
                {
                    CppMethod cppMethod = ParseMethodOrFunction<CppMethod>(element);

                    if (!string.IsNullOrWhiteSpace(pureVirtual))
                    {
                        cppMethod.Virtual = true;
                    }

                    if (!string.IsNullOrWhiteSpace(overrides))
                    {
                        cppMethod.Override = true;
                    }

                    if (!string.IsNullOrWhiteSpace(access))
                    {
                        cppMethod.Access = access;
                    }

                    methods.Add(cppMethod);
                }
                else if (element.Name.LocalName == "Constructor")
                {
                    CppConstructor cppConstructor = ParseConstructor(element);

                    if (!string.IsNullOrWhiteSpace(access))
                    {
                        cppConstructor.Access = access;
                    }

                    constructors.Add(cppConstructor);
                }
            }

            // The Visual C++ compiler breaks the rules of the COM ABI when overloaded methods are used.
            // It will group the overloads together in memory and lay them out in the reverse of their declaration order.
            // Since GCC always lays them out in the order declared, we have to modify the order of the methods to match Visual C++.
            // See http://support.microsoft.com/kb/131104 for more information.
            for (int i = 0; i < methods.Count; i++)
            {
                string name = methods[i].Name;

                // Look for overloads of this function
                for (int j = i + 1; j < methods.Count; j++)
                {
                    var nextMethod = methods[j];
                    if (nextMethod.Name == name)
                    {
                        // Remove this one from its current position further into the vtable
                        methods.RemoveAt(j);

                        // Put this one before all other overloads (aka reverse declaration order)
                        int k = i - 1;
                        while (k >= 0 && methods[k].Name == name)
                            k--;
                        methods.Insert(k + 1, nextMethod);
                        i++;
                    }
                }
            }

            // Add the methods to the cppbase with the correct offsets
            foreach (CppMethod cppMethod in methods)
            {
                
                cppMethod.Offset = offsetMethod++;
                cppBaseType.Add(cppMethod);
            }

            // Add the constructor to the cppbase with the correct offsets
            foreach (CppConstructor cppConstructor in constructors)
            {

                cppConstructor.Offset = offsetConstructor++;
                cppBaseType.Add(cppConstructor);
            }

            cppBaseType.TotalConstructorCount = offsetConstructor;
            cppBaseType.TotalMethodCount = offsetMethod;

            return cppBaseType;
        }

        /// <summary>
        /// Parses a C++ method or function.
        /// </summary>
        private T ParseMethodOrFunction<T>(XElement xElement) where T : CppMethod, new()
        {
            T cppMethod = new T();
            cppMethod.Name = xElement.Attribute("name").Value;
            cppMethod.Id = xElement.Attribute("id").Value;

            // Parse parameters
            ParseParameters(xElement, cppMethod);

            // Parse return type
            cppMethod.ReturnType = new CppType();
            XAttribute returnsAttribute = xElement.Attribute("returns");
            if (returnsAttribute != null)
            {
                ResolveAndFillType(returnsAttribute.Value, cppMethod.ReturnType);
            }

            return cppMethod;
        }

        /// <summary>
        /// Parses a C++ constructor.
        /// </summary>
        private CppConstructor ParseConstructor(XElement xElement)
        {
            CppConstructor cppConstructor = new CppConstructor();
            cppConstructor.Name = xElement.Attribute("name").Value;
            cppConstructor.Id = xElement.Attribute("id").Value;

            // Parse parameters
            ParseParameters(xElement, cppConstructor);

            return cppConstructor;
        }

        /// <summary>
        /// Parses a C++ parameters.
        /// </summary>
        private void ParseParameters(XElement xElement, CppElement constructorOrMethodOrFunction)
        {
            int paramCount = 0;
            foreach (XElement parameter in xElement.Elements())
            {
                if (parameter.Name.LocalName != "Argument")
                {
                    continue;
                }

                CppParameter cppParameter = new CppParameter();
                XAttribute nameAttribute = parameter.Attribute("name");
                if (nameAttribute != null)
                {
                    cppParameter.Name = nameAttribute.Value;
                }
                
                if (string.IsNullOrEmpty(cppParameter.Name))
                {
                    cppParameter.Name = "arg" + paramCount;
                }

                // All parameters without any annotations are considerate as In
                if (cppParameter.Attribute == ParameterAttributeMapping.None)
                {
                    cppParameter.Attribute = ParameterAttributeMapping.In;
                }
                    
                XAttribute typeAttribute = parameter.Attribute("type");
                if (typeAttribute != null)
                {
                    ResolveAndFillType(typeAttribute.Value, cppParameter);
                }

                constructorOrMethodOrFunction.Add(cppParameter);
                paramCount++;
            }
        }

        /// <summary>
        /// Resolves a type to its fundamental type or a binded type.
        /// This methods is going through the type declaration in order to return the most fundamental type
        /// or to return a bind.
        /// </summary>
        private void ResolveAndFillType(string typeId, CppType type)
        {
            List<string> fullTypeName = new List<string>();
            XElement xType = mapIdToXElement[typeId];
            bool isTypeResolved = false;

            while (!isTypeResolved)
            {
                string name = String.Empty;
                XAttribute nameAttribute = xType.Attribute("name");
                if (nameAttribute != null)
                {
                    name = nameAttribute.Value;
                    fullTypeName.Add(name);
                }

                string nextType = String.Empty;
                XAttribute nextTypeAttribute = xType.Attribute("type");
                if (nextTypeAttribute != null)
                {
                    nextType = nextTypeAttribute.Value;
                }

                string localName = xType.Name.LocalName;
                if (localName == StringEnum.GetStringValue(CastXMLTag.FundamentalType))
                {
                    //type.TypeName = ConvertFundamentalType(name);
                    type.TypeName = name;
                    isTypeResolved = true;
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.Enumeration))
                {
                    type.TypeName = name;
                    isTypeResolved = true;
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.Struct) || name == StringEnum.GetStringValue(CastXMLTag.Union))
                {
                    type.TypeName = name;
                    isTypeResolved = true;
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.Typedef))
                {
                    type.TypeName = name;
                    xType = mapIdToXElement[nextType];
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.PointerType))
                {
                    xType = mapIdToXElement[nextType];
                    type.Pointer = (type.Pointer ?? "") + "*";
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.ArrayType))
                {
                    type.IsArray = true;
                    string maxArrayIndex = xType.Attribute("max").Value;
                    int arrayDim = int.Parse(maxArrayIndex.TrimEnd('u')) + 1;

                    if (type.ArrayDimension == null)
                    {
                        type.ArrayDimension = "" + arrayDim;
                    }
                    else
                    {
                        type.ArrayDimension += "," + arrayDim;
                    }

                    xType = mapIdToXElement[nextType];
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.ReferenceType))
                {
                    xType = mapIdToXElement[nextType];
                    type.Pointer = (type.Pointer ?? "") + "&";
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.CvQualifiedType))
                {
                    xType = mapIdToXElement[nextType];
                    type.Const = true;
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.FunctionType))
                {
                    type.TypeName = "__function__stdcall";
                    isTypeResolved = true;
                }
                else if (localName == StringEnum.GetStringValue(CastXMLTag.Class) || localName == StringEnum.GetStringValue(CastXMLTag.Interface))
                {
                    type.TypeName = name;
                    isTypeResolved = true;
                }
            }
        }

        /// <summary>
        /// Parse a C++ variable from <see cref="XElement"/> 
        /// </summary>
        private CppElement ParseVariable(XElement xElement)
        {
            string name = String.Empty;
            XAttribute nameAttribute = xElement.Attribute("name");
            if (nameAttribute != null)
            {
                name = nameAttribute.Value;
            }

            CppType cppType = new CppType();
            XAttribute typeAttribute = xElement.Attribute("type");
            if (typeAttribute != null)
            {
                ResolveAndFillType(typeAttribute.Value, cppType);
            }

            string value = String.Empty;
            XAttribute valueAttribute = xElement.Attribute("init");
            if (valueAttribute != null)
            {
                value = valueAttribute.Value;

                // CastXML outputs initialization expressions. Cast to proper type.
                Match match = Regex.Match(value, @"\((?:\(.+\))?(.+)\)");
                if (match.Success)
                {
                    value = $"unchecked(({cppType.TypeName}){match.Groups[1].Value})";
                }

                // Handle C++ floating point literals
                value = value.Replace(".F", ".0F");
            }

            return new CppConstant() { Name = name, Value = value };
        }

        /// <summary>
        /// Parses a C++ struct or union declaration.
        /// </summary>
        private CppStruct ParseStructOrUnion(XElement xElement, CppElement cppParent = null, int innerAnonymousIndex = 0)
        {
            CppStruct cppStruct = xElement.Annotation<CppStruct>();
            if (cppStruct != null)
            {
                return cppStruct;
            }

            // Build struct name directly from the struct name or based on the parent
            string structName = String.Empty;
            XAttribute structNameAttribute = xElement.Attribute("name");
            if (structNameAttribute != null)
            {
                structName = structNameAttribute.Value;
            }

            if (cppParent != null)
            {
                if (string.IsNullOrEmpty(structName))
                {
                    structName = cppParent.Name + "_Inner_" + innerAnonymousIndex;
                }
                else
                {
                    structName = cppParent.Name + "_" + structName + "_Inner";
                }
            }

            // Create struct
            cppStruct = new CppStruct { Name = structName };
            xElement.AddAnnotation(cppStruct);
            bool isUnion = (xElement.Name.LocalName == StringEnum.GetStringValue(CastXMLTag.Union));

            // Get align from structure
            cppStruct.Align = xElement.Attribute("align") != null ? int.Parse(xElement.Attribute("align").Value) / 8 : 0;

            // Enter struct/union description
            var basesValueAttribute = xElement.Attribute("bases");
            IEnumerable<string> bases = basesValueAttribute != null ? basesValueAttribute.Value.Split(' ') : Enumerable.Empty<string>();
            foreach (string xElementBaseId in bases)
            {
                if (string.IsNullOrEmpty(xElementBaseId))
                {
                    continue;
                }
                    
                XElement xElementBase = mapIdToXElement[xElementBaseId];

                CppStruct cppStructBase = ParseStructOrUnion(xElementBase);
                if (string.IsNullOrEmpty(cppStructBase.ParentName))
                {
                    cppStruct.ParentName = cppStructBase.Name;
                }
            }

            // Parse all fields
            int fieldOffset = 0;
            int innerStructCount = 0;
            foreach (XElement field in xElement.Elements())
            {
                if (field.Name.LocalName != StringEnum.GetStringValue(CastXMLTag.Field))
                {
                    continue;
                }
                    
                // Parse the field
                var cppField = ParseField(field);
                cppField.Offset = fieldOffset;

                // Test if the field type is declared inside this struct or union
                string fieldName = field.Attribute("name") != null ? field.Attribute("name").Value : String.Empty;
                string fieldTypeId = field.Attribute("type") != null ? field.Attribute("type").Value : String.Empty;

                XElement fieldType = mapIdToXElement[fieldTypeId];
                string fieldTypeContext = fieldType.Attribute("context") != null ? fieldType.Attribute("context").Value : String.Empty;
                string fieldId = field.Attribute("id") != null ? field.Attribute("id").Value : String.Empty;
                if (fieldTypeContext == fieldId)
                {
                    var fieldSubStruct = ParseStructOrUnion(fieldType, cppStruct, innerStructCount++);

                    // If fieldName is empty, then we need to inline fields from the struct/union.
                    if (string.IsNullOrEmpty(fieldName))
                    {
                        // Make a copy in order to remove fields
                        List<CppField> listOfSubFields = new List<CppField>(fieldSubStruct.Fields);

                        // Copy the current field offset
                        int lastFieldOffset = fieldOffset;
                        foreach (CppField subField in listOfSubFields)
                        {
                            subField.Offset = subField.Offset + fieldOffset;
                            cppStruct.Add(subField);
                            lastFieldOffset = subField.Offset;
                        }

                        // Set the current field offset according to the inlined fields
                        if (!isUnion)
                        {
                            fieldOffset = lastFieldOffset;
                        }
                            
                        // Don't add the current field, as it is actually an inline struct/union
                        cppField = null;
                    }
                    else
                    {
                        // Get the type name from the inner-struct and set it to the field
                        cppField.TypeName = fieldSubStruct.Name;
                        currentCppInclude.Add(fieldSubStruct);
                    }
                }

                // Go to next field offset if not in union
                bool goToNextFieldOffset = !isUnion;

                // Add the field if any
                if (cppField != null)
                {
                    cppStruct.Add(cppField);
                    // TODO managed multiple bitfield group
                    // Current implem is only working with a single set of consecutive bitfield in the same struct
                    goToNextFieldOffset = goToNextFieldOffset && !cppField.IsBitField;
                }

                if (goToNextFieldOffset)
                {
                    fieldOffset++;
                }
            }

            return cppStruct;
        }

        /// <summary>
        /// Parses a C++ field declaration.
        /// </summary>
        private CppField ParseField(XElement xElement)
        {
            CppField cppField = new CppField();
            cppField.Name = xElement.Attribute("name") != null ? xElement.Attribute("name").Value : String.Empty;

            // Handle bitfield info
            string bitField = xElement.Attribute("bits") != null ? xElement.Attribute("bits").Value : String.Empty;
            if (!string.IsNullOrEmpty(bitField))
            {
                cppField.IsBitField = true;
                cppField.BitOffset = int.Parse(bitField);
            }

            string fieldType = xElement.Attribute("type") != null ? xElement.Attribute("type").Value : String.Empty;
            ResolveAndFillType(fieldType, cppField);

            return cppField;
        }

        /// <summary>
        /// Parses a C++ function.
        /// </summary>
        private CppElement ParseFunction(XElement xElement)
        {
            return ParseMethodOrFunction<CppFunction>(xElement);
        }

        /// <summary>
        /// Parses a C++ enum declaration.
        /// </summary>
        private CppEnum ParseEnum(XElement xElement)
        {
            CppEnum cppEnum = new CppEnum();
            cppEnum.Name = xElement.Attribute("name") != null ? xElement.Attribute("name").Value : String.Empty;

            // Anonymous enum
            if (cppEnum.Name.StartsWith("$") || string.IsNullOrEmpty(cppEnum.Name))
            {
                int enumOffset;
                string includeFrom = GetIncludeIdFromFileId(xElement.Attribute("file").Value);
                
                if (!mapIncludeToAnonymousEnumCount.TryGetValue(includeFrom, out enumOffset))
                {
                    mapIncludeToAnonymousEnumCount.Add(includeFrom, enumOffset);
                }
                    
                cppEnum.Name = includeFrom.ToUpper() + "_ENUM_" + enumOffset;
                mapIncludeToAnonymousEnumCount[includeFrom]++;
            }

            foreach (XElement xEnumItems in xElement.Elements())
            {
                string enumItemName = xEnumItems.Attribute("name") != null ? xEnumItems.Attribute("name").Value : String.Empty;
                string enumItemInit = xEnumItems.Attribute("init") != null ? xEnumItems.Attribute("init").Value : String.Empty;

                cppEnum.Add(new CppEnumItem(enumItemName, enumItemInit));
            }

            return cppEnum;
        }
    }
}