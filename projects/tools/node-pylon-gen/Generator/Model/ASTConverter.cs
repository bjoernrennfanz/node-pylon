using CppSharp;
using NodePylonGen.Config;
using NodePylonGen.Parser.Model;
using System;

using ParserAST = CppSharp.Parser.AST;
using ModelAST = CppSharp.AST;

namespace NodePylonGen.Generator.Model
{
    public class ASTConverter : CppIncludeVisitor
    {
        private ParserAST.ASTContext currentASTContext;
        private ConfigMapping configurationContext;
        private CppModule moduleContext;

        public ASTConverter(ConfigMapping configurationContext, CppModule moduleContext)
        {
            this.configurationContext = configurationContext;
            this.moduleContext = moduleContext;
        }

        public ModelAST.ASTContext Convert()
        {
            currentASTContext = new ParserAST.ASTContext();
            CppSharp.ASTConverter parserToModelAST = new CppSharp.ASTConverter(currentASTContext);

            moduleContext.Visit(this);

            return parserToModelAST.Convert();
        }

        public override bool VisitCppClass(CppClass cppClass)
        {
            return base.VisitCppClass(cppClass);
        }

        public override bool VisitCppDefine(CppDefine cppDefine)
        {
            return base.VisitCppDefine(cppDefine);
        }

        public override bool VisitCppInterface(CppInterface cppInterface)
        {
            return base.VisitCppInterface(cppInterface);
        }

        public override bool VisitCppFunction(CppFunction cppFunction)
        {
            return base.VisitCppFunction(cppFunction);
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

        public override bool VisitCppBase(CppBase cppBase)
        {
            return base.VisitCppBase(cppBase);
        }

        public override bool VisitCppConstructor(CppConstructor cppConstructor)
        {
            Method method = new Method();

            method.Access = AccessSpecifier.Public;
            
            return base.VisitCppConstructor(cppConstructor);
        }

        internal CppSharp.AST.ASTContext Convert(CppModule moduleContext)
        {
            throw new NotImplementedException();
        }

        public override bool VisitCppType(CppType cppType)
        {
            throw new NotImplementedException();
        }

        public override bool VisitCppField(CppField cppField)
        {
            throw new NotImplementedException();
        }

        public override bool VisitCppParameter(CppParameter cppParameter)
        {
            throw new NotImplementedException();
        }

        public override bool VisitCppMethod(CppMethod cppMethod)
        {
            throw new NotImplementedException();
        }

        public override bool VisitCppElement(CppElement cppElement)
        {
            throw new NotImplementedException();
        }

        public override bool VisitCppEnumItem(CppEnumItem cppEnumItem)
        {
            throw new NotImplementedException();
        }

        public override bool VisitCppInclude(CppInclude cppInclude)
        {
            throw new NotImplementedException();
        }

        public override bool VisitCppModule(CppModule cppModule)
        {
            throw new NotImplementedException();
        }
    }
}
