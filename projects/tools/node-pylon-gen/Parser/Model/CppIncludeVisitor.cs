// MIT License
//
// Copyright (c) 2017 Björn Rennfanz <bjoern@fam-rennfanz.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

using System.Collections.Generic;

namespace NodePylonGen.Parser.Model
{
    public interface ICppIncludeVisited
    {
        ISet<object> Visited { get; }
        bool AlreadyVisited(CppElement element);
    }

    public abstract class CppIncludeVisitor : ICppElementVisitor<bool>, ICppIncludeVisited
    {
        public ISet<object> Visited { get; private set; }

        public CppIncludeVisitor()
        {
            Visited = new HashSet<object>();
        }

        #region Already visited implementation

        public bool AlreadyVisited(CppElement element)
        {
            return !Visited.Add(element);
        }

        #endregion

        #region CppElement visitor implementation

        public virtual bool VisitCppClass(CppClass cppClass)
        {
            if (!VisitCppBase(cppClass))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppDefine(CppDefine cppDefine)
        {
            if (!VisitCppElement(cppDefine))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppInterface(CppInterface cppInterface)
        {
            if (!VisitCppBase(cppInterface))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppFunction(CppFunction cppFunction)
        {
            if (!VisitCppMethod(cppFunction))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppStruct(CppStruct cppStruct)
        {
            if (!VisitCppElement(cppStruct))
            {
                return false;
            }

            foreach (CppField field in cppStruct.Fields)
            {
                field.Visit(this);
            }

            return true;
        }

        public virtual bool VisitCppEnum(CppEnum cppEnum)
        {
            if (!VisitCppElement(cppEnum))
            {
                return false;
            }

            foreach (CppEnumItem enumItem in cppEnum.EnumItems)
            {
                enumItem.Visit(this);
            }

            return true;
        }

        public virtual bool VisitCppConstant(CppConstant cppConstant)
        {
            if (!VisitCppElement(cppConstant))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppBase(CppBase cppBase)
        {
            if (!VisitCppElement(cppBase))
            {
                return false;
            }

            foreach(CppConstructor constructor in cppBase.Constructors)
            {
                constructor.Visit(this);
            }

            foreach (CppMethod method in cppBase.Methods)
            {
                method.Visit(this);
            }

            return true;
        }

        public virtual bool VisitCppConstructor(CppConstructor cppConstructor)
        {
            if (!VisitCppElement(cppConstructor))
            {
                return false;
            }

            foreach(CppParameter parameter in cppConstructor.Parameters)
            {
                parameter.Visit(this);
            }

            return true;
        }

        public virtual bool VisitCppType(CppType cppType)
        {
            if (!VisitCppElement(cppType))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppField(CppField cppField)
        {
            if (!VisitCppType(cppField))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppParameter(CppParameter cppParameter)
        {
            if (!VisitCppType(cppParameter))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppMethod(CppMethod cppMethod)
        {
            if (!VisitCppElement(cppMethod))
            {
                return false;
            }

            foreach (CppParameter parameter in cppMethod.Parameters)
            {
                parameter.Visit(this);
            }

            cppMethod.ReturnType.Visit(this);

            return true;
        }

        public virtual bool VisitCppElement(CppElement cppElement)
        {
            return !AlreadyVisited(cppElement);
        }

        public virtual bool VisitCppEnumItem(CppEnumItem cppEnumItem)
        {
            if (!VisitCppElement(cppEnumItem))
            {
                return false;
            }

            return true;
        }

        public virtual bool VisitCppInclude(CppInclude cppInclude)
        {
            if (!VisitCppElement(cppInclude))
            {
                return false;
            }

            foreach (CppClass cppClass in cppInclude.Classes)
            {
                cppClass.Visit(this);
            }

            foreach (CppDefine cppDefine in cppInclude.Macros)
            {
                cppDefine.Visit(this);
            }

            foreach (CppInterface cppInterface in cppInclude.Interfaces)
            {
                cppInterface.Visit(this);
            }

            foreach (CppFunction cppFunction in cppInclude.Functions)
            {
                cppFunction.Visit(this);
            }

            foreach (CppStruct cppStruct in cppInclude.Structs)
            {
                cppStruct.Visit(this);
            }

            foreach (CppEnum cppEnum in cppInclude.Enums)
            {
                cppEnum.Visit(this);
            }

            foreach (CppConstant cppConstant in cppInclude.Constants)
            {
                cppConstant.Visit(this);
            }

            return true;
        }

        public virtual bool VisitCppModule(CppModule cppModule)
        {
            if (!VisitCppElement(cppModule))
            {
                return false;
            }

            foreach (CppInclude cppInclude in cppModule.Includes)
            {
                cppInclude.Visit(this);
            }

            return true;
        }

        #endregion
    }
}