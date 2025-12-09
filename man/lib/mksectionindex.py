import os
import sys

if __name__ == "__main__":
    prefix = sys.argv[1]
    section = sys.argv[2]
    index_file = """<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8"/>
<meta name="viewport" content="width=device-width, initial-scale=1.0"/>
<style>
    table.head, table.foot { width: 100%; }
    td.head-rtitle, td.foot-os { text-align: right; }
    td.head-vol { text-align: center; }
    .Nd, .Bf, .Op { display: inline; }
    .Pa, .Ad { font-style: italic; }
    .Ms { font-weight: bold; }
    .Bl-diag > dt { font-weight: bold; }
    code.Nm, .Fl, .Cm, .Ic, code.In, .Fd, .Fn, .Cd { font-weight: bold;
    font-family: inherit; }
</style>"""

    index_file += f"<title>Section {section} index</title>"
    index_file += f"""
</head>
<body>
    <h1>Section {section} index</h1>
<pre>
"""

    for f in os.listdir():
        if f == 'INDEX':
            continue
        index_file += f"<a href=\"{prefix}{section}/{f}.html\">{f}</a>\n"

    index_file += "</pre></body></html>"
    print(index_file)
