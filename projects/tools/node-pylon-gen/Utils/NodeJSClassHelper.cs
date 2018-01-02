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
        /// Class Names from GeniCam that needs special handling
        /// </summary>
        private static string[] genicamSpecialClassFilter = { "ibase", "iboolean", "icategory", "ichunkport", "ivalue", "icommand",
            "ideviceinfo", "ienumentry", "ienumeration", "ifloat", "iinteger", "inode", "inodemap", "iport", "iportconstruct",
            "iportrecorder", "iregister", "iselector", "iselectordigit", "istring", "ivalue" };

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
                trimmedClassName = trimmedClassName.Substring(1);
            }
            else
            {
                // Trim only when 2 characters at the begin are uppercase
                if (trimmedClassName.Substring(0, 2).All(c => char.IsUpper(c)))
                {
                    trimmedClassName = trimmedClassName.TrimStart('I').TrimStart('C');
                }
                else
                {
                    // Filter special GeniCam class namings
                    if (genicamSpecialClassFilter.Any(filter => filter.Equals(className)))
                    {
                        trimmedClassName = trimmedClassName.Substring(1, 1).ToUpper() + className.Substring(2);
                    }

                    // Convert GeniCam names
                    if (trimmedClassName.StartsWith("gc"))
                    {
                        trimmedClassName = trimmedClassName.Substring(0, 3).ToUpper() + trimmedClassName.Substring(3);
                    }
                }
            }

            // Check if trimmed name contains template parameter
            if (trimmedClassName.Contains("<"))
            {
                // Remove template parameter
                trimmedClassName = trimmedClassName.Substring(0, trimmedClassName.IndexOf("<"));
            }

            // Remove type indicators
            if (trimmedClassName.EndsWith("_t"))
            {
                trimmedClassName = trimmedClassName.Replace("_t", "");
            }

            // Convert underscores and uppercase followed character
            if (trimmedClassName.Contains("_"))
            {
                string searchPattern = @"_";
                MatchCollection matches = Regex.Matches(trimmedClassName, searchPattern);

                // Check if we have matches
                if (matches.Count > 0)
                {
                    do
                    {
                        // Get first entry
                        Match match = matches[0];

                        // Check if found index is not at the end
                        if ((match.Index + 2) < trimmedClassName.Count())
                        {
                            // Split string
                            string beforeMatch = trimmedClassName.Substring(0, match.Index);
                            string upperPart = trimmedClassName.Substring(match.Index + 1, 1).ToUpper();
                            string lowerPart = trimmedClassName.Substring(match.Index + 2).ToLower();

                            // Assemble string again
                            trimmedClassName = beforeMatch + upperPart + lowerPart;

                            // Refresh matches
                            matches = Regex.Matches(trimmedClassName, searchPattern);
                        }
                        else
                        {
                            // Prevent endless loop
                            break;
                        }
                    }
                    while (matches.Count > 0);
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
