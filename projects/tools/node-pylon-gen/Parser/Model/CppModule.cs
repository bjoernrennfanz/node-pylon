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
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Serialization;

namespace NodePylonGen.Parser.Model
{
    /// <summary>
    /// A C++ module contains includes.
    /// </summary>
    [XmlRoot("cpp-module", Namespace = NS)]
    public class CppModule : CppElement
    {
        internal const string NS = "urn:NodePylonGen.Parser.Model";

        /// <summary>
        /// Gets the full name.
        /// </summary>
        [XmlIgnore]
        public override string FullName
        {
            get { return ""; }
        }

        /// <summary>
        /// Gets the includes.
        /// </summary>
        [XmlIgnore]
        public IEnumerable<CppInclude> Includes
        {
            get { return Iterate<CppInclude>(); }
        }

        /// <summary>
        /// Finds the include.
        /// </summary>
        public CppInclude FindInclude(string includeName)
        {
            return (from cppInclude in Iterate<CppInclude>()
                where cppInclude.Name == includeName
                select cppInclude).FirstOrDefault();
        }

        /// <summary>
        /// Reads the module from the specified file.
        /// </summary>
        public static CppModule Read(string file)
        {
            FileStream inputStream = new FileStream(file, FileMode.Open);
            CppModule result = Read(inputStream);
            inputStream.Close();

            return result;
        }

        /// <summary>
        /// Reads the module from the specified input.
        /// </summary>
        public static CppModule Read(Stream input)
        {
            CppModule module = null;
            XmlSerializer xmlSerializer = new XmlSerializer(typeof(CppModule));

            using (XmlReader xmlReader = XmlReader.Create(input))
            {
                module = xmlSerializer.Deserialize(xmlReader) as CppModule;
            }

            if (module != null)
            {
                module.ResetParents();
            }
                
            return module;
        }

        /// <summary>
        /// Writes this instance to the specified file.
        /// </summary>
        public void Write(string file)
        {
            FileStream outputStream = new FileStream(file, FileMode.Create);
            Write(outputStream);
            outputStream.Close();
        }

        /// <summary>
        /// Writes this instance to the specified output.
        /// </summary>
        public void Write(Stream output)
        {
            XmlSerializer xmlSerializer = new XmlSerializer(typeof(CppModule));

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;

            using (XmlWriter xmlWriter = XmlWriter.Create(output, settings))
            {
                XmlSerializerNamespaces xmlSerializerNamespaces = new XmlSerializerNamespaces();
                xmlSerializerNamespaces.Add("", NS);

                xmlSerializer.Serialize(xmlWriter, this, xmlSerializerNamespaces);
            }
        }
    }
}
