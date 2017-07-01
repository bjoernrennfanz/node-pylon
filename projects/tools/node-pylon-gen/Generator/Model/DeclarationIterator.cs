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

using NodePylonGen.Parser.Model;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace NodePylonGen.Generator.Model
{
    public struct DeclarationIterator<T> : IEnumerable<T> where T : CppElement
    {
        private readonly List<CppElement> Elements;

        public DeclarationIterator(List<CppElement> elements)
        {
            Elements = elements;
        }

        public int Count
        {
            get { return Elements.OfType<T>().ToArray().Length; }
        }

        public T this[int index]
        {
            get { return Elements.OfType<T>().ToArray()[index]; }
        }

        public void Add(T declaration)
        {
            Elements.Add(declaration);
        }

        public void AddRange(IEnumerable<T> range)
        {
            Elements.AddRange(range);
        }

        public T Find(Func<T, bool> predicate)
        {
            return Elements.OfType<T>().SingleOrDefault<T>(predicate);
        }

        public int FindIndex(Predicate<T> predicate)
        {
            return Elements.OfType<T>().ToList().FindIndex(predicate);
        }

        public bool Exists(Func<T, bool> predicate)
        {
            return Elements.OfType<T>().Any(predicate);
        }

        public IEnumerator<T> GetEnumerator()
        {
            return Elements.OfType<T>().GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public void Replace(T decl, T newDecl)
        {
            Elements[Elements.FindIndex(d => d == decl)] = newDecl;
        }
    }
}
