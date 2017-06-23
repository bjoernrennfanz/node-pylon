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
using System.Linq;

namespace NodePylonGen.Generator.Rules
{
    /// <summary>
    /// This class is used to build rules that will be run against the 
    /// <see cref="CppIncludes"/> that comes from C++.
    /// </summary>
    public class RulesBuilder<T>
    {
        public BindingContext Context { get; private set; }
        public List<T> Rules { get; private set; }

        public RulesBuilder(BindingContext context)
        {
            Context = context;
            Rules = new List<T>();
        }

        /// <summary>
        /// Adds a new rule to the builder.
        /// </summary>
        public void AddRule(T rule)
        {
            if (rule is CppIncludeRule)
            {
                (rule as CppIncludeRule).Context = Context;
            }

            Rules.Add(rule);
        }

        /// <summary>
        /// Finds a previously-added rule of the given type.
        /// </summary>
        public U FindRule<U>() where U : CppIncludeRule
        {
            return Rules.OfType<U>().Select(rule => rule as U).FirstOrDefault();
        }

        /// <summary>
        /// Runs the rules in the builder.
        /// </summary>
        public void RunRules(Action<T> action)
        {
            foreach (var rule in Rules)
            {
                action(rule);
            }
        }
    }
}
