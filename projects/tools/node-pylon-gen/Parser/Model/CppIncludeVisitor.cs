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
using System;

namespace NodePylonGen.Parser.Model
{
    public interface ICppIncludeVisited
    {
        ISet<object> Visited { get; }
        bool AlreadyVisited(CppElement element);
    }

    public class CppIncludeVisitorOptions
    {
        public bool VisitClassBases = true;
        public bool VisitClassFields = true;
        public bool VisitClassMethods = true;
    }

    public abstract class CppIncludeVisitor : ICppElementVisitor<bool>, ICppIncludeVisited
    {
        public ISet<object> Visited { get; private set; }
        public CppIncludeVisitorOptions VisitOptions { get; private set; }

        public CppIncludeVisitor()
        {
            Visited = new HashSet<object>();
            VisitOptions = new CppIncludeVisitorOptions();
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
            throw new NotImplementedException();
        }

        public virtual bool VisitCppInterface(CppInterface cppInterface)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppFunction(CppFunction cppFunction)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppStruct(CppStruct cppStruct)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppEnum(CppEnum cppEnum)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppConstant(CppConstant cppConstant)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppBase(CppBase cppBase)
        {
            if (!AlreadyVisited(cppBase))
            {
                return false;
            }

            return VisitCppElement(cppBase);
        }

        public virtual bool VisitConstructors(CppConstructor cppConstructor)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitMethods(CppMethod cppMethod)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppType(CppType cppType)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppField(CppField cppField)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppParameter(CppParameter cppParameter)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitConstructor(CppConstructor cppConstructor)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitMethod(CppMethod cppMethod)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppElement(CppElement cppElement)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppEnumItem(CppEnumItem cppEnumItem)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppInclude(CppInclude cppInclude)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitCppModule(CppModule cppModule)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}