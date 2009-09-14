AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

CFLAGS=-Wall -pipe -ansi -I../src

## sources
bin_PROGRAMS = utils_test tiler_test memmgr_test

utils_testdir = .
utils_test_SOURCES = utils_test.c testlib.c

memmgr_testdir = .
memmgr_test_SOURCES = utils_test.c testlib.c
memmgr_test_LDADD = ../src/libmemmgr.la

tiler_test_SOURCES = tiler_test.c
tiler_test_LDADD = ../src/libmemmgr.la

