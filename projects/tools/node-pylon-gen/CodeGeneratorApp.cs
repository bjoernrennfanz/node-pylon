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
using Mono.Options;
using NodePylonGen.Config;
using NodePylonGen.Parser;
using NodePylonGen.Parser.Model;
using System;
using System.IO;
using System.Linq;
using System.Reflection;
using NodePylonGen.Generator.Generators.NodeJS;
using NodePylonGen.Generator;
using System.Collections.Generic;

namespace NodePylonGen
{
    /// <summary>
    /// pylon-node Code Generator Application
    /// </summary>
    public class CodeGeneratorApp
    {
        private static readonly ILog log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        /// <summary>
        /// Gets or sets the CastXML executable path.
        /// </summary>
        public string CastXmlExecutablePath { get; set; }

        /// <summary>
        /// Gets or sets the path to the Visual C++ toolset
        /// </summary>
        public string VcToolsPath { get; set; }

        private bool forceParsing;
        private bool configHasChanged;

        private ConfigMapping config;
        private string configRootPath;
        private string configCheckFilePath;

        private string generatorRootPath;

        /// <summary>
        /// Print usages with error message.
        /// </summary>
        private void UsageError(string error, params object[] parameters)
        {
            var exeName = Path.GetFileName(Assembly.GetEntryAssembly().Location);

            Console.Write(exeName + ": ");
            Console.WriteLine(error, parameters);
            Console.WriteLine("Use " + exeName + " --help' for more information.");

            Environment.Exit(1);
        }

        /// <summary>
        /// Parses the command line arguments.
        /// </summary>
        /// <param name="args">The args.</param>
        public void ParseArguments(string[] args)
        {
            var showHelp = false;

            var options = new OptionSet()
            {
                "Copyright (c) 2017 pylon-node - Björn Rennfanz",
                "Usage: pylon-node-gen [options] Mapping.xml",
                "Code generator from C++ to Node.JS language",
                "",
                {"c|castxml=", "Specify the path to castxml.exe", opt => CastXmlExecutablePath = opt},
                {"v|vctools=", "Specify the path to the Visual C++ Toolset", opt => VcToolsPath = opt },
                {"f|force", "Force parsing of pylon header files", opt => forceParsing = opt != null },
                "",
                {"h|help", "Show this message and exit", opt => showHelp = opt != null},
                // default
                {"<>", opt => configRootPath = opt },
            };

            try
            {
                options.Parse(args);
            }
            catch (OptionException e)
            {
                UsageError(e.Message);
            }

            if (showHelp)
            {
                options.WriteOptionDescriptions(Console.Out);
                Environment.Exit(0);
            }

            if (configRootPath == null)
            {
                UsageError("Missing Mapping.xml. A Mapping.xml must be specified");
            }
        }

        /// <summary>
        /// Initializes the specified instance with a config root file.
        /// </summary>
        public bool Initialize()
        {
            generatorRootPath = Path.GetDirectoryName(configRootPath);

            log.Info("Loading config files...");
            config = ConfigMapping.Load(configRootPath, new VariableMapping("VC_TOOLS_PATH", VcToolsPath));

            DateTime configTimestamp = ConfigMapping.GetLatestTimestamp(config.ConfigFilesLoaded);
            configCheckFilePath = config.Id + "-" + "codegen.check";

            configHasChanged = !File.Exists(configCheckFilePath) || configTimestamp > File.GetLastWriteTime(configCheckFilePath);
            if (configHasChanged)
            {
                log.Info("Config files " + string.Join(", ", config.ConfigFilesLoaded.Select(file => Path.GetFileName(file.AbsoluteFilePath))) + " changed.");
            }

            return configHasChanged;
        }

        /// <summary>
        /// Run CodeGenerator
        /// </summary>
        public void Run()
        {
            log.Info("Starting code generation...");

            // Initialize the parser
            CppParser parser = new CppParser(CastXmlExecutablePath, VcToolsPath);
            parser.ForceParsing = configHasChanged || forceParsing;
            parser.Initialize(config);

            // Run the parser
            CppModule mainModule = parser.Run();

            // Run the code generation process
            NodeJSGenerator generator = new NodeJSGenerator(config, mainModule);

            // Setup rules and process code 
            generator.SetupCodeRules();
            generator.ProcessCode();

            // Generate wrapper code
            List<GeneratorOutput> outputs = generator.GenerateCode();

            SaveCode(outputs);
        }

        private void SaveCode(IEnumerable<GeneratorOutput> outputs)
        {
            foreach (var output in outputs)
            {
                foreach (var template in output.Outputs)
                {
                    var filePath = template.FilePath;

                    File.WriteAllText(filePath, template.Generate());
                    log.Info("Generated '" + filePath + "'");
                }
            }
        }
    }
}
