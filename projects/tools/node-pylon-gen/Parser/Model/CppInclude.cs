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

using System.Collections.Generic;
using System.Xml.Serialization;

namespace NodePylonGen.Parser.Model
{
    /// <summary>
    /// A C++ include declarations for macros, enums, 
    /// structs, interfaces and functions.
    /// </summary>
    [XmlType("include")]
    public class CppInclude : CppElement
    {
        /// <summary>
        /// Gets the full name.
        /// </summary>
        [XmlIgnore]
        public override string FullName
        {
            get { return ""; }
        }

        /// <summary>
        /// Gets the classes.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppClass> Classes
        {
            get { return Iterate<CppClass>(); }
        }

        /// <summary>
        /// Gets the macros.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppDefine> Macros
        {
            get { return Iterate<CppDefine>(); }
        }

        /// <summary>
        /// Gets the interfaces.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppInterface> Interfaces
        {
            get { return Iterate<CppInterface>(); }
        }

        /// <summary>
        /// Gets the functions.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppFunction> Functions
        {
            get { return Iterate<CppFunction>(); }
        }

        /// <summary>
        /// Gets the structs.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppStruct> Structs
        {
            get { return Iterate<CppStruct>(); }
        }

        /// <summary>
        /// Gets the enums.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppEnum> Enums
        {
            get { return Iterate<CppEnum>(); }
        }

        /// <summary>
        /// Gets the constants.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppConstant> Constants
        {
            get { return Iterate<CppConstant>(); }
        }

        /// <summary>
        /// Overriden version of Visit<T>
        /// </summary>
        public override T Visit<T>(ICppElementVisitor<T> visitor)
        {
            return visitor.VisitCppInclude(this);
        }
    }
}
