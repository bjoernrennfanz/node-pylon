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
using System.Text;
using System.Xml.Serialization;
using System.Collections.Generic;

namespace NodePylonGen.Parser.Model
{
    /// <summary>
    /// A C++ method.
    /// </summary>
    [XmlType("method")]
    public class CppMethod : CppElement
    {
        /// <summary>
        /// Gets or sets the type of the return.
        /// </summary>
        [XmlElement("return")]
        public CppType ReturnType { get; set; }

        /// <summary>
        /// Gets or sets the offset.
        /// </summary>
        [XmlAttribute("offset")]
        public int Offset { get; set; }

        /// <summary>
        /// Gets the parameters.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppParameter> Parameters
        {
            get { return Iterate<CppParameter>(); }
        }

        /// <summary>
        /// Gets a list of all items
        /// </summary>
        protected override IEnumerable<CppElement> AllItems
        {
            get
            {
                // Make copy of all elements and add return type
                List<CppElement> allElements = new List<CppElement>(Iterate<CppElement>());
                allElements.Add(ReturnType);

                return allElements;
            }
        }

        /// <summary>
        /// Gets or sets a value indicating whether this method is virtual.
        /// </summary>
        [XmlAttribute("virtual")]
        public bool Virtual { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether this instance is override.
        /// </summary>
        [XmlAttribute("override")]
        public bool Override { get; set; }

        /// <summary>
        /// Returns a <see cref="System.String"/> that represents this instance.
        /// </summary>
        public override string ToString()
        {
            StringBuilder builder = new StringBuilder();
            
            // Check if virtual method
            if (Virtual)
            {
                builder.Append("virtual");
                builder.Append(" ");
            }

            builder.Append(ReturnType);
            builder.Append(" ");

            // Check if parent is interface or class
            if (Parent is CppInterface || Parent is CppClass)
            {
                builder.Append(Parent.Name);
                builder.Append("::");
            }

            // Build method
            builder.Append(Name);
            builder.Append("(");

            int i = 0, count = Parameters.Count();
            foreach (var cppParameter in Parameters)
            {
                builder.Append(cppParameter);
                if ((i + 1) < count)
                {
                    builder.Append(",");
                }
                i++;
            }
            builder.Append(")");

            // Check if we override base method
            if (Override)
            {
                builder.Append(" ");
                builder.Append("override");
            }

            return builder.ToString();
        }
    }
}
