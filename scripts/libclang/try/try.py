#!/usr/bin/env python


def get_info(node, depth=0):
    if opts.maxDepth is not None and depth >= opts.maxDepth:
        children = None
    else:
        children = [get_info(c, depth+1)
                    for c in node.get_children()]
    return {'id': get_cursor_id(node),
            'kind': node.kind,
            'usr': node.get_usr(),
            'spelling': node.spelling,
            'location': node.location,
            'extent.start': node.extent.start,
            'extent.end': node.extent.end,
            'is_definition': node.is_definition(),
            'definition id': get_cursor_id(node.get_definition()),
            'children': children}


def main():

    import sys
    from pprint import pprint
    from clang import cindex

    global opts

    class Options:
        maxDepth = 10

    opts = Options()

    assert len(sys.argv[1]) != 0, 'invalid number arguments'

    cindex.Config.set_library_path("/usr/local/Cellar/llvm/7.0.0/lib")

    index = cindex.Index.create()
    tu = index.parse(sys.argv[1])
    assert tu, 'unable to load input'

    pprint(('includes', [i.include.name for i in tu.get_includes()]))
    pprint(('nodes', get_info(tu.cursor)))


if __name__ == '__main__':
    main()
