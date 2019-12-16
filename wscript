#!/usr/bin/env python
import os
from os.path import join
from waflib.extras.test_base import summary
from waflib.extras.symwaf2ic import get_toplevel_path

def depends(dep):
    dep('code-format')
    dep('lib-rcf')
    dep.recurse('pyflange')


def options(opt):
    opt.load('compiler_c')
    opt.load('compiler_cxx')
    opt.load("doxygen")
    opt.recurse('pyflange')


def configure(conf):
    conf.load('compiler_c')
    conf.load('compiler_cxx')
    conf.load("doxygen")
    conf.recurse('pyflange')


def build(bld):
    bld(target          = 'flange_inc',
        export_includes = 'include'
    )

    bld.shlib(
        target       = 'flange',
        features     = 'cxx',
        source       = bld.path.ant_glob('src/flange/*.cpp'),
        use          = ['flange_inc', 'rcf-sf-only'],
        install_path = '${PREFIX}/lib',
    )

    bld(
        target       = 'flange_swtests',
        features     = 'gtest cxx cxxprogram',
        source       = bld.path.ant_glob('tests/sw/flange/test-*.cpp'),
        use          = ['flange'],
    )

    bld.recurse('pyflange')

    bld(
        features = 'doxygen',
        name = 'flange_documentation',
        doxyfile = bld.root.make_node(join(get_toplevel_path(), "code-format", "doxyfile")),
        install_path = 'doc/flange',
        pars = {
            "PROJECT_NAME": "\"Flange\"",
            "INPUT": join(get_toplevel_path(), "flange", "include"),
            "PREDEFINED": "GENPYBIND()= GENPYBIND_TAG_FLANGE=",
            "OUTPUT_DIRECTORY": join(get_toplevel_path(), "build", "flange", "doc")
        },
    )

    # Create test summary (to stdout and XML file)
    bld.add_post_fun(summary)
