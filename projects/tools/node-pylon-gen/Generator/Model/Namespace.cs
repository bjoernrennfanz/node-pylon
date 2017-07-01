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
using NodePylonGen.Parser.Model;
using System.Collections.Generic;

namespace NodePylonGen.Generator.Model
{
    /// <summary>
    /// Represents a declaration context.
    /// </summary>
    public abstract class DeclarationContext : Declaration
    {
        public bool IsAnonymous { get; set; }

        #region Iterators

        /// <summary>
        /// Gets the namespaces
        /// </summary>
        public DeclarationIterator<Namespace> Namespaces
        {
            get { return new DeclarationIterator<Namespace>(Items); }
        }

        /// <summary>
        /// Gets the macros
        /// </summary>
        public DeclarationIterator<CppDefine> Macros
        {
            get { return new DeclarationIterator<CppDefine>(Items); }
        }

        /// <summary>
        /// Gets the classes.
        /// </summary>
        public DeclarationIterator<CppClass> Classes
        {
            get { return new DeclarationIterator<CppClass>(Items); }
        }

        /// <summary>
        /// Gets the interfaces.
        /// </summary>
        public DeclarationIterator<CppInterface> Interfaces
        {
            get { return new DeclarationIterator<CppInterface>(Items); }
        }

        /// <summary>
        /// Gets the functions.
        /// </summary>
        public DeclarationIterator<CppFunction> Functions
        {
            get { return new DeclarationIterator<CppFunction>(Items); }
        }

        /// <summary>
        /// Gets the structs.
        /// </summary>
        public DeclarationIterator<CppStruct> Structs
        {
            get { return new DeclarationIterator<CppStruct>(Items); }
        }

        /// <summary>
        /// Gets the enums.
        /// </summary>
        public DeclarationIterator<CppEnum> Enums
        {
            get { return new DeclarationIterator<CppEnum>(Items); }
        }

        /// <summary>
        /// Gets the constants.
        /// </summary>
        public DeclarationIterator<CppConstant> Constants
        {
            get { return new DeclarationIterator<CppConstant>(Items); }
        }

        #endregion

        #region Find helpers

        public Namespace FindNamespace(string name)
        {
            string[] namespaces = name.Split(new string[] { "::" },
                StringSplitOptions.RemoveEmptyEntries);

            return FindNamespace(namespaces);
        }

        public Namespace FindNamespace(IEnumerable<string> namespaces)
        {
            DeclarationContext currentNamespace = this;
            foreach (var @namespace in namespaces)
            {
                var childNamespace = currentNamespace.Namespaces.Find(
                    e => e.Name.Equals(@namespace));

                if (childNamespace == null)
                {
                    return null;
                }

                currentNamespace = childNamespace;
            }

            return currentNamespace as Namespace;
        }

        public Namespace FindCreateNamespace(string name)
        {
            string lastNamespace = "";
            string[] namespaces = name.Split(new string[] { "::" },
                StringSplitOptions.RemoveEmptyEntries);

            Namespace childNamespace = null;
            DeclarationContext currentNamespace = this;

            foreach (String @namespace in namespaces)
            {
                childNamespace = currentNamespace.Namespaces.Find(
                    item => item.Name.Equals(@namespace));

                // Check if child namespace was found
                if (childNamespace != null)
                {
                    currentNamespace = childNamespace;
                }

                lastNamespace = @namespace;
            }

            if (childNamespace == null && !string.IsNullOrEmpty(lastNamespace))
            {
                childNamespace = new Namespace
                {
                    Name = lastNamespace,
                    Namespace = this,
                };

                currentNamespace.Items.Add(childNamespace);
                childNamespace.Parent = currentNamespace;
            }

            return childNamespace;
        }

        #endregion
    }

    /// <summary>
    /// Represents a C++ namespace.
    /// </summary>
    public class Namespace : DeclarationContext
    {
        public override string LogicalName
        {
            get { return IsInline ? string.Empty : base.Name; }
        }

        public override string LogicalOriginalName
        {
            get { return IsInline ? string.Empty : base.OriginalName; }
        }

        public bool IsInline;

        public override T Visit<T>(IDeclarationVisitor<T> visitor)
        {
            return visitor.VisitNamespace(this);
        }
    }
}