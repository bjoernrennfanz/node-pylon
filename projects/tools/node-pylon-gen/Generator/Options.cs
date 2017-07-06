namespace NodePylonGen.Generator
{
    public class DriverOptions : CppSharp.DriverOptions
    {
        /// <summary>
        /// Overloaded version of generator type
        /// </summary>
        public new GeneratorType GeneratorKind;

        /// <summary>
        /// Indicates that generator is for Node JS
        /// </summary>
        public bool IsNodeJSGenerator => GeneratorKind == GeneratorType.NodeJS;

        public bool IsJavaGenerator => GeneratorKind == GeneratorType.Java;
    }
}
