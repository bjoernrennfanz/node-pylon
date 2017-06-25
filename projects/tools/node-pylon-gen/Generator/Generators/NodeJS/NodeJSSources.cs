using System;
using System.Collections.Generic;
using NodePylonGen.Parser.Model;

namespace NodePylonGen.Generator.Generators.NodeJS
{
    /// <summary>
    /// Generates C++/NodeJS wrapper source files.
    /// </summary>
    public class NodeJSSources : NodeJSTemplate
    {
        public NodeJSSources(BindingContext context, IEnumerable<CppInclude> units)
            : base(context, units)
        {
        }

        public override string FileExtension => "cc";

        public override void Process()
        {
            string test = FilePath;

            //throw new NotImplementedException();
        }
    }
}