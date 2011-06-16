from waflib import Utils
from waflib.Task import Task
from waflib.TaskGen import feature, before, extension, after_method
from waflib.Tools import ccroot
import krawl_scan

def to_dict(list_or_dict):
	if isinstance(list_or_dict, list):
		d = {}
		for i in list_or_dict:
			d[i] = True
		return d
	return list_or_dict

def configure(conf):
	opts = to_dict(getattr(conf.env, 'KRAWL_CONF_OPTS', {}))
	if 'KRAWL_CONF_OPTS' in conf.env:
		del conf.env.KRAWL_CONF_OPTS

	if 'bootstrap' not in opts:
		conf.fatal("looking for a compiler is not implemented")

	conf.env.KRAWL_UID_ST = '-u%s'
	conf.env.KRAWL_HASH_UID_ST = '-U%s'
	conf.env.KRAWL_PKG_ST = '-P%s'
	conf.env.KRAWL_CLANG_ST = '--clang=%s'
	conf.env.KRAWL_CLANG_PLUGIN_ST = '--clang-plugin=%s'
	conf.env.KRAWL_BRL_OUT_ST = '-b%s'
	conf.env.KRAWL_I_ST = '-I%s'
	if 'bootstrap' in opts:
		conf.env.KRAWL_BOOTSTRAP = True

class krawl(Task):
	run_str = """
	${KRAWL}
	${KRAWL_UID_ST:UID}
	${KRAWL_HASH_UID_ST:HASH_UID}
	${KRAWL_PKG_ST:PKG}
	${KRAWL_CLANG_ST:CLANG}
	${KRAWL_CLANG_PLUGIN_ST:CLANG_PLUGIN}
	${KRAWL_BRL_OUT_ST:BRL_OUT}
	${KRAWL_I_ST:KRAWL_INCPATHS}
	-o ${TGT[0].bldpath()} ${SRC}
	""".replace("\n", "")

	color = 'GREEN'
	scan  = krawl_scan.scan

@feature('krawl')
@after_method('propagate_uselib_vars', 'process_source')
def apply_krawl_incpaths(self):
	lst = self.to_incnodes(self.to_list(getattr(self, 'krawl_includes', [])) + self.env['KRAWL_INCLUDES'])
	self.includes_nodes = lst
	self.env['KRAWL_INCPATHS'] = [x.abspath() for x in lst]

#==============================================================================
# Params:
# uid          - unique id
# hash_uid     - hashed unique id
# package      - package name
# install_path - installation path
# target_brl   - override default location for interface file
# includes     - dirs to use as includes
#==============================================================================
@feature('krawl')
@before('process_source')
def apply_krawl(self):
	"""Create krawl task"""
	# extract .krl nodes from 'source'
	src_nodes = []
	no_nodes = []
	for n in self.to_nodes(self.source):
		if n.name.endswith('.krl'):
			src_nodes.append(n)
		else:
			no_nodes.append(n)
	self.source = no_nodes

	# if there were no nodes, return
	if len(src_nodes) == 0:
		return

	# object file
	obj_node = src_nodes[0].change_ext('.o')

	# brawl interface file
	target_brl = getattr(self, 'target_brl', None)
	if target_brl:
		brl_node = self.path.find_or_declare(target_brl)
	else:
		brl_node = src_nodes[0].change_ext('.brl')

	# create task
	task = self.create_task('krawl', src_nodes, [obj_node, brl_node])
	if target_brl:
		task.env.BRL_OUT = [brl_node.bldpath()]

	# uid
	uid = getattr(self, 'uid', None)
	if uid:
		task.env.UID = [uid]

	# hash uid
	hash_uid = getattr(self, 'hash_uid', None)
	if hash_uid:
		task.env.HASH_UID = [hash_uid]

	# package
	pkg = getattr(self, 'package', None)
	if pkg:
		task.env.PKG = [pkg]

	# get path to krawl executable and clang plugin
	if self.env.KRAWL_BOOTSTRAP:
		tg = self.bld.get_tgen_by_name('krawl')
		krawlc = tg.link_task.outputs[0]
		task.env.KRAWL = krawlc.bldpath()
		task.dep_nodes.append(krawlc)

		tg = self.bld.get_tgen_by_name('ctokrawl')
		clang_plugin = tg.link_task.outputs[0]
		task.env.CLANG_PLUGIN = [clang_plugin.bldpath()]
		task.dep_nodes.append(clang_plugin)

	try:
		self.compiled_tasks.append(task)
	except AttributeError:
		self.compiled_tasks = [task]

	#------------------------------------------------------
	# installation
	#------------------------------------------------------

	if getattr(self.bld, 'is_install', None) and (uid or hash_uid):
		pkg = getattr(self, 'package', src_nodes[0].parent.name)
		inst_to = getattr(self, 'install_path', None)

		if inst_to:
			self.header_install_task = self.bld.install_as(
				dest    = inst_to + '/' + pkg + '.brl',
				srcfile = brl_node,
				env     = self.env,
				chmod   = Utils.O644
			)


