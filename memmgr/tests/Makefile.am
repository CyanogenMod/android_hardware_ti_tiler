AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

CFLAGS=-Wall -pipe -ansi -I.. -I../../utils

## sources
bin_PROGRAMS = utils_test memmgr_test

if !STUB_TILER
  bin_PROGRAMS += tiler_test
endif

utils_testdir = .
utils_test_SOURCES = utils_test.c testlib.c

memmgr_testdir = .
memmgr_test_SOURCES = memmgr_test.c testlib.c
memmgr_test_LDADD = ../libmemmgr.la

tiler_test_SOURCES = tiler_test.c
tiler_test_LDADD = ../libmemmgr.la

