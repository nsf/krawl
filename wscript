import sys

top = '.'
out = 'build'

def options(opt):
	opt.load('compiler_cxx')
	opt.add_option(
		'--lenstr',
		action  = 'store_true',
		default = False,
		help    = 'Add LLVM RPATH to the executable'
	)

def configure(conf):
	conf.define('KRAWL_INSTALL_PREFIX', conf.env.PREFIX)

	if sys.platform == "darwin":
		conf.env.append_unique('LINKFLAGS_CLANG_PLUGIN', '-Wl,-undefined,dynamic_lookup')

	conf.load('compiler_cxx')
	conf.check_cfg(
		path         = 'llvm-config',
		args         = '--cxxflags --ldflags',
		package      = '',
		uselib_store = 'LLVM',
	)

	conf.write_config_header('config.hpp')
	conf.env.append_unique('INCLUDES', conf.bldnode.abspath())

def build(bld):
	bld.recurse('ctokrawl compiler')
