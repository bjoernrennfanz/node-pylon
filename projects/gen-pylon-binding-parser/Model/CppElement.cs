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
using System.Xml.Serialization;

namespace GenPylonBinding.Parser.Model
{
    public abstract class CppElement
    {
        /// <summary>
        /// Default constructor
        /// </summary>
        public CppElement()
        {
        }

        /// <summary>
        /// Copy constructor
        /// </summary>
        public CppElement(CppElement otherCppElement)
        {
            Name = otherCppElement.Name;
            Namespace = otherCppElement.Namespace;
            Id = otherCppElement.Namespace;
            Description = otherCppElement.Description;
            Remarks = otherCppElement.Remarks;
            Parent = otherCppElement.Parent;
            Items = otherCppElement.Items;
        }

        /// <summary>
        /// Gets or sets the name.
        /// </summary>
        [XmlAttribute("name")]
        public string Name { get; set; }

        /// <summary>
        /// Gets or sets the namespace.
        /// </summary>
        [XmlAttribute("namespace")]
        public string Namespace { get; set; }

        /// <summary>
        /// Gets or sets the id.
        /// </summary>
        [XmlElement("id")]
        public string Id { get; set; }

        /// <summary>
        /// Gets or sets the description.
        /// </summary>
        [XmlElement("description")]
        public string Description { get; set; }

        /// <summary>
        /// Gets or sets the remarks.
        /// </summary>
        [XmlElement("remarks")]
        public string Remarks { get; set; }

        /// <summary>
        /// Gets or sets the parent.
        /// </summary>
        [XmlIgnore]
        public CppElement Parent { get; set; }

        /// <summary>
        /// Gets the parent include.
        /// </summary>
        [XmlIgnore]
        public CppInclude ParentInclude
        {
            get
            {
                CppElement cppInclude = Parent;
                while (cppInclude != null && !(cppInclude is CppInclude))
                {
                    cppInclude = cppInclude.Parent;
                }

                return cppInclude as CppInclude;
            }
        }

        /// <summary>
        /// Gets the parent root C++ element.
        /// </summary>
        [XmlIgnore]
        public CppElement ParentRoot
        {
            get
            {
                CppElement cppRoot = this;
                while (cppRoot.Parent != null)
                {
                    cppRoot = cppRoot.Parent;
                }
                    
                return cppRoot;
            }
        }

        /// <summary>
        /// Gets the path.
        /// </summary>
        [XmlIgnore]
        public virtual string Path
        {
            get
            {
                if (Parent != null)
                {
                    return Parent.FullName;
                }
                    
                return "";
            }
        }

        /// <summary>
        /// Gets the full name.
        /// </summary>
        [XmlIgnore]
        public virtual string FullName
        {
            get
            {
                string path = Path;
                string name = String.IsNullOrEmpty(Name) ? "" : Name;

                return String.IsNullOrEmpty(path) ? name : path + "::" + name;
            }
        }

        /// <summary>
        /// Return all items inside this C++ element.
        /// </summary>
        [XmlArray("items")]
        [XmlArrayItem(typeof(CppClass))]
        [XmlArrayItem(typeof(CppConstructor))]
        [XmlArrayItem(typeof(CppConstant))]
        [XmlArrayItem(typeof(CppDefine))]
        [XmlArrayItem(typeof(CppEnum))]
        [XmlArrayItem(typeof(CppEnumItem))]
        [XmlArrayItem(typeof(CppField))]
        [XmlArrayItem(typeof(CppFunction))]
        [XmlArrayItem(typeof(CppInclude))]
        [XmlArrayItem(typeof(CppInterface))]
        [XmlArrayItem(typeof(CppMethod))]
        [XmlArrayItem(typeof(CppParameter))]
        [XmlArrayItem(typeof(CppStruct))]
        [XmlArrayItem(typeof(CppType))]
        public List<CppElement> Items { get; set; }

        /// <summary>
        /// Gets a list of all items
        /// </summary>
        protected virtual IEnumerable<CppElement> AllItems
        {
            get { return Iterate<CppElement>(); }
        }

        /// <summary>
        /// Flag that indicates empty items list
        /// </summary>
        [XmlIgnore]
        public bool IsEmpty
        {
            get { return Items == null || Items.Count == 0; }
        }

        /// <summary>
        /// Add an inner element to this CppElement
        /// </summary>
        public void Add(CppElement element)
        {
            if (element.Parent != null)
            {
                element.Parent.Remove(element);
            }

            element.Parent = this;
            GetSafeItems().Add(element);
        }

        /// <summary>
        /// Add an inner element to this CppElement
        /// </summary>
        public void Add<T>(IEnumerable<T> elements) where T : CppElement
        {
            foreach (var cppElement in elements)
            {
                Add(cppElement);
            }
        }

        /// <summary>
        ///   Remove an inner element to this CppElement
        /// </summary>
        public void Remove(CppElement element)
        {
            element.Parent = null;
            GetSafeItems().Remove(element);
        }

        /// <summary>
        /// Get instance or new instance of items list
        /// </summary>
        /// <returns></returns>
        private List<CppElement> GetSafeItems()
        {
            if (Items == null)
            {
                Items = new List<CppElement>();
            }
                
            return Items;
        }

        /// <summary>
        ///   Iterates on items on this instance.
        /// </summary>
        public IEnumerable<T> Iterate<T>() where T : CppElement
        {
            return Items == null ? Enumerable.Empty<T>() : Items.OfType<T>();
        }

        /// <summary>
        /// Resets all parents
        /// </summary>
        protected void ResetParents()
        {
            foreach (CppElement innerElement in Iterate<CppElement>())
            {
                innerElement.Parent = this;
                innerElement.ResetParents();
            }
        }

        /// <summary>
        /// Gets a string representation of this <see cref="CppElement"/>
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return GetType().Name + " [" + Name + "]";
        }

        public abstract T Visit<T>(ICppElementVisitor<T> visitor);
    }
}
