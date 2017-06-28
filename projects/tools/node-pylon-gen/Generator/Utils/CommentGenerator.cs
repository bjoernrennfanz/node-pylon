using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NodePylonGen.Generator.Utils
{
    /// <summary>
    /// Comment type.
    /// </summary>
    public enum CommentType
    {
        // Invalid comment.
        Invalid,
        // "// comment"
        BCPL,
        // "/* comment */"
        C,
        // "/// comment"
        BCPLSlash,
        // "//! comment"
        BCPLExcl,
        // "/** comment */"
        JavaDoc
    }

    public abstract class CommentGenerator
    {
        public static string GetMultiLinePrologue(CommentType kind)
        {
            switch (kind)
            {
                case CommentType.BCPL:
                case CommentType.BCPLExcl:
                    return "//";
                case CommentType.C:
                case CommentType.JavaDoc:
                    return " *";
                case CommentType.BCPLSlash:
                    return "///";
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static string GetLinePrologue(CommentType kind)
        {
            switch (kind)
            {
                case CommentType.BCPL:
                case CommentType.BCPLSlash:
                    return string.Empty;
                case CommentType.C:
                    return "/*";
                case CommentType.BCPLExcl:
                    return "//!";
                case CommentType.JavaDoc:
                    return "/**";
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static string GetLineEpilogue(CommentType kind)
        {
            switch (kind)
            {
                case CommentType.BCPL:
                case CommentType.BCPLSlash:
                case CommentType.BCPLExcl:
                    return string.Empty;
                case CommentType.C:
                case CommentType.JavaDoc:
                    return " */";
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }
    }
}

