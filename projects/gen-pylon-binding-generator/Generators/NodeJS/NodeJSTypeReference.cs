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

using CppSharp;
using CppSharp.AST;
using CppSharp.AST.Extensions;
using CppSharp.Generators.AST;
using GenPylonBinding.Core.Config;
using GenPylonBinding.Generator.Model.Types;
using System.Collections.Generic;
using System.IO;

namespace GenPylonBinding.Generator.Generators.NodeJS
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
        private readonly CppSharp.Types.ITypeMapDatabase TypeMapDatabase;
        private readonly DriverOptions DriverOptions;
        private TranslationUnit TranslationUnit;

        private Dictionary<Declaration, NodeJSTypeReference> typeReferences;

        public NodeJSTypeReferenceCollector(ConfigMapping configurationContext, CppSharp.Types.ITypeMapDatabase typeMapDatabase, DriverOptions driverOptions)
        {
            ConfigurationContext = configurationContext;
            TypeMapDatabase = typeMapDatabase;
            DriverOptions = driverOptions;

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

        public void Process(Namespace @namespace, bool filterNamespaces = false)
        {
            TranslationUnit = @namespace.TranslationUnit;

            var collector = new RecordCollector(TranslationUnit);
            @namespace.Visit(collector);

            foreach (var record in collector.Declarations)
            {
                if (record.Value is Namespace)
                    continue;

                if (record.Value.IsDependent)
                    continue;

                record.Value.Visit(this);
                GenerateInclude(record);
            }
        }

        private void GenerateInclude(ASTRecord<Declaration> record)
        {
            var decl = record.Value;
            if (decl.Namespace == null)
            {
                return;
            }

            // Find a type map for the declaration and use it if it exists.
            CppSharp.Types.TypeMap typeMap;
            if (TypeMapDatabase.FindTypeMap(record.Value, out typeMap))
            {
                (typeMap as TypeMap).Declaration = record.Value;
                (typeMap as TypeMap).NodeJSTypeReference(this, record);

                return;
            }

            TranslationUnit translationUnit = decl.Namespace.TranslationUnit;

            if (translationUnit.IsSystemHeader)
            {
                return;
            }

            if (!decl.IsGenerated)
            {
                return;
            }

            if (IsBuiltinTypedef(decl))
            {
                return;
            }

            NodeJSTypeReference typeRef = GetTypeReference(decl);
            if (typeRef.Include.TranslationUnit == null)
            {
                typeRef.Include = new Include
                {
                    Type = translationUnit.IsGenerated ? Include.IncludeType.Quoted : Include.IncludeType.Angled,
                    File = GetIncludePath(translationUnit),
                    TranslationUnit = translationUnit
                };
            }

            typeRef.Include.InHeader |= IsIncludeInHeader(record);
        }

        private string GetIncludePath(TranslationUnit translationUnit)
        {
            string rel = PathHelpers.GetRelativePath(TranslationUnit.FileRelativeDirectory, translationUnit.FileRelativeDirectory);
            if (string.IsNullOrEmpty(rel))
            {
                return Path.ChangeExtension(translationUnit.FileName, "h");
            }

            return Path.Combine(rel, Path.ChangeExtension(translationUnit.FileName, "h"));
        }

        private bool IsBuiltinTypedef(Declaration decl)
        {
            var typedefDecl = decl as TypedefDecl;
            if (typedefDecl == null) return false;
            if (typedefDecl.Type is BuiltinType) return true;

            var typedefType = typedefDecl.Type as TypedefType;
            if (typedefType == null) return false;
            if (typedefType.Declaration == null) return false;

            return typedefType.Declaration.Type is BuiltinType;
        }

        private bool IsIncludeInHeader(ASTRecord<Declaration> record)
        {
            if (TranslationUnit == record.Value.Namespace.TranslationUnit)
            {
                return false;
            }

            return true;
        }

        public override bool VisitDeclaration(Declaration decl)
        {
            return decl.IsDeclared && (decl.Namespace == null || !decl.TranslationUnit.IsSystemHeader);
        }

        public override bool VisitClassDecl(Class @class)
        {
            if (!VisitDeclaration(@class))
            {
                return false;
            }

            if (@class.IsIncomplete && @class.CompleteDeclaration != null)
            {
                @class = (Class)@class.CompleteDeclaration;
            }
                
            var keywords = @class.IsValueType ? "struct" : "class";
            var @ref = string.Format("{0} {1};", keywords, @class.Name);
            GetTypeReference(@class).FowardReference = @ref;

            return false;
        }

        public override bool VisitEnumDecl(Enumeration @enum)
        {
            if (!VisitDeclaration(@enum))
            {
                return false;
            }

            var @base = "";
            if (!@enum.Type.IsPrimitiveType(PrimitiveType.Int))
            {
                @base = string.Format(" : {0}", @enum.Type);
            }

            var @ref = string.Format("enum {0}{1};", @enum.Name, @base);
            GetTypeReference(@enum).FowardReference = @ref;

            return false;
        }
    }
}
