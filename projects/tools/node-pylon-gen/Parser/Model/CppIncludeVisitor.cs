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

using NodePylonGen.Parser.Model;
using System.Collections.Generic;
using System;

namespace NodePylonGen.Generator.Model
{
    public interface ICppIncludeVisited
    {
        ISet<object> Visited { get; }
        bool AlreadyVisited(CppElement element);
    }

    public interface ICppIncludeVisitor<out T> : ICppTypeVisitor<T>, ICppBaseVisitor<T>
    {
        T VisitCppClass(CppClass cppClass);
        T VisitCppDefine(CppDefine cppDefine);
        T VisitCppInterface(CppInterface cppInterface);
        T VisitCppFunction(CppFunction cppFunction);
        T VisitCppStruct(CppStruct cppStruct);
        T VisitCppEnum(CppEnum cppEnum);
        T VisitCppConstant(CppConstant cppConstant);
    }

    public class CppIncludeVisitorOptions
    {
        public bool VisitClassBases = true;
        public bool VisitClassFields = true;
        public bool VisitClassMethods = true;
    }

    public abstract class CppIncludeVisitor : ICppIncludeVisitor<bool>, ICppIncludeVisited
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

        #region Include visitor implementation

        public virtual bool VisitCppClass(CppClass cppClass)
        {
            if (!VisitCppBase(cppClass))
            {
                return false;
            }

            /*
            cppClass.B
            if (VisitOptions.)
                    foreach (var baseClass in @class.Bases)
                        if (baseClass.IsClass)
                            baseClass.Class.Visit(this); cppClass.Constructors
                            */
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
            return false;   
        }

        public virtual bool VisitConstructors(CppConstructor cppConstructor)
        {
            throw new NotImplementedException();
        }

        public virtual bool VisitMethods(CppMethod cppMethod)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region Type visitor implementation

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

        #endregion
    }
}