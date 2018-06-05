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
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace GenPylonBinding.Parser.Model
{
    /// <summary>
    /// A C++ constructor.
    /// </summary>
    [XmlType("constructor")]
    public class CppConstructor : CppElement
    {
        /// <summary>
        /// Gets or sets the offset.
        /// </summary>
        [XmlAttribute("offset")]
        public int Offset { get; set; }

        /// <summary>
        /// Gets or sets the access.
        /// </summary>
        [XmlAttribute("access")]
        public string Access { get; set; }

        /// <summary>
        /// Gets the parameters.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppParameter> Parameters
        {
            get { return Iterate<CppParameter>(); }
        }

        /// <summary>
        /// Returns a <see cref="System.String"/> that represents this instance.
        /// </summary>
        public override string ToString()
        {
            StringBuilder builder = new StringBuilder();

            // Check if access is set
            if (!string.IsNullOrEmpty(Access))
            {
                builder.Append(Access);
                builder.Append(" ");
            }

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

            return builder.ToString();
        }

        /// <summary>
        /// Overriden version of Visit<T>
        /// </summary>
        public override T Visit<T>(ICppElementVisitor<T> visitor)
        {
            return visitor.VisitCppConstructor(this);
        }
    }
}
