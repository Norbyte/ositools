import pathlib, shutil, re

def doxy_should_ignore(path, files):
    ignored = []
    for file in files:
        if file == 'x64' or file == 'Doxygen':
            ignored.append(file)
    return ignored

def make_doxy_root(src, dst):
    shutil.rmtree(dst, ignore_errors=True)
    shutil.copytree(src, dst, ignore=doxy_should_ignore)

def rewrite_includes_in_file(root: pathlib.Path, depth: int, path: pathlib.Path):
    def include_replacement(matches: re.Match):
        path = root.joinpath(matches.group(1))
        if path.exists():
            return '#include "' + ('../' * depth) + matches.group(1) + '"'
        else:
            return matches.group(0)

    with open(path, 'r') as f:
        body = f.read()
    body = re.sub('#include <(.*)>', include_replacement, body)
    with open(path, 'w') as f:
        f.write(body)

def rewrite_includes(root: pathlib.Path, depth: int, dir: pathlib.Path):
    for path in dir.iterdir():
        if path.is_file():
            ext = path.suffix
            if ext == '.c' or ext == '.h' or ext == '.cpp' or ext == '.hpp' or ext == '.inl':
                rewrite_includes_in_file(root, depth, path)
        if path.is_dir():
            rewrite_includes(root, depth+1, path)

src = pathlib.Path(__file__).parent.resolve().joinpath('ScriptExtender')
dst_root = pathlib.Path(__file__).parent.resolve().joinpath('ScriptExtenderTypeGen')
make_doxy_root(src, dst_root)
rewrite_includes(dst_root, 0, dst_root)
