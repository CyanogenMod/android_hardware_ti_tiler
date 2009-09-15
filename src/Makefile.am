AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

## sources

h_sources = memmgr.h tilermem.h mem_types.h phase1_d2c_remap.h
c_sources = memmgr.c tilermgr.c

if TILERMGR
library_include_HEADERS = $(h_sources) tilermgr.h
else
library_include_HEADERS = $(h_sources)
endif

#Install the headers in library directory - e.g. examplelib-1.0:
library_includedir=$(includedir)/memmgr

# library sources
lib_LTLIBRARIES= libmemmgr.la libd2cmap.la
libmemmgr_la_SOURCES = $(h_sources) $(c_sources)
# libmemmgr_la_LIBADD= sub/libsub.la
# libmemmgr_la_LDFLAGS = -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
libmemmgr_la_CFLAGS  = -Wall -fpic -pipe -ansi
libmemmgr_la_LIBTOOLFLAGS = --tag=disable-static
libmemmgr_la_LDFLAGS = -version-info 0:0:0
 
libd2cmap_la_SOURCES = phase1_d2c_remap.c
libd2cmap_la_CFLAGS  = -Wall -fpic -pipe -I$(includedir)/syslink
libd2cmap_la_LIBTOOLFLAGS = --tag=disable-static
libd2cmap_la_LDFLAGS = -version-info 0:0:0

noinst_HEADERS = tiler.h tilermem_utils.h list_utils.h debug_utils.h utils.h

