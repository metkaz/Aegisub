// Copyright (c) 2014, Thomas Goyne <plorkyeran@aegisub.org>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// Aegisub Project http://www.aegisub.org/

using Microsoft.Build.Evaluation;
using Microsoft.Build.Framework;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Xml;

namespace BuildTasks {
    static class Utils {
        static public Dictionary<string, string> ReadPropertyMap(IBuildEngine be) {
            var reader = XmlReader.Create(be.ProjectFileOfTaskNode);
            return new Project(reader).AllEvaluatedProperties
                .Where(x => !x.IsEnvironmentProperty)
                .Where(x => !x.IsGlobalProperty)
                .Where(x => !x.IsReservedProperty)
                .ToDictionary(x => x.Name, x => x.EvaluatedValue);
        }

        /// Convert an absolute windows path to an msys path
        static public string MungePath(string path) {
            var match = Regex.Match(path, @"([A-Za-z]):\\(.*)");
            if (!match.Success)
                return path;
            return string.Format("/{0}/{1}", match.Groups[1].Value, match.Groups[2].Value.Replace('\\', '/'));
        }
    }
}
