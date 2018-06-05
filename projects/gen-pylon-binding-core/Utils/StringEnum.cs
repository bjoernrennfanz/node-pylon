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

using System;
using System.Reflection;

namespace GenPylonBinding.Core.Utils
{
    public class StringEnumValue : System.Attribute 
    {
        private readonly string stringValue;

        public StringEnumValue(string value)
        {
            stringValue = value;
        }

        public string Value
        {
            get { return stringValue; }
        }
    }

    public static class StringEnum
    {
        public static string GetStringValue(Enum value)
        {
            string result = null;
            Type enumType = value.GetType();

            FieldInfo enumFieldInfo = enumType.GetField(value.ToString());
            StringEnumValue[] enumAttrs = enumFieldInfo.GetCustomAttributes(typeof(StringEnumValue), false) as StringEnumValue[];

            if (enumAttrs.Length > 0)
            {
                result = enumAttrs[0].Value;
            }

            return result;
        }
    }
}
