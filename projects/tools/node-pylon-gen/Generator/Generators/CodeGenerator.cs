using NodePylonGen.Config;
using NodePylonGen.Generator.Utils;
using NodePylonGen.Parser.Model;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace NodePylonGen.Generator.Generators
{
    public abstract class CodeGenerator : BlockGenerator, ICppElementVisitor<bool>
    {
        public BindingContext Context { get; }
        public List<CppInclude> Units { get; }

        public CppInclude Unit => Units[0];

        public abstract string FileExtension { get; }
        public virtual string FilePath
        {
            get
            {
                IEnumerable<ConfigMapping> configFilesLoaded = Context.ConfigContext.ConfigFilesLoaded;
                IEnumerable < ConfigMapping> configOfUnit = configFilesLoaded.Where(config => config.Includes.All(include => include.Id == Unit.Name));

                return ((configOfUnit != null) ? Path.Combine(Path.GetDirectoryName(configOfUnit.AbsoluteFilePath), Unit.Name) : Unit.Name) + "." + FileExtension;
            }
        }

        protected CodeGenerator(BindingContext context, CppInclude unit)
            : this(context, new List<CppInclude> { unit })
        {
        }

        protected CodeGenerator(BindingContext context, IEnumerable<CppInclude> units)
        {
            Context = context;
            Units = new List<CppInclude>(units);
        }

        public abstract void Process();


        #region ICppElementVisitor

        public bool VisitCppBase(CppBase cppBase)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppClass(CppClass cppClass)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppConstant(CppConstant cppConstant)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppConstructor(CppConstructor cppConstructor)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppDefine(CppDefine cppDefine)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppElement(CppElement cppElement)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppEnum(CppEnum cppEnum)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppEnumItem(CppEnumItem cppEnumItem)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppField(CppField cppField)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppFunction(CppFunction cppFunction)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppInclude(CppInclude cppInclude)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppInterface(CppInterface cppInterface)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppMethod(CppMethod cppMethod)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppModule(CppModule cppModule)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppParameter(CppParameter cppParameter)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppStruct(CppStruct cppStruct)
        {
            throw new NotImplementedException();
        }

        public bool VisitCppType(CppType cppType)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}