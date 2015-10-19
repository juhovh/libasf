#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006 (ita)

# advanced tests
# look in src/ for an example of a new compiler that creates cpp files

# the following two variables are used by the target "waf dist"
VERSION='0.1.0'
APPNAME='libasf'

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = '_build_'

def options(opt):
	# the gcc module provides a --debug-level option
	opt.load('compiler_c')

def configure(conf):
	conf.load('compiler_c')

def build(bld):
	# process subfolders from here
	bld.recurse('src')


