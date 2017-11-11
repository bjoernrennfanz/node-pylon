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

using System.Linq;
using System.Text.RegularExpressions;

namespace NodePylonGen.Utils
{
    public static class NodeJSClassHelper
    {
        /// <summary>
        /// Convert given class name to name used for parameters
        /// </summary>
        public static string ConvertToParameterName(string name)
        {
            // Generate name for wrapped parameters
            string convertedName = GenerateTrimmedClassName(name);
            convertedName = char.ToLowerInvariant(convertedName[0]) + convertedName.Substring(1);

            return convertedName;
        }

        /// <summary>
        /// Remove leading I, C and template parameters from class name
        /// </summary>
        public static string GenerateTrimmedClassName(string className)
        {
            // Remove C or I prefix from class name
            string trimmedClassName = className;
            if (className.Substring(0, 2).Contains("CC") || className.Substring(0, 2).Contains("II") || className.Substring(0, 2).Contains("IC"))
            {
                trimmedClassName = className.Substring(1);
            }
            else
            {
                // Trim only when 2 characters at the begin are uppercase
                if (trimmedClassName.Substring(0, 2).All(c => char.IsUpper(c)))
                {
                    trimmedClassName = className.TrimStart('I').TrimStart('C');
                }
            }

            // Convert GeniCam names
            if (trimmedClassName.StartsWith("gc"))
            {
                trimmedClassName = trimmedClassName.Substring(0, 3).ToUpper() + trimmedClassName.Substring(3);
            }

            // Check if trimmed name contains template parameter
            if (trimmedClassName.Contains("<"))
            {
                // Remove template parameter
                trimmedClassName = trimmedClassName.Substring(0, trimmedClassName.IndexOf("<"));
            }

            // Convert underscores and uppercase followed character
            if (trimmedClassName.Contains("_"))
            {
                string searchPattern = @"_";
                foreach (Match match in Regex.Matches(trimmedClassName, searchPattern))
                {
                    // Check if found index is not at the end
                    if ((match.Index + 2) < trimmedClassName.Count())
                    {
                        // Split string
                        string front = trimmedClassName.Substring(0, match.Index);
                        string upper = trimmedClassName.Substring(match.Index + 1, 1).ToUpper();
                        string back = trimmedClassName.Substring(match.Index + 2);

                        // Assemble string again
                        trimmedClassName = front + upper + back;
                    }
                }
            }

            // Trim only when 1 characters at the begin are lowercase
            if (trimmedClassName.Substring(0, 1).All(c => char.IsLower(c)))
            {
                trimmedClassName = trimmedClassName.Substring(0, 1).ToUpper() + trimmedClassName.Substring(1);
            }

            return trimmedClassName;
        }

        public static string GenerateClassWrapName(string className)
        {
            return GenerateTrimmedClassName(className) + "Wrap";
        }

        public static string GenerateWrapperClassName(string className)
        {
            return "wrapped" + GenerateTrimmedClassName(className);
        }

        public static string GenerateClassWrapperMember(string className)
        {
            return "m_" + GenerateTrimmedClassName(className);
        }
    }
}
