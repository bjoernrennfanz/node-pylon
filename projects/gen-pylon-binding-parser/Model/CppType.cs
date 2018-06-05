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

using System.Text;
using System.Xml.Serialization;

namespace GenPylonBinding.Parser.Model
{
    /// <summary>
    /// Type declaration.
    /// </summary>
    [XmlType("type")]
    public class CppType : CppElement
    {
        /// <summary>
        /// Gets or sets the name of the type.
        /// </summary>
        [XmlAttribute("type-name")]
        public string TypeName { get; set; }

        /// <summary>
        /// Gets or sets the pointer.
        /// </summary>
        [XmlAttribute("ptr")]
        public string Pointer { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether this <see cref="CppType"/> is const.
        /// </summary>
        [XmlAttribute("const")]
        public bool Const { get; set; }

        /// <summary>
        /// Gets or sets a value indicating whether this instance is array.
        /// </summary>
        [XmlAttribute("array")]
        public bool IsArray { get; set; }

        /// <summary>
        /// Gets or sets the array dimension.
        /// </summary>
        [XmlAttribute("array-dim")]
        public string ArrayDimension { get; set; }

        /// <summary>
        /// Returns a <see cref="System.String"/> that represents this instance.
        /// </summary>
        public override string ToString()
        {
            StringBuilder builder = new StringBuilder();
            if (Const)
            {
                builder.Append("const ");
            }
            builder.Append(TypeName);
            builder.Append(Pointer);

            if (!string.IsNullOrEmpty(Name))
            {
                builder.Append(" ");
                builder.Append(Name);
            }

            if (IsArray)
            {
                builder.Append("[");
                builder.Append(ArrayDimension);
                builder.Append("]");
            }
            return builder.ToString();
        }

        /// <summary>
        /// Check if given <see cref="CppType"/> instance are equal this instance.
        /// </summary>
        public bool Equals(CppType other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;

            return Equals(other.TypeName, TypeName) && Equals(other.Pointer, Pointer);
        }

        /// <summary>
        /// Check if given <see cref="System.Object"/> instance are equal this instance.
        /// </summary>
        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != typeof(CppType)) return false;

            return Equals((CppType)obj);
        }

        /// <summary>
        /// Get hash code of this <see cref="CppType"/> instance.
        /// </summary>
        /// <returns></returns>
        public override int GetHashCode()
        {
            return (((TypeName != null ? TypeName.GetHashCode() : 0) + (Pointer != null ? Pointer.GetHashCode() : 0)) / 2);
        }

        /// <summary>
        /// Overriden version of Visit<T>
        /// </summary>
        public override T Visit<T>(ICppElementVisitor<T> visitor)
        {
            return visitor.VisitCppType(this);
        }
    }
}