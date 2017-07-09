using CppSharp.AST;
using NodePylonGen.Config;
using NodePylonGen.Parser.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using static CppSharp.AST.ArrayType;

namespace NodePylonGen.Generator.Model
{
    public class ASTConverter : CppIncludeVisitor
    {
        private ASTContext currentASTContext;
        private ConfigMapping configurationContext;

        private CppModule moduleContext;

        private readonly Dictionary<CppElement, Declaration> cppElementToDeclarationMapping;

        public ASTConverter(ConfigMapping configurationContext, CppModule moduleContext)
        {
            this.configurationContext = configurationContext;
            this.moduleContext = moduleContext;

            cppElementToDeclarationMapping = new Dictionary<CppElement, Declaration>();
        }

        public ASTContext Convert()
        {
            currentASTContext = new ASTContext();
            moduleContext.Visit(this);

            return currentASTContext;
        }

        /// <summary>
        /// Find parent <see cref="Declaration"/> with given name. 
        /// </summary>
        private T FindDeclParent<T>(CppElement cppElement) where T : Declaration
        {
            switch (typeof(T).Name)
            {
                case "TranslationUnit":
                    return currentASTContext.TranslationUnits.Where(unit => unit.Name == cppElement.Name).First() as T;
                    
                default:
                    // Find any declaration matching the name, remove not valid elements
                    List<T> declarations = currentASTContext.FindDecl<T>(cppElement.Name).ToList();
                    declarations.RemoveAll(item => item == null);

                    return declarations.First();
            }
        }

        private IEnumerable<T> FindTypeDeclRecursive<T>(string typeName) where T : Declaration
        {
            foreach(TranslationUnit unit in currentASTContext.)

        }

        /// <summary>
        /// Get file path of cpp include
        /// </summary>
        private string GetFilePathOfCppInclude(ConfigMapping configurationContext, CppInclude unit)
        {
            ConfigMapping configOfUnit = null;
            IncludeMapping includeOfUnit = null;

            IEnumerable<ConfigMapping> configFilesLoaded = configurationContext.ConfigFilesLoaded;
            foreach (ConfigMapping configFileLoad in configFilesLoaded)
            {
                foreach (IncludeMapping include in configFileLoad.Includes)
                {
                    if (include.Id == unit.Name)
                    {
                        includeOfUnit = include;
                        configOfUnit = configFileLoad;
                        break;
                    }

                    if ((configOfUnit != null) && (includeOfUnit != null))
                    {
                        break;
                    }
                }
            }

            return (((configOfUnit != null) && (includeOfUnit != null)) ? System.IO.Path.Combine(System.IO.Path.GetDirectoryName(configOfUnit.AbsoluteFilePath), (string.IsNullOrEmpty(includeOfUnit.Alias) ? unit.Name : includeOfUnit.Alias.ToLower())) : "invalid") + ".gen";
        }

        /// <summary>
        /// Get include path for cpp include
        /// </summary>
        private string GetIncludePathOfCppInclude(ConfigMapping configurationContext, CppInclude unit)
        {
            IncludeMapping includeOfUnit = null;

            IEnumerable<ConfigMapping> configFilesLoaded = configurationContext.ConfigFilesLoaded;
            foreach (ConfigMapping configFileLoad in configFilesLoaded)
            {
                foreach (IncludeMapping include in configFileLoad.Includes)
                {
                    if (include.Id == unit.Name)
                    {
                        includeOfUnit = include;
                        break;
                    }
                }

                if (includeOfUnit != null)
                {
                    break;
                }
            }

            return (includeOfUnit != null) ? includeOfUnit.File : "invalid";
        }

        #region ICppElementVisitor implementation

        public override bool VisitCppClass(CppClass cppClass)
        {
            Declaration unitDecl;
            cppElementToDeclarationMapping.TryGetValue(cppClass.ParentInclude, out unitDecl);
            TranslationUnit unit = unitDecl != null ? unitDecl as TranslationUnit : FindDeclParent<TranslationUnit>(cppClass.ParentInclude);

            Namespace @namespace = null;
            Class @class = null;

            // Check if we have an namespace
            if (!string.IsNullOrEmpty(cppClass.Namespace))
            {
                @namespace = unit.FindCreateNamespace(cppClass.Namespace.TrimStart(':'));
                @class = @namespace.FindClass(cppClass.Name, true, true);
            }
            else
            {
                @class = unit.FindClass(cppClass.Name, true, true);
            }

            // Check if we have an base class
            if (!string.IsNullOrEmpty(cppClass.ParentName))
            {
                Class baseClass = null;
                if (@namespace != null)
                {
                    baseClass = @namespace.FindClass(cppClass.ParentName, true, true);
                }
                else
                {
                    baseClass = unit.FindClass(cppClass.ParentName, true, true);
                }

                // Set type of base class
                baseClass.Type = baseClass.Name.StartsWith("I") ? ClassType.Interface : ClassType.ValueType;

                // Add type tag
                @class.Bases.Add(new BaseClassSpecifier
                {
                    Access = AccessSpecifier.Public,
                    Type = new TagType(baseClass)
                });
            }

            cppElementToDeclarationMapping.Add(cppClass, @class);
            return base.VisitCppClass(cppClass);
        }

        public override bool VisitCppDefine(CppDefine cppDefine)
        {


            return base.VisitCppDefine(cppDefine);
        }

        public override bool VisitCppInterface(CppInterface cppInterface)
        {
            Declaration unitDecl;
            cppElementToDeclarationMapping.TryGetValue(cppInterface.ParentInclude, out unitDecl);
            TranslationUnit unit = unitDecl != null ? unitDecl as TranslationUnit : FindDeclParent<TranslationUnit>(cppInterface.ParentInclude);

            Namespace @namespace = null;
            Class @interface = null;

            // Check if we have an namespace
            if (!string.IsNullOrEmpty(cppInterface.Namespace))
            {
                @namespace = unit.FindCreateNamespace(cppInterface.Namespace.TrimStart(':'));
                @interface = @namespace.FindClass(cppInterface.Name, true, true);
            }
            else
            {
                @interface = unit.FindClass(cppInterface.Name, true, true);
            }

            @interface.Type = ClassType.Interface;

            // Check if we have an base class
            if (!string.IsNullOrEmpty(cppInterface.ParentName))
            {
                Class baseClass = null;
                if (@namespace != null)
                {
                    baseClass = @namespace.FindClass(cppInterface.ParentName, true, true);
                }
                else
                {
                    baseClass = unit.FindClass(cppInterface.ParentName, true, true);
                }

                // Set type of base class
                baseClass.Type = baseClass.Name.StartsWith("I") ? ClassType.Interface : ClassType.ValueType;

                // Add type tag
                @interface.Bases.Add(new BaseClassSpecifier
                {
                    Access = AccessSpecifier.Public,
                    Type = new TagType(baseClass)
                });
            }

            cppElementToDeclarationMapping.Add(cppInterface, @interface);
            return base.VisitCppInterface(cppInterface);
        }

        public override bool VisitCppStruct(CppStruct cppStruct)
        {
            
            return base.VisitCppStruct(cppStruct);
        }

        public override bool VisitCppEnum(CppEnum cppEnum)
        {
            return base.VisitCppEnum(cppEnum);
        }

        public override bool VisitCppConstant(CppConstant cppConstant)
        {
            return base.VisitCppConstant(cppConstant);
        }

        public override bool VisitCppConstructor(CppConstructor cppConstructor)
        {
            Declaration classDecl;
            cppElementToDeclarationMapping.TryGetValue(cppConstructor.Parent, out classDecl);
            Class @class = classDecl != null ? classDecl as Class : FindDeclParent<Class>(cppConstructor.Parent);

            Method method = new Method();

            method.Kind = CXXMethodKind.Constructor;
            method.Access = AccessSpecifier.Public;
            method.Name = cppConstructor.Name;
            method.Namespace = @class.Namespace;

            // Check for default constructor
            if (cppConstructor.Parameters.Count() == 0)
            {
                method.IsDefaultConstructor = true;
            }

            // Check for copy constructor
            if ((cppConstructor.Parameters.Count() == 1) && (cppConstructor.Parameters.First().TypeName == cppConstructor.Name))
            {
                method.IsCopyConstructor = true;
            }

            @class.Methods.Add(method);

            cppElementToDeclarationMapping.Add(cppConstructor, method);
            return base.VisitCppConstructor(cppConstructor);
        }

        public override bool VisitCppField(CppField cppField)
        {
            
            return base.VisitCppField(cppField);
        }

        public override bool VisitCppParameter(CppParameter cppParameter)
        {
            Declaration declaration;
            cppElementToDeclarationMapping.TryGetValue(cppParameter.Parent, out declaration);
            Parameter parameter = new Parameter();

            string parentType = cppParameter.Parent.GetType().Name;
            switch (parentType)
            {
                case "CppMethod":
                case "CppConstructor":
                    Method method = declaration != null ? declaration as Method : FindDeclParent<Method>(cppParameter.Parent);

                    parameter.Name = cppParameter.Name;
                    parameter.Namespace = method.Namespace;
                    parameter.QualifiedType = GetQualifiedType(cppParameter);

                    method.Parameters.Add(parameter);
                    cppElementToDeclarationMapping.Add(cppParameter, parameter);
                    break;

                case "CppFunction":
                    Function function = declaration != null ? declaration as Function : FindDeclParent<Function>(cppParameter.Parent);

                    parameter.Name = cppParameter.Name;
                    parameter.Namespace = function.Namespace;
                    parameter.QualifiedType = GetQualifiedType(cppParameter);

                    function.Parameters.Add(parameter);
                    cppElementToDeclarationMapping.Add(cppParameter, parameter);
                    break;

                default:
                    break;
            }

            return base.VisitCppParameter(cppParameter);
        }

        public override bool VisitCppMethod(CppMethod cppMethod)
        {
            Declaration declaration;
            cppElementToDeclarationMapping.TryGetValue(cppMethod.Parent, out declaration);

            switch(cppMethod.GetType().Name)
            {
                case "CppFunction":
                    TranslationUnit unit = declaration != null ? declaration as TranslationUnit : FindDeclParent<TranslationUnit>(cppMethod.Parent);
                    Function function = new Function();

                    function.Name = cppMethod.Name;
                    function.Namespace = unit.FindNamespace(cppMethod.Namespace);
                    function.ReturnType = GetQualifiedType(cppMethod.ReturnType);

                    unit.Functions.Add(function);
                    cppElementToDeclarationMapping.Add(cppMethod, function);
                    break;

                default:
                    Class @class = declaration != null ? declaration as Class : FindDeclParent<Class>(cppMethod.Parent);
                    Method method = new Method();

                    method.Kind = CXXMethodKind.Normal;
                    method.Name = cppMethod.Name;
                    method.Namespace = @class.Namespace;

                    method.Access = AccessSpecifier.Public;
                    method.IsOverride = cppMethod.Override;
                    method.IsVirtual = cppMethod.Virtual;

                    method.ReturnType = GetQualifiedType(cppMethod.ReturnType);

                    @class.Methods.Add(method);
                    cppElementToDeclarationMapping.Add(cppMethod, method);
                    break;
            }

            return base.VisitCppMethod(cppMethod);
        }

        public override bool VisitCppEnumItem(CppEnumItem cppEnumItem)
        {
            return base.VisitCppEnumItem(cppEnumItem);
        }

        public override bool VisitCppInclude(CppInclude cppInclude)
        {
            string filePath = GetFilePathOfCppInclude(configurationContext, cppInclude);
            string includePath = GetIncludePathOfCppInclude(configurationContext, cppInclude);

            TranslationUnit unit = currentASTContext.FindOrCreateTranslationUnit(filePath);
            unit.Name = cppInclude.Name;
            unit.IncludePath = includePath;

            cppElementToDeclarationMapping.Add(cppInclude, unit);
            return base.VisitCppInclude(cppInclude);
        }

        #endregion

        #region Private helpers

        private QualifiedType GetQualifiedType(CppType cppType)
        {
            QualifiedType qualType = new QualifiedType()
            {
                Type = WalkType(cppType),
                Qualifiers = GetTypeQualifiers(cppType)
            };

            return qualType;
        }

        private TypeQualifiers GetTypeQualifiers(CppType cppType)
        {
            TypeQualifiers result = new TypeQualifiers();
            result.IsConst = cppType.Const;

            return result;
        }

        private CppSharp.AST.Type WalkType(CppType cppType)
        {
            CppSharp.AST.Type resultType;
            if (WalkPointerType(cppType, out resultType))
            {
                return resultType;
            }
            else if (WalkArrayType(cppType, out resultType))
            {
                return resultType;
            }

            return WalkType(cppType.TypeName);
        }

        private CppSharp.AST.Type WalkType(string typeName)
        {
            CppSharp.AST.Type resultType;
            if (WalkTagType(typeName, out resultType))
            {
                return resultType;
            }
            else if (WalkBuiltinType(typeName, out resultType))
            {
                return resultType;
            }

            return new UnsupportedType(typeName);
        }

        private bool WalkBuiltinType(string typeName, out CppSharp.AST.Type builtinType)
        {
            string typeNameLower = typeName.ToLower();
            switch (typeNameLower)
            {
                case "void": builtinType = new BuiltinType(PrimitiveType.Void); return true;
                case "bool": builtinType = new BuiltinType(PrimitiveType.Bool); return true;

                case "char": builtinType = new BuiltinType(PrimitiveType.Char); return true;
                case "signed char": builtinType = new BuiltinType(PrimitiveType.SChar); return true;
                case "unsigned char": builtinType = new BuiltinType(PrimitiveType.UChar); return true;

                case "short int": builtinType = new BuiltinType(PrimitiveType.Short); return true;
                case "short unsigned int": builtinType = new BuiltinType(PrimitiveType.UShort); return true;

                case "int": builtinType = new BuiltinType(PrimitiveType.Int); return true;
                case "unsigned int": builtinType = new BuiltinType(PrimitiveType.UInt); return true;

                case "long int": builtinType = new BuiltinType(PrimitiveType.Long); return true;
                case "long unsigned int": builtinType = new BuiltinType(PrimitiveType.ULong); return true;

                case "long long int": builtinType = new BuiltinType(PrimitiveType.Int128); return true;
                case "long long unsigned int": builtinType = new BuiltinType(PrimitiveType.UInt128); return true;

                case "float": builtinType = new BuiltinType(PrimitiveType.Float); return true;
                case "double": builtinType = new BuiltinType(PrimitiveType.Double); return true;
            }

            builtinType = null;
            return false;
        }

        private bool WalkTagType(string typeName, out CppSharp.AST.Type tagType)
        {
            //IEnumerable<Declaration> declarations = FindDeclRecursive<Declaration>(typeName);
            var test = currentASTContext.TranslationUnits.First().FindDeclaration(new[] { typeName });

            tagType = null;
            return false;
        }

        private bool WalkPointerType(CppType cppType, out CppSharp.AST.Type pointerType)
        {
            if (string.IsNullOrEmpty(cppType.Pointer))
            {
                pointerType = null;
                return false;
            }

            // It is a pointer type
            pointerType = new PointerType();
            switch (cppType.Pointer)
            {
                case "*":
                    (pointerType as PointerType).Modifier = PointerType.TypeModifier.Pointer;
                    break;
                case "&":
                    (pointerType as PointerType).Modifier = PointerType.TypeModifier.LVReference;
                    break;
                case "&&":
                    (pointerType as PointerType).Modifier = PointerType.TypeModifier.RVReference;
                    break;
            }

            // Try to find pointee
            (pointerType as PointerType).QualifiedPointee = new QualifiedType()
            {
                Qualifiers = new TypeQualifiers(),
                Type = WalkType(cppType.TypeName)
            };

            return true;
        }

        private bool WalkArrayType(CppType cppType, out CppSharp.AST.Type arrayType)
        {
            if (!cppType.IsArray)
            {
                arrayType = null;
                return false;
            }

            // Create new array type
            arrayType = new ArrayType();

            // Set content
            (arrayType as ArrayType).SizeType = ArraySize.Constant;
            (arrayType as ArrayType).Size = int.Parse(cppType.ArrayDimension);
            (arrayType as ArrayType).QualifiedType = new QualifiedType()
            {
                Qualifiers = new TypeQualifiers(),
                Type = WalkType(cppType.TypeName)
            };

            return true;
        }

        #endregion
    }
}
