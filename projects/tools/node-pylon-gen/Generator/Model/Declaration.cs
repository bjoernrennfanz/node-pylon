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
    public abstract class Declaration : CppElement
    {
        #region Constructors


        #endregion

        
        private DeclarationContext declarationNamespace;
        public DeclarationContext OriginalNamespace;

        /// <summary>
        /// Namespace the declaration is contained in.
        /// </summary>
        public new DeclarationContext Namespace
        {
            get { return declarationNamespace; }
            set
            {
                declarationNamespace = value;
                if (OriginalNamespace == null)
                {
                    OriginalNamespace = declarationNamespace;
                }
            }
        }

        /// <summary>
        /// The original name of the declaration
        /// </summary>
        public virtual string OriginalName { get; set; }

        /// <summary>
        /// The name of the declaration
        /// </summary>
        public new virtual string Name
        {
            get { return base.Name; }
            set
            {
                base.Name = value;
                if (OriginalName == null)
                {
                    OriginalName = base.Name;
                }
            }
        }

        public TranslationUnit TranslationUnit
        {
            get
            {
                if (this is TranslationUnit)
                {
                    return this as TranslationUnit;
                }
                    
                return Namespace.TranslationUnit;
            }
        }

        #region Visitor implementation

        public override T Visit<T>(ICppElementVisitor<T> visitor)
        {
            return visitor.VisitCppElement(this);
        }

        public abstract T Visit<T>(IDeclarationVisitor<T> visitor);

        #endregion
    }

    /// <summary>
    /// Interface for declaration visitors
    /// </summary>
    public interface IDeclarationVisitor<out T> : ICppElementVisitor<T>
    {
        T VisitDeclaration(Declaration declaration);
        T VisitNamespace(Namespace declarationNamespace);
        T VisitTranslationUnit(TranslationUnit unit);
    }
}