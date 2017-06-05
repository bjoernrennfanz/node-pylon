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
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Xml.Serialization;

namespace NodePylonGen.Config
{
    /// <summary>
    ///     Config file for mappings
    /// </summary>
    [XmlRoot("config", Namespace = NS)]
    public class ConfigMapping
    {
        internal const string NS = "urn:NodePylonGen.Config";

        private static readonly ILog log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private static readonly Regex SearchVariableRegex = new Regex(@"\$\(([a-zA-Z_][\w_]*)\)", RegexOptions.Compiled);

        public ConfigMapping()
        {
            Files = new List<string>();
            Includes = new List<IncludeMapping>();
            IncludeDirs = new List<IncludeDirMapping>();
            References = new List<ConfigMapping>();
            Variables = new List<VariableMapping>();
        }

        /// <summary>
        /// Gets the parent of this mapping file.
        /// </summary>
        /// <value>The parent mapping config.</value>
        [XmlIgnore]
        public ConfigMapping Parent { get; private set; }

        /// <summary>
        /// Gets the path of this MappingFile.
        /// </summary>
        /// <value>The path to mapping config.</value>
        [XmlIgnore]
        public string FilePath { get; private set; }

        /// <summary>
        /// Gets the absolute path of this MappingFile.
        /// </summary>
        [XmlIgnore]
        public string AbsoluteFilePath
        {
            get
            {
                // Check if we are the root file
                if (Path.IsPathRooted(FilePath))
                {
                    return FilePath;
                }

                // Check if we are a child file    
                if (Parent != null)
                {
                    return Path.Combine(Path.GetDirectoryName(Parent.AbsoluteFilePath), FilePath);
                }
                    
                // Fallback
                return Path.GetFullPath(Path.Combine(".", FilePath));
            }
        }

        /// <summary>
        /// Gets a list with references to this MappingFile.
        /// </summary>
        [XmlIgnore]
        public List<ConfigMapping> References { get; private set; }

        [XmlAttribute("id")]
        public string Id { get; set; }

        [XmlElement("binding")]
        public string Binding { get; set; }

        [XmlElement("module")]
        public string Module { get; set; }

        [XmlElement("include-dir")]
        public List<IncludeDirMapping> IncludeDirs { get; set; }

        [XmlElement("include")]
        public List<IncludeMapping> Includes { get; set; }

        [XmlElement("file")]
        public List<string> Files { get; set; }

        [XmlElement("var")]
        public List<VariableMapping> Variables { get; set; }

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        public bool Equals(ConfigMapping other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;

            return Equals(other.Id, Id);
        }

        /// <summary>
        /// Determines whether the specified <see cref="System.Object"/> is equal to this instance.
        /// </summary>
        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof(ConfigMapping)) return false;

            return Equals((ConfigMapping)obj);
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        public override int GetHashCode()
        {
            return (Id != null ? Id.GetHashCode() : 0);
        }

        /// <summary>
        /// Unfold all variables used inside tags.
        /// </summary>
        public void UnfoldVariables()
        {
            UnfoldVariables(Variables);
            UnfoldVariables(Includes);
            UnfoldVariables(IncludeDirs);

            // Expand also all references
            foreach (ConfigMapping config in References)
            {
                config.UnfoldVariables();
            }       
        }

        /// <summary>
        /// Iterate over all objects and sub-objects to unfold variables
        /// </summary>
        private object UnfoldVariables(object objectToExpand)
        {
            if (objectToExpand == null)
            {
                return null;
            }
                
            if (objectToExpand is string)
            {
                return UnfoldString((string)objectToExpand);
            }
                
            if (objectToExpand.GetType().IsPrimitive)
            {
                return objectToExpand;
            }
                
            if (objectToExpand is IList)
            {
                IList list = ((IList)objectToExpand);
                for (int i = 0; i < list.Count; i++)
                {
                    list[i] = UnfoldVariables(list[i]);
                }
                    
                return list;
            }

            foreach (var propertyInfo in objectToExpand.GetType().GetProperties())
            {
                if (propertyInfo.GetCustomAttributes(typeof(XmlIgnoreAttribute), false).Length == 0)
                {
                    // Check that this field is "ShouldSerializable"
                    var method = objectToExpand.GetType().GetMethod("ShouldSerialize" + propertyInfo.Name);
                    if (method != null && !((bool)method.Invoke(objectToExpand, null)))
                    {
                        continue;
                    }
                        
                    propertyInfo.SetValue(objectToExpand, UnfoldVariables(propertyInfo.GetValue(objectToExpand, null)), null);
                }
            }

            return objectToExpand;
        }

        /// <summary>
        /// Unfold a string using environment variable and variables defined in mapping configuration.
        /// </summary>
        public string UnfoldString(string variableValue)
        {
            var result = variableValue;

            // Perform config variable substitution
            if (SearchVariableRegex.Match(result).Success)
            {
                result = SearchVariableRegex.Replace(result, delegate (Match match)
                {
                    string name = match.Groups[1].Value;
                    string localResult = GetVariable(name);

                    if (localResult == null)
                    {
                        localResult = Environment.GetEnvironmentVariable(name);
                    }

                    if (localResult == null)
                    {
                        log.Error("Unable to substitute config or environment variable " + name + ". Variable is not defined");
                        return "";
                    }

                    return localResult;
                });
            }

            return result;
        }

        /// <summary>
        /// Gets a variable value. Value is unfolded if it contains any reference to other variables.
        /// </summary>
        private string GetVariable(string variableName)
        {
            foreach (VariableMapping keyValue in Variables)
            {
                if (keyValue.Name == variableName)
                {
                    return UnfoldString(keyValue.Value);
                }
                    
            }

            if (Parent != null)
            {
                return Parent.GetVariable(variableName);
            }
                
            return null;
        }

        private void PostLoad(ConfigMapping parent, string file, IEnumerable<VariableMapping> variables)
        {
            FilePath = file;
            Parent = parent;

            Variables.Add(new VariableMapping("CONFIG_PATH", Path.GetDirectoryName(AbsoluteFilePath)));
            Variables.AddRange(variables);

            // Load all dependencies
            foreach (string dependFile in Files)
            {
                string dependFilePath = dependFile;
                if (!Path.IsPathRooted(dependFilePath))
                {
                    dependFilePath = Path.Combine(Path.GetDirectoryName(AbsoluteFilePath), dependFilePath);
                }

                ConfigMapping subMapping = Load(this, dependFilePath, variables);
                if (subMapping != null)
                {
                    subMapping.FilePath = dependFile;
                    References.Add(subMapping);
                }
            }

            // Clear all depends file
            Files.Clear();

            // Add this mapping file
            GetRoot().IdToConfigMapping.Add(Id, this);
        }

        private void Verify()
        {
            // Check all given include dirs
            foreach (var includeDir in IncludeDirs)
            {
                includeDir.Path = UnfoldString(includeDir.Path);
                if (!Directory.Exists(includeDir.Path))
                {
                    throw new DirectoryNotFoundException("Directory " + includeDir.Path + " not found in config file " + FilePath);
                }
            }

            // Verify all dependencies
            foreach (ConfigMapping mappingFile in References)
            {
                mappingFile.Verify();
            }
        }

        /// <summary>
        /// Loads the specified config file attached to a parent config file.
        /// </summary>
        private static ConfigMapping Load(ConfigMapping parent, string file, IEnumerable<VariableMapping> variables)
        {
            var deserializer = new XmlSerializer(typeof(ConfigMapping));
            ConfigMapping config = null;
            try
            {
                config = (ConfigMapping)deserializer.Deserialize(new StringReader(File.ReadAllText(file)));
                if (config != null)
                {
                    config.PostLoad(parent, file, variables);
                } 
            }
            catch (Exception ex)
            {
                log.Error("Unable to parse " + file, ex);
            }

            return config;
        }

        /// <summary>
        /// Loads a specified config file.
        /// </summary>
        public static ConfigMapping Load(string file, params VariableMapping[] variables)
        {
            ConfigMapping root = Load(null, file, variables);
            root.Verify();

            return root;
        }

        /// <summary>
        /// Gets the latest timestamp from a set of config files.
        /// </summary>
        /// <param name="files">The files to check.</param>
        /// <returns>The latest timestamp from a set of config files</returns>
        public static DateTime GetLatestTimestamp(IEnumerable<ConfigMapping> files)
        {
            DateTime latestTimestamp = new DateTime(0);
            foreach (ConfigMapping configFile in files)
            {
                DateTime fileTimestamp = File.GetLastWriteTime(configFile.AbsoluteFilePath);
                if (fileTimestamp > latestTimestamp)
                {
                    latestTimestamp = fileTimestamp;
                }
            }

            return latestTimestamp;
        }

        /// <summary>
        ///     Get a list of loaded mapping configs.
        /// </summary>
        public IEnumerable<ConfigMapping> ConfigFilesLoaded
        {
            get { return GetRoot().IdToConfigMapping.Values; }
        }

        /// <summary>
        ///     Get the root mapping config.
        /// </summary>
        /// <returns></returns>
        public ConfigMapping GetRoot()
        {
            ConfigMapping root = this;
            while (root.Parent != null)
            {
                root = root.Parent;
            }
                
            return root;
        }

        private Dictionary<string, ConfigMapping> IdToConfigMapping = new Dictionary<string, ConfigMapping>();
    }
}
