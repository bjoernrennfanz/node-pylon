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
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using NodePylonGen.Config;
using NodePylonGen.Parser.Cpp;
using System.IO;

namespace NodePylonGen.Parser
{
    public class CppParser
    {
        private static readonly ILog log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private const string Version = "0.0.1";

        private ConfigMapping masterConfig;
        private string masterHeaderFile;
        
        private CppModule cppModule;
        private CastXML castXML;

        private Dictionary<string, bool> includeToProcess = new Dictionary<string, bool>();
        private Dictionary<string, bool> includeIsAttached = new Dictionary<string, bool>();

        public CppParser(string castXmlExecutablePath)
        {
            this.CastXmlExecutablePath = castXmlExecutablePath;
        }

        /// <summary>
        /// Gets the CastXML executable path.
        /// </summary>
        public string CastXmlExecutablePath { get; private set; }

        public void Initialize(ConfigMapping config)
        {
            masterConfig = config;
            masterHeaderFile = masterConfig.Id + ".hpp";

            // Create CastXml instance
            castXML = new CastXML();
            castXML.ExecutablePath = CastXmlExecutablePath;

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
                includeIsAttached.Add(configFile.Id, true);

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

             
            }
        }
    }
}
