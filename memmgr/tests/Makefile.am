AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

CFLAGS=$(MEMMGR_CFLAGS) -ansi -I$(top_srcdir) -I$(top_srcdir)/../utils

## sources
bin_PROGRAMS = utils_test memmgr_test tiler_ptest

utils_testdir = .
utils_test_SOURCES = utils_test.c testlib.c

memmgr_testdir = .
memmgr_test_SOURCES = memmgr_test.c testlib.c
memmgr_test_LDADD = ../libmemmgr.la

tiler_ptest_SOURCES = tiler_ptest.c
tiler_ptest_LDADD = ../libmemmgr.la

