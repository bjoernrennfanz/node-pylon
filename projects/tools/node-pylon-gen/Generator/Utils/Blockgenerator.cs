using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NodePylonGen.Generator.Utils
{
    /// <summary>
    /// Controls the new line generation behavior
    /// </summary>
    public enum NewLineType
    {
        Never,
        Always,
        BeforeNextBlock,
        IfNotEmpty
    }

    /// <summary>
    /// Enum for block types
    /// </summary>
    public enum BlockType
    {
        Unknown
    }

    /// <summary>
    /// Block implementation
    /// </summary>
    public class Block : ITextGenerator
    {
        public TextGenerator Text { get; set; }
        public BlockType Type { get; set; }
        public NewLineType NewLineType { get; set; }

        public object Object { get; set; }

        public Block Parent { get; set; }
        public List<Block> Blocks { get; set; }

        private bool hasIndentChanged;
        private bool isSubBlock;

        public Func<bool> CheckGenerate;

        /// <summary>
        /// Construct a empty <see cref="Block"/> instance.
        /// </summary>
        public Block()
            : this(BlockType.Unknown)
        {
        }

        /// <summary>
        /// Construct a <see cref="Block"/> instance with given type.
        /// </summary>
        public Block(BlockType type)
        {
            Type = type;
            Blocks = new List<Block>();
            Text = new TextGenerator();
            hasIndentChanged = false;
            isSubBlock = false;
        }

        /// <summary>
        /// Add a block to instance.
        /// </summary>
        public void AddBlock(Block block)
        {
            if (Text.StringBuilder.Length != 0 || hasIndentChanged)
            {
                hasIndentChanged = false;
                Block newBlock = new Block()
                {
                    Text = Text.Clone(),
                    isSubBlock = true
                };

                Text.StringBuilder.Clear();
                AddBlock(newBlock);
            }

            block.Parent = this;
            Blocks.Add(block);
        }

        /// <summary>
        /// Find all blocks with given type
        /// </summary>
        public IEnumerable<Block> FindBlocks(BlockType type)
        {
            foreach (var block in Blocks)
            {
                if (block.Type == type)
                {
                    yield return block;
                }

                foreach (Block childBlock in block.FindBlocks(type))
                {
                    yield return childBlock;
                }
            }
        }

        /// <summary>
        /// Generate block with intends
        /// </summary>
        public virtual string Generate(bool withIntends)
        {
            if (CheckGenerate != null && !CheckGenerate())
            {
                return "";
            }

            if (Blocks.Count == 0)
            {
                return Text.ToString();
            }

            StringBuilder builder = new StringBuilder();
            Block previousBlock = null;
            uint totalIndent = 0;
            int blockIndex = 0;

            foreach (Block childBlock in Blocks)
            {
                string childText = childBlock.Generate(withIntends);
                bool skipBlock = false;

                Block nextBlock = (++blockIndex < Blocks.Count) ? Blocks[blockIndex] : null;
                if (nextBlock != null)
                {
                    string nextText = nextBlock.Generate(withIntends);
                    if (string.IsNullOrEmpty(nextText) && childBlock.NewLineType == NewLineType.IfNotEmpty)
                    {
                        skipBlock = true;
                    }
                }

                if (skipBlock)
                {
                    continue;
                }

                if (string.IsNullOrEmpty(childText))
                {
                    continue;
                }

                if (previousBlock != null && previousBlock.NewLineType == NewLineType.BeforeNextBlock)
                {
                    builder.AppendLine();
                }

                if (withIntends)
                {
                    IEnumerable<string> lines = childText.SplitAndKeep(Environment.NewLine).ToList();

                    if (childBlock.isSubBlock)
                    {
                        totalIndent = 0;
                    }

                    foreach (string line in lines)
                    {
                        if (string.IsNullOrEmpty(line))
                        {
                            continue;
                        }

                        if (!string.IsNullOrWhiteSpace(line))
                        {
                            builder.Append(new string(' ', (int)totalIndent));
                        }

                        builder.Append(line);

                        if (!line.EndsWith(Environment.NewLine, StringComparison.Ordinal))
                        {
                            builder.AppendLine();
                        }
                    }
                }
                else
                {
                    builder.Append(childText);
                }

                if (childBlock.NewLineType == NewLineType.Always)
                {
                    builder.AppendLine();
                }

                totalIndent += childBlock.Text.Indent;
                previousBlock = childBlock;
            }

            if (Text.StringBuilder.Length != 0)
            {
                builder.Append(Text.StringBuilder);
            }

            return builder.ToString();
        }

        /// <summary>
        /// Indicates that the block is empty
        /// </summary>
        public bool IsEmpty
        {
            get
            {
                if (Blocks.Any(block => !block.IsEmpty))
                {
                    return false;
                }

                return string.IsNullOrEmpty(Text.ToString());
            }
        }

        #region ITextGenerator implementation

        public uint Indent
        {
            get { return Text.Indent; }
        }

        public void NeedNewLine()
        {
            Text.NeedNewLine();
        }

        public void NewLine()
        {
            Text.NewLine();
        }

        public void NewLineIfNeeded()
        {
            Text.NewLineIfNeeded();
        }

        public void PopIndent()
        {
            hasIndentChanged = true;
            Text.PopIndent();
        }

        public void PushIndent(uint indent = 4)
        {
            hasIndentChanged = true;
            Text.PushIndent(indent);
        }

        public void ResetNewLine()
        {
            Text.ResetNewLine();
        }

        public void Write(string msg, params object[] args)
        {
            Text.Write(msg, args);
        }

        public void WriteCloseBraceIndent()
        {
            Text.WriteCloseBraceIndent();
        }

        public void WriteLine(string msg, params object[] args)
        {
            Text.WriteLine(msg, args);
        }

        public void WriteLineIndent(string msg, params object[] args)
        {
            Text.WriteLineIndent(msg, args);
        }

        public void WriteStartBraceIndent()
        {
            Text.WriteStartBraceIndent();
        }

        #endregion
    }

    /// <summary>
    /// Blockgenerator implementation
    /// </summary>
    public abstract class BlockGenerator : ITextGenerator
    {
        public Block RootBlock { get; private set; }
        public Block ActiveBlock { get; private set; }

        protected BlockGenerator()
        {
            RootBlock = new Block();
            ActiveBlock = RootBlock;
        }

        public virtual string Generate()
        {
            return RootBlock.Generate(true);
        }

        public string GenerateUnformatted()
        {
            return RootBlock.Generate(false);
        }

        #region Block helpers

        public void AddBlock(Block block)
        {
            ActiveBlock.AddBlock(block);
        }

        public void PushBlock(BlockType type = BlockType.Unknown, object obj = null)
        {
            Block block = new Block { Type = type, Object = obj };
            PushBlock(block);
        }

        public void PushBlock(Block block)
        {
            block.Parent = ActiveBlock;
            ActiveBlock.AddBlock(block);
            ActiveBlock = block;
        }

        public Block PopBlock(NewLineType newLineType = NewLineType.Never)
        {
            Block block = ActiveBlock;

            ActiveBlock.NewLineType = newLineType;
            ActiveBlock = ActiveBlock.Parent;

            return block;
        }

        public IEnumerable<Block> FindBlocks(BlockType type)
        {
            return RootBlock.FindBlocks(type);
        }

        public Block FindBlock(BlockType type)
        {
            return FindBlocks(type).SingleOrDefault();
        }

        #endregion

        #region ITextGenerator implementation

        public uint Indent { get { return ActiveBlock.Indent; } }

        public void Write(string msg, params object[] args)
        {
            ActiveBlock.Write(msg, args);
        }

        public void WriteLine(string msg, params object[] args)
        {
            ActiveBlock.WriteLine(msg, args);
        }

        public void WriteLineIndent(string msg, params object[] args)
        {
            ActiveBlock.WriteLineIndent(msg, args);
        }

        public void NewLine()
        {
            ActiveBlock.NewLine();
        }

        public void NewLineIfNeeded()
        {
            ActiveBlock.NewLineIfNeeded();
        }

        public void NeedNewLine()
        {
            ActiveBlock.NeedNewLine();
        }

        public void ResetNewLine()
        {
            ActiveBlock.ResetNewLine();
        }

        public void PushIndent(uint indent = 4u)
        {
            ActiveBlock.PushIndent(indent);
        }

        public void PopIndent()
        {
            ActiveBlock.PopIndent();
        }

        public void WriteStartBraceIndent()
        {
            ActiveBlock.WriteStartBraceIndent();
        }

        public void WriteCloseBraceIndent()
        {
            ActiveBlock.WriteCloseBraceIndent();
        }

        #endregion
    }
}
