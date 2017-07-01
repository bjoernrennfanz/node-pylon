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

namespace NodePylonGen.Generator.Model
{
    public class TranslationUnit : Namespace
    {
        /// <summary>
        /// Copy constructor from <see cref="CppInclude"/> 
        /// </summary>
        public TranslationUnit(CppInclude include)
        {
            foreach (CppElement element in include.Items)
            {
                if (!string.IsNullOrEmpty(element.Namespace))
                {
                    Namespace declarationNamespace = FindCreateNamespace(element.Namespace);
                    declarationNamespace.Items.Add(element);
                    element.Parent = this;
                }
                else
                {
                    Items.Add(element);
                    element.Parent = this;
                }
            }
        }

        /// <summary>
        /// Construct a empty translationunit with given filename
        /// </summary>
        public TranslationUnit(string file)
        {
            FilePath = file;
        }

        private string fileName;
        private string fileNameWithoutExtension;

        /// <summary>
        /// Contains the name of the file.
        /// </summary>
        public string FileName
        {
            get
            {
                if (string.IsNullOrEmpty(fileName))
                {
                    fileName = System.IO.Path.GetFileName(FilePath);
                }

                return fileName;
            }
        }

        /// <summary>
        /// Contains the name of the module.
        /// </summary>
        public string FileNameWithoutExtension
        {
            get
            {
                if (string.IsNullOrEmpty(fileNameWithoutExtension))
                {
                    fileNameWithoutExtension = System.IO.Path.GetFileNameWithoutExtension(FileName);
                }

                return fileNameWithoutExtension;
            }
        }

        /// <summary>
        /// Contains the include path.
        /// </summary>
        public string IncludePath { get; internal set; }

        /// <summary>
        /// Contains the file path.
        /// </summary>
        public string FilePath { get; internal set; }
    }
}