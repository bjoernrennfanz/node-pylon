using System;
using System.Collections.Generic;
using NodePylonGen.Parser.Model;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    /// <summary>
    /// Generates C++/NodeJS wrapper header files.
    /// </summary>
    internal class NodeJSHeaders : NodeJSTemplate
    {
        public NodeJSHeaders(BindingContext context, IEnumerable<CppInclude> units)
            : base(context, units)
        {
        }

        public override string FileExtension => "h";

        public override void Process()
        {
            //throw new NotImplementedException();
        }
    }
}