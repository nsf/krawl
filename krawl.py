from waflib import Utils
from waflib.Task import Task
from waflib.TaskGen import feature, before, extension

def configure(conf):
	conf.env.KRAWL_UID_ST = '--uid=%s'
	conf.env.KRAWL_HUID_ST = '--hash-uid=%s'
	conf.env.KRAWL_PKG_ST = '-P%s'

class krawl(Task):
	run_str = '${KRAWL} ${KRAWL_UID_ST:UID} ${KRAWL_HUID_ST:HUID} ${KRAWL_PKG_ST:PKG} -o ${TGT[0].abspath()} ${SRC}'
	color   = 'GREEN'

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

	# create task
	out_nodes = [
		src_nodes[0].change_ext('.%d.o' % self.idx),
		src_nodes[0].change_ext('.%d.brl' % self.idx)
	]
	task = self.create_task('krawl', src_nodes, out_nodes)

	uid = getattr(self, 'uid', None)
	if uid:
		task.env.UID = [uid]
	huid = getattr(self, 'huid', None)
	if huid:
		task.env.HUID = [huid]

	pkg = getattr(self, 'package', None)
	if pkg:
		task.env.PKG = [pkg]

	# get path to krawl executable
	tg = self.bld.get_tgen_by_name('krawl')
	krawlc = tg.link_task.outputs[0]
	task.env.KRAWL = krawlc.abspath()

	try:
		self.compiled_tasks.append(task)
	except AttributeError:
		self.compiled_tasks = [task]

	#------------------------------------------------------
	# installation
	#------------------------------------------------------

	if getattr(self.bld, 'is_install', None) and (uid or huid):
		pkg = getattr(self, 'package', src_nodes[0].parent.name)
		inst_to = getattr(self, 'install_path', None)

		if inst_to:
			self.header_install_task = self.bld.install_as(
				dest    = inst_to + '/' + pkg + '.brl',
				srcfile = out_nodes[1],
				env     = self.env,
				chmod   = Utils.O644
			)


