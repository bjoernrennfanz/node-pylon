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
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NodePylonGen.Generator.Utils
{
    public class TextGenerator : ITextGenerator
    {
        public const uint DefaultIndent = 4;

        public StringBuilder StringBuilder;
        protected bool isStartOfLine;
        protected bool needsNewLine;
        protected Stack<uint> currentIndent;

        /// <summary>
        /// Construct a empty instance
        /// </summary>
        public TextGenerator()
        {
            StringBuilder = new StringBuilder();
            currentIndent = new Stack<uint>();
            isStartOfLine = false;
        }

        /// <summary>
        /// Copy constructor
        /// </summary>
        public TextGenerator(TextGenerator generator)
        {
            currentIndent = new Stack<uint>(generator.currentIndent);
            StringBuilder = new StringBuilder(generator);
            isStartOfLine = generator.isStartOfLine;
            needsNewLine = generator.needsNewLine;
        }

        /// <summary>
        /// Create a clone of this instance.
        /// </summary>
        public TextGenerator Clone()
        {
            return new TextGenerator(this);
        }

        /// <summary>
        /// Gets the intent count
        /// </summary>
        public uint Indent
        {
            get { return (uint)currentIndent.Sum(u => (int)u); }
        }

        /// <summary>
        /// Sets the new line needed flag
        /// </summary>
        public void NeedNewLine()
        {
            needsNewLine = true;
        }

        /// <summary>
        /// Inserts a new line
        /// </summary>
        public void NewLine()
        {
            StringBuilder.AppendLine(string.Empty);
            isStartOfLine = true;
        }

        /// <summary>
        /// Inserts a new line if it is needed
        /// </summary>
        public void NewLineIfNeeded()
        {
            if (!needsNewLine)
            {
                return;
            }

            NewLine();
            needsNewLine = false;
        }

        /// <summary>
        /// Decrease line intent
        /// </summary>
        public void PopIndent()
        {
            currentIndent.Pop();
        }

        /// <summary>
        /// Increase line intent
        /// </summary>
        public void PushIndent(uint indent = DefaultIndent)
        {
            currentIndent.Push(indent);
        }

        /// <summary>
        /// Resets new line flag
        /// </summary>
        public void ResetNewLine()
        {
            needsNewLine = false;
        }

        /// <summary>
        /// Write to current line
        /// </summary>
        public void Write(string msg, params object[] args)
        {
            if (string.IsNullOrEmpty(msg))
            {
                return;
            }

            if (args.Length > 0)
            {
                msg = string.Format(msg, args);
            }

            foreach (var line in msg.SplitAndKeep(Environment.NewLine))
            {
                if (isStartOfLine && !string.IsNullOrWhiteSpace(line))
                {
                    StringBuilder.Append(new string(' ', (int)currentIndent.Sum(u => u)));
                }

                if (line.Length > 0)
                {
                    isStartOfLine = line.EndsWith(Environment.NewLine);
                }
                    
                StringBuilder.Append(line);
            }
        }

        /// <summary>
        /// Insert close bracket
        /// </summary>
        public void WriteCloseBraceIndent()
        {
            PopIndent();
            WriteLine("}");
        }

        /// <summary>
        /// Write line
        /// </summary>
        public void WriteLine(string msg, params object[] args)
        {
            Write(msg, args);
            NewLine();
        }

        /// <summary>
        /// Write line with extra intent
        /// </summary>
        public void WriteLineIndent(string msg, params object[] args)
        {
            PushIndent();
            WriteLine(msg, args);
            PopIndent();
        }

        /// <summary>
        /// Insert open bracket
        /// </summary>
        public void WriteStartBraceIndent()
        {
            WriteLine("{");
            PushIndent();
        }

        /// <summary>
        /// Overriden version of <see cref="string.ToString"/>
        /// </summary>
        public override string ToString()
        {
            return StringBuilder.ToString();
        }

        /// <summary>
        /// Operator to convert <see cref="TextGenerator"/> into <see cref="string"/> 
        /// </summary>
        public static implicit operator string(TextGenerator textGenerator)
        {
            return textGenerator.ToString();
        }
    }
}
