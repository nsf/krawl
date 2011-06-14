import sys

top = '.'
out = 'build'

def options(opt):
	opt.load('compiler_cxx')
	opt.load('compiler_c')
	opt.add_option(
		'--lenstr',
		action  = 'store_true',
		default = False,
		help    = 'Add LLVM RPATH to the executable'
	)
	opt.add_option(
		'--stdlib',
		action  = 'store_true',
		default = False,
		help    = 'Build Krawl standard library as well'
	)

def configure(conf):
	conf.define('KRAWL_INSTALL_PREFIX', conf.env.PREFIX)

	conf.env.OPT_LENSTR = conf.options.lenstr
	conf.env.OPT_STDLIB = conf.options.stdlib

	if sys.platform == "darwin":
		conf.env.append_unique('LINKFLAGS_CLANG_PLUGIN', '-Wl,-undefined,dynamic_lookup')

	conf.env.KRAWL_CONF_OPTS = ['bootstrap']
	conf.load('krawl', tooldir='wafscripts')

	conf.load('compiler_cxx')
	conf.load('compiler_c')
	conf.check_cfg(
		path         = 'llvm-config',
		args         = '--cxxflags --ldflags --libs',
		package      = '',
		uselib_store = 'LLVM',
	)

	conf.write_config_header('config.hpp')
	conf.env.append_unique('INCLUDES', conf.bldnode.abspath())

def build(bld):
	bld.recurse('ctokrawl compiler')
	if bld.env.OPT_STDLIB:
		bld.add_group()
		bld.recurse('stdlib')
