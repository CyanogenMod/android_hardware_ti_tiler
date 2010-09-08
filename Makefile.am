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
# libmemmgr_la_LIBADD= sub/libsub.la
# libmemmgr_la_LDFLAGS = -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
libmemmgr_la_CFLAGS  = $(MEMMGR_CFLAGS) -fpic
libmemmgr_la_LIBTOOLFLAGS = --tag=disable-static
libmemmgr_la_LDFLAGS = -version-info 0:0:0

if UNIT_TESTS
SUBDIRS = . tests
endif

pkgconfig_DATA = libmemmgr.pc
pkgconfigdir   = $(libdir)/pkgconfig

