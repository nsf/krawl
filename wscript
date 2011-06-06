top = '.'
out = 'build'

def options(opt):
	opt.load('compiler_cxx')

def configure(conf):
	conf.define('CRAWL_INSTALL_PREFIX', conf.env.PREFIX)

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
	bld.recurse('ctocrawl compiler')
