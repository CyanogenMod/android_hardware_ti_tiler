AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

## sources

h_sources = memmgr.h tilermem.h mem_types.h tiler.h tilermem_utils.h
if STUB_TILER
c_sources = memmgr.c
else
c_sources = memmgr.c tilermgr.c
endif

if TILERMGR
library_include_HEADERS = $(h_sources) tilermgr.h
else
library_include_HEADERS = $(h_sources)
noinst_HEADERS = tilermgr.h
endif

#Install the headers in library directory - e.g. examplelib-1.0:
library_includedir=$(includedir)/memmgr

# library sources
lib_LTLIBRARIES= libmemmgr.la
libmemmgr_la_SOURCES = $(h_sources) $(c_sources)
libmemmgr_la_CFLAGS  = $(MEMMGR_CFLAGS) -fpic -ansi
libmemmgr_la_LIBTOOLFLAGS = --tag=disable-static
libmemmgr_la_LDFLAGS = -version-info 0:0:0

if UNIT_TESTS
bin_PROGRAMS = utils_test memmgr_test tiler_ptest

utils_testdir = .
utils_test_SOURCES = utils_test.c testlib.c

memmgr_testdir = .
memmgr_test_SOURCES = memmgr_test.c testlib.c
memmgr_test_LDADD = libmemmgr.la

tiler_ptest_SOURCES = tiler_ptest.c
tiler_ptest_LDADD = libmemmgr.la
endif

pkgconfig_DATA = libmemmgr.pc
pkgconfigdir   = $(libdir)/pkgconfig

