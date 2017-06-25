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

namespace NodePylonGen.Parser.Model
{
    public interface ICppElementVisitor<out T>
    {
        // Class & Interface
        T VisitCppBase(CppBase cppBase);
        T VisitCppConstructor(CppConstructor cppConstructor);
        T VisitCppMethod(CppMethod cppMethod);

        // All other elements
        T VisitCppElement(CppElement cppElement);
        T VisitCppClass(CppClass cppClass);
        T VisitCppDefine(CppDefine cppDefine);
        T VisitCppInterface(CppInterface cppInterface);
        T VisitCppFunction(CppFunction cppFunction);
        T VisitCppStruct(CppStruct cppStruct);
        T VisitCppEnum(CppEnum cppEnum);
        T VisitCppEnumItem(CppEnumItem cppEnumItem);
        T VisitCppConstant(CppConstant cppConstant);

        // Module & Include
        T VisitCppInclude(CppInclude cppInclude);
        T VisitCppModule(CppModule cppModule);

        // Types
        T VisitCppType(CppType cppType);
        T VisitCppField(CppField cppField);
        T VisitCppParameter(CppParameter cppParameter);
    }
}
