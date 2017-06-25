using System;
using System.Collections.Generic;
using NodePylonGen.Parser.Model;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    public abstract class NodeJSTemplate : CodeGenerator
    {
        protected NodeJSTemplate(BindingContext context, IEnumerable<CppInclude> units)
            : base(context, units)
        {
        }

        public abstract override string FileExtension { get; }

        public abstract override void Process();
    }
}