﻿// MIT License
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
using System.Xml.Serialization;

namespace NodePylonGen.Parser.Model
{
    public class CppBase : CppElement
    {
        /// <summary>
        /// Gets or sets the name of the parent.
        /// </summary>
        [XmlAttribute("base")]
        public string ParentName { get; set; }

        /// <summary>
        /// Gets the constructors.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppConstructor> Constructors
        {
            get { return Iterate<CppConstructor>(); }
        }

        /// <summary>
        /// Gets or sets the total constructor count.
        /// </summary>
        internal int TotalConstructorCount { get; set; }

        /// <summary>
        /// Gets the methods.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppMethod> Methods
        {
            get { return Iterate<CppMethod>(); }
        }

        /// <summary>
        /// Gets or sets the total method count.
        /// </summary>
        internal int TotalMethodCount { get; set; }

        /// <summary>
        /// Overriden version of Visit<T>
        /// </summary>
        public override T Visit<T>(ICppElementVisitor<T> visitor)
        {
            return visitor.VisitCppBase(this);
        }
    }
}
