using NodePylonGen.Parser.Model;
using System;

namespace NodePylonGen.Generator.Model
{
    class DeclarationVisitor : CppIncludeVisitor, IDeclarationVisitor<bool>
    {
        public bool VisitDeclaration(Declaration declaration)
        {
            if (!VisitCppElement(declaration))
            {
                return false;
            }

            return true;
        }

        public bool VisitNamespace(Namespace declarationNamespace)
        {
            throw new NotImplementedException();
        }

        public bool VisitTranslationUnit(TranslationUnit unit)
        {
            throw new NotImplementedException();
        }
    }
}
