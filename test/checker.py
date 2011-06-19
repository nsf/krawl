#!/usr/bin/env python

import subprocess, sys, os
from glob import glob

errors = 0
problems = []

RED = '\033[0;31m'
GRN = '\033[0;32m'
NC  = '\033[0m'

def read_file(fname, m='r', default=''):
	try:
		f = open(fname, m)
		try:
			return f.read()
		finally:
			f.close()
	except IOError:
		return default

def colored(color, text):
	return color + text + NC

def run_shell(cmd):
	p = subprocess.Popen(cmd, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, shell=True)
	out = str(p.communicate()[0], 'utf-8')
	return p.returncode, out

def run_test(f):
	global errors
	ret, out = run_shell("krawl -o test.o %s && clang -o test test.o" % f)
	if ret != 0:
		problems.append(colored(RED, "%s: failed to compile\n") % f + out)
		errors += 1
		return False

	gold = read_file("0.%s" % f)
	ret, out = run_shell("./test")
	if out != gold:
		problems.append(colored(RED, "%s: gold mismatch: %s\nVS\n%s") % (f, out, gold))
		errors += 1
		return False
	return True

files = [f for f in glob("*.krl") if not f.startswith("0.")]
files.sort()
for f in files:
	if run_test(f):
		sys.stdout.write(colored(GRN, "+"))
		sys.stdout.flush()
	else:
		sys.stdout.write(colored(RED, "-"))
		sys.stdout.flush()
print()

if errors != 0:
	for p in problems:
		print(p)
	print("---------------------------------------------")
	if errors == 1:
		print(colored(RED, "There was 1 unexpected ERROR!"))
	else:
		print(colored(RED, "There were %d unexpected ERRORS!") % errors)

os.unlink("test")
os.unlink("test.o")
