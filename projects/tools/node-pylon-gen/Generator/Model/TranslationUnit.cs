using NodePylonGen.Parser.Model;

namespace NodePylonGen.Generator.Model
{
    public class TranslationUnit : Namespace
    {
        /// <summary>
        /// Copy constructor from <see cref="CppInclude"/> 
        /// </summary>
        public TranslationUnit(CppInclude include)
        {
            // CppElement members
            Name = include.Name;
            Id = include.Id;
            Description = include.Description;
            Remarks = include.Remarks;
            Parent = include.Parent;
            Items = include.Items;
        }

        private string fileName;
        private string fileNameWithoutExtension;

        /// <summary>
        /// Contains the name of the file.
        /// </summary>
        public string FileName
        {
            get
            {
                if (string.IsNullOrEmpty(fileName))
                {
                    fileName = System.IO.Path.GetFileName(FilePath);
                }

                return fileName;
            }
        }

        /// <summary>
        /// Contains the name of the module.
        /// </summary>
        public string FileNameWithoutExtension
        {
            get
            {
                if (string.IsNullOrEmpty(fileNameWithoutExtension))
                {
                    fileNameWithoutExtension = System.IO.Path.GetFileNameWithoutExtension(FileName);
                }

                return fileNameWithoutExtension;
            }
        }

        /// <summary>
        /// Contains the include path.
        /// </summary>
        public string IncludePath { get; internal set; }

        /// <summary>
        /// Contains the file path.
        /// </summary>
        public string FilePath { get; internal set; }
    }
}