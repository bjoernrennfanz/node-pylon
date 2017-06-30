using System;
using NodePylonGen.Parser.Model;

namespace NodePylonGen.Generator.Model
{
    /// <summary>
    /// Represents a declaration context.
    /// </summary>
    public abstract class DeclarationContext : Declaration
    {
        public bool IsAnonymous { get; set; }





    }

    public class Namespace : DeclarationContext
    {
        





        public override T Visit<T>(IDeclarationVisitor<T> visitor)
        {
            return visitor.VisitNamespace(this);
        }
    }
}