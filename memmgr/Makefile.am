AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

## sources

h_sources = memmgr.h tilermem.h mem_types.h
c_sources = memmgr.c tilermgr.c

if TILERMGR
library_include_HEADERS = $(h_sources) tilermgr.h
else
library_include_HEADERS = $(h_sources)
endif

#Install the headers in library directory - e.g. examplelib-1.0:
library_includedir=$(includedir)

# library sources
lib_LTLIBRARIES= libmemmgr.la
libmemmgr_la_SOURCES = $(h_sources) $(c_sources)
# libmemmgr_la_LIBADD= sub/libsub.la
# libmemmgr_la_LDFLAGS = -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
libmemmgr_la_CFLAGS  = -Wall -fpic -pipe -ansi -I../utils
libmemmgr_la_LIBTOOLFLAGS = --tag=disable-static
libmemmgr_la_LDFLAGS = -version-info 0:0:0
 
noinst_HEADERS = tiler.h tilermem_utils.h

if UNIT_TESTS
SUBDIRS = . tests
endif


