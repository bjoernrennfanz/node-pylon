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
using NodePylonGen.Config;
using NodePylonGen.Utils;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Text.RegularExpressions;

namespace NodePylonGen.Parser
{
    public class CastXML
    {
        private static readonly ILog log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        // Regex for error detection
        private static Regex MatchForError = new Regex("error:");

        /// <summary>
        /// Enum for CastXML Tag definitions
        /// </summary>
        public enum CastXMLTag
        {
            [StringEnumValue("FundamentalType")]
            FundamentalType,
            [StringEnumValue("Enumeration")]
            Enumeration,
            [StringEnumValue("Struct")]
            Struct,
            [StringEnumValue("Field")]
            Field,
            [StringEnumValue("Union")]
            Union,
            [StringEnumValue("Typedef")]
            Typedef,
            [StringEnumValue("Function")]
            Function,
            [StringEnumValue("PointerType")]
            PointerType,
            [StringEnumValue("ArrayType")]
            ArrayType,
            [StringEnumValue("ReferenceType")]
            ReferenceType,
            [StringEnumValue("CvQualifiedType")]
            CvQualifiedType,
            [StringEnumValue("Namespace")]
            Namespace,
            [StringEnumValue("Variable")]
            Variable,
            [StringEnumValue("FunctionType")]
            FunctionType,
            [StringEnumValue("Class")]
            Class
        }

        /// <summary>
        /// Gets or sets the executable path of castxml.exe.
        /// </summary>
        /// <value>The executable path.</value>
        public string ExecutablePath { get; set; }

        /// <summary>
        /// Gets or sets the path to the Visual C++ toolset
        /// </summary>
        public string VcToolsPath { get; set; }

        /// <summary>
        /// Gets or sets include directorys
        /// </summary>
        public List<IncludeDirMapping> IncludeDirs { get; set; }

        /// <summary>
        /// Initializes a new instance of the <see cref="CastXml"/> class.
        /// </summary>
        public CastXML()
        {
            IncludeDirs = new List<IncludeDirMapping>();
        }

        /// <summary>
        /// Get command-line arguments of castxml.exe
        /// </summary>
        private static string GetCastXmlArgs()
        {
            string arguments = String.Empty;
            arguments += "--castxml-cc-msvc cl --castxml-gccxml -fexceptions";

            return arguments;
        }

        /// <summary>
        /// Processes the error from header file.
        /// </summary>
        private void ProcessErrorFromHeaderFile(object sender, DataReceivedEventArgs e)
        {
            if (e.Data != null)
            {
                string errorText = e.Data;
                if (MatchForError.Match(errorText).Success)
                {
                    log.Error(errorText);
                }
                else
                {
                    log.Warn(errorText);
                }
            }
        }

        /// <summary>
        /// Processes the output from header file.
        /// </summary>
        static void ProcessOutputFromHeaderFile(object sender, DataReceivedEventArgs e)
        {
            if (e.Data != null)
            {
                log.Info(e.Data);
            }
        }

        /// <summary>
        ///     Pre process header file an check for parser errors
        /// </summary>
        /// <param name="headerFile"></param>
        /// <param name="consoleHandler"></param>
        public void PreProcessHeader(string headerFile, DataReceivedEventHandler consoleHandler)
        {
            if (!File.Exists(ExecutablePath))
            {
                log.Fatal("castxml.exe not found in path: " + ExecutablePath);
                return;
            }

            if (!File.Exists(headerFile))
            {
                log.Fatal("C++ header file " + headerFile + " was not found.");
                return;
            }

            string castXmlPreProcessBatch = Path.Combine(Environment.CurrentDirectory, "castxml_preprocess.bat");

            Process castXMLProcess = new Process();
            ProcessStartInfo castXMLInfo = new ProcessStartInfo(castXmlPreProcessBatch)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                WorkingDirectory = Environment.CurrentDirectory
            };

            string arguments = GetCastXmlArgs();
            arguments += " -E -dD";
            foreach (IncludeDirMapping directory in IncludeDirs)
            {
                arguments += " -I\"" + directory.Path + "\"";
            }

            log.Info("Generating batch: " + castXmlPreProcessBatch);
            StreamWriter streamWriter = new StreamWriter(castXmlPreProcessBatch);
            streamWriter.WriteLine("@echo off");
            streamWriter.WriteLine("call \"" + VcToolsPath + "\\vcvarsall.bat\" x86");
            streamWriter.WriteLine(ExecutablePath + " " + arguments + " " + headerFile);
            streamWriter.Close();

            log.Info("Running batch: " + castXmlPreProcessBatch);

            castXMLProcess.StartInfo = castXMLInfo;
            castXMLProcess.ErrorDataReceived += ProcessErrorFromHeaderFile;
            castXMLProcess.OutputDataReceived += consoleHandler;
            castXMLProcess.Start();
            castXMLProcess.BeginOutputReadLine();
            castXMLProcess.BeginErrorReadLine();

            castXMLProcess.WaitForExit();
            castXMLProcess.Close();
        }

        /// <summary>
        /// Processes the specified header headerFile.
        /// </summary>
        /// <param name="headerFile">The header headerFile.</param>
        /// <returns></returns>
        public StreamReader Process(string headerFile)
        {
            StreamReader result = null;
            ExecutablePath = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, ExecutablePath));

            if (!File.Exists(ExecutablePath))
            {
                log.Fatal("castxml.exe not found from path: " + ExecutablePath);
                return result;
            }

            if (!File.Exists(headerFile))
            {
                log.Fatal("C++ Header file " + headerFile + " not found");
                return result;
            }

            string castXmlProcessBatch = Path.Combine(Environment.CurrentDirectory, "castxml_process.bat");

            Process currentProcess = new Process();
            ProcessStartInfo startInfo = new ProcessStartInfo(castXmlProcessBatch)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                WorkingDirectory = Environment.CurrentDirectory
            };
            var xmlFile = Path.ChangeExtension(headerFile, "xml");

            // Delete any previously generated xml file
            File.Delete(xmlFile);

            string arguments = GetCastXmlArgs();
            arguments += " -o " + xmlFile;

            foreach (IncludeDirMapping directory in IncludeDirs)
            {
                arguments += " -I\"" + directory.Path + "\"";
            }

            log.Info("Generating batch: " + castXmlProcessBatch);
            StreamWriter streamWriter = new StreamWriter(castXmlProcessBatch);
            streamWriter.WriteLine("@echo off");
            streamWriter.WriteLine("call \"" + VcToolsPath + "\\vcvarsall.bat\" x86");
            streamWriter.WriteLine(ExecutablePath + " " + arguments + " " + headerFile);
            streamWriter.Close();

            log.Info("Running batch: " + castXmlProcessBatch);
            currentProcess.StartInfo = startInfo;
            currentProcess.ErrorDataReceived += ProcessErrorFromHeaderFile;
            currentProcess.OutputDataReceived += ProcessOutputFromHeaderFile;
            currentProcess.Start();
            currentProcess.BeginOutputReadLine();
            currentProcess.BeginErrorReadLine();

            currentProcess.WaitForExit();

            currentProcess.Close();

            if (!File.Exists(xmlFile))
            {
                log.Error("Unable to generate XML file with castxml " + xmlFile + ". Check previous errors.");
                return result;
            }
            else
            {
                result = new StreamReader(xmlFile);
            }

            return result;
        }

    }
}
