import sys
import re
import argparse

path_prefix = ""

def repl_normal(m):
    if m.group('name').lower() == 'section':
        return f"<a href=\"{path_prefix}{m.group('section')}\">{m.group('name')} ({m.group('section')})</a>"
    else:
        if m.group('section') not in ['lib', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10']:
            pass
        else:
            return f"<a href=\"{path_prefix}{m.group('section')}/{m.group('name')}.html\">{m.group('name')} ({m.group('section')})</a>"            
    return f"{m.group('name')} ({m.group('section')})"

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='mklinks',
                    description='Makes links obviously',
                    epilog='God bless you')
    parser.add_argument('-i', '--input')
    parser.add_argument('-o', '--output')
    parser.add_argument('-p', '--path-prefix')
    args = parser.parse_args()

    path_prefix = ""
    if args.path_prefix is not None:
        path_prefix = args.path_prefix

    link_re = re.compile(r"(?P<name>[a-zA-Z0-9\-\_]+) \((?P<section>[0-9a-z]+)\)")
    link_i_re = re.compile(r"<i>(?P<name>[a-zA-Z0-9\-\_]+)</i>\((?P<section>[0-9a-z]+)\)")
    f = None
    if args.input is None:
        f = sys.stdin
    else:
        f = open(args.input, 'r')

    text = f.read()
    f.close()

    text = link_re.sub(repl_normal, text)
    text = link_i_re.sub(repl_normal, text)
    if args.output is not None:
        with open(args.output, 'w') as out:
            out.write(text)
    else:
        print(text)
