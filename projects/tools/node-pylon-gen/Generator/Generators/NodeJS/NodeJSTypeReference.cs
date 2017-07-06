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

using System.Collections.Generic;
using NodePylonGen.Config;
using CppSharp.AST;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    /// <summary>
    /// NodeJS type reference
    /// </summary>
    public class NodeJSTypeReference : TypeReference
    {
        public Include Include;

        public override string ToString()
        {
            if (Include.InHeader)
            {
                return Include.ToString();
            }
                
            if (!string.IsNullOrWhiteSpace(FowardReference))
            {
                return FowardReference;
            }

            return Include.ToString();
        }
    }

    public class NodeJSTypeReferenceCollector : AstVisitor
    {

        private readonly ConfigMapping ConfigurationContext;
        private TranslationUnit TranslationUnit;

        private Dictionary<Declaration, NodeJSTypeReference> typeReferences;

        public NodeJSTypeReferenceCollector(ConfigMapping configurationContext)
        {
            ConfigurationContext = configurationContext;
            typeReferences = new Dictionary<Declaration, NodeJSTypeReference>();
        }

        public IEnumerable<NodeJSTypeReference> TypeReferences
        {
            get { return typeReferences.Values; }
        }

        public NodeJSTypeReference GetTypeReference(Declaration decl)
        {
            if (typeReferences.ContainsKey(decl))
            {
                return typeReferences[decl];
            }

            NodeJSTypeReference reference = new NodeJSTypeReference { Declaration = decl };
            typeReferences.Add(decl, reference);

            return reference;
        }

        public void Process(Namespace declNamespace, bool filterNamespaces = false)
        {
            TranslationUnit = declNamespace.TranslationUnit;
            // TODO
        }
    }
}
