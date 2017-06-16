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

using System.Xml.Serialization;

namespace NodePylonGen.Parser.Model
{
    /// <summary>
    /// A C++ field.
    /// </summary>
    [XmlType("field")]
    public class CppField : CppType
    {
        /// <summary>
        /// Index of the field mostly used to handle unions.
        /// </summary>
        [XmlAttribute("offset")]
        public int Offset { get; set; }

        /// <summary>
        /// Used only for structure with bits
        /// </summary>
        [XmlAttribute("bitfield")]
        public bool IsBitField { get; set; }

        /// <summary>
        /// Used only for structure with bits
        /// </summary>
        [XmlAttribute("bitoffset")]
        public int BitOffset { get; set; }

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        public bool Equals(CppField other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return base.Equals(other) && other.Offset == Offset && other.IsBitField.Equals(IsBitField) && other.BitOffset == BitOffset;
        }

        /// <summary>
        /// Determines whether the specified <see cref="System.Object"/> is equal to this instance.
        /// </summary>
        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            return Equals(obj as CppField);
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        public override int GetHashCode()
        {
            int result = base.GetHashCode();
            result += Offset.GetHashCode();
            result += IsBitField.GetHashCode();
            result += BitOffset.GetHashCode();
            result /= 4;

            return result;
        }
    }
}