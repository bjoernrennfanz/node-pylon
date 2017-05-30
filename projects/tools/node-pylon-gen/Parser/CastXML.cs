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

using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace NodePylonGen.Parser
{
    public class CastXML
    {
        // Regex for error detection
        private static Regex MatchForError = new Regex("error:");

        /// <summary>
        ///     CastXml tag for FundamentalType
        /// </summary>
        public const string TagFundamentalType = "FundamentalType";

        /// <summary>
        ///     CastXml tag for Enumeration
        /// </summary>
        public const string TagEnumeration = "Enumeration";

        /// <summary>
        ///     CastXml tag for Struct
        /// </summary>
        public const string TagStruct = "Struct";

        /// <summary>
        ///     CastXml tag for Field
        /// </summary>
        public const string TagField = "Field";

        /// <summary>
        ///     CastXml tag for Union
        /// </summary>
        public const string TagUnion = "Union";

        /// <summary>
        ///     CastXml tag for Typedef
        /// </summary>
        public const string TagTypedef = "Typedef";

        /// <summary>
        ///     CastXml tag for Function
        /// </summary>
        public const string TagFunction = "Function";

        /// <summary>
        ///     CastXml tag for PointerType
        /// </summary>
        public const string TagPointerType = "PointerType";

        /// <summary>
        ///     CastXml tag for ArrayType
        /// </summary>
        public const string TagArrayType = "ArrayType";

        /// <summary>
        ///     CastXml tag for ReferenceType
        /// </summary>
        public const string TagReferenceType = "ReferenceType";

        /// <summary>
        ///     CastXml tag for CvQualifiedType
        /// </summary>
        public const string TagCvQualifiedType = "CvQualifiedType";

        /// <summary>
        ///     CastXml tag for Namespace
        /// </summary>
        public const string TagNamespace = "Namespace";

        /// <summary>
        ///     CastXml tag for Variable
        /// </summary>
        public const string TagVariable = "Variable";

        /// <summary>
        ///     CastXml tag for FunctionType
        /// </summary>
        public const string TagFunctionType = "FunctionType";

        /// <summary>
        ///     Gets or sets the executable path of castxml.exe.
        /// </summary>
        /// <value>The executable path.</value>
        public string ExecutablePath { get; set; }

        /// <summary>
        ///     Gets or sets additional include directory
        /// </summary>
        public List<string> AdditionalIncludeDirs { get; private set; }

        /// <summary>
        /// Initializes a new instance of the <see cref="CastXml"/> class.
        /// </summary>
        public CastXML()
        {
            AdditionalIncludeDirs = new List<string>();
        }
    }
}
