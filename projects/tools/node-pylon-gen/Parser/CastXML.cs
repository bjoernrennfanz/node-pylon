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
        ///     Enum for CastXML Tag definitions
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
            FunctionType
        }

        /// <summary>
        ///     Gets or sets the executable path of castxml.exe.
        /// </summary>
        /// <value>The executable path.</value>
        public string ExecutablePath { get; set; }

        /// <summary>
        ///     Gets or sets additional include directory
        /// </summary>
        public List<string> AdditionalIncludeDirs { get; set; }

        /// <summary>
        /// Initializes a new instance of the <see cref="CastXml"/> class.
        /// </summary>
        public CastXML()
        {
            AdditionalIncludeDirs = new List<string>();
        }

        /// <summary>
        ///     Get command-line arguments of castxml.exe
        /// </summary>
        /// <returns></returns>
        private static string GetCastXmlArgs()
        {
            var arguments = String.Empty;

            arguments += " --castxml-gccxml";
            arguments += " -x c++ -std=c++11 -fmsc-version=1900 -fms-extensions -fms-compatibility";

            return arguments;
        }

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

            Process castXMLProcess = new Process();
            ProcessStartInfo castXMLInfo = new ProcessStartInfo(ExecutablePath)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                WorkingDirectory = Environment.CurrentDirectory
            };

            string castXMLArgs = GetCastXmlArgs();
            castXMLArgs += " -E -dD";
            foreach (string directory in AdditionalIncludeDirs)
            {
                castXMLArgs += " " + directory;
            }

            castXMLInfo.Arguments = castXMLArgs + " " + headerFile;
            castXMLProcess.StartInfo = castXMLInfo;
            castXMLProcess.ErrorDataReceived += ProcessErrorFromHeaderFile;
            castXMLProcess.OutputDataReceived += consoleHandler;

            castXMLProcess.Start();
            castXMLProcess.BeginOutputReadLine();
            castXMLProcess.BeginErrorReadLine();

            castXMLProcess.WaitForExit();
            castXMLProcess.Close();
        }
    

    }
}
