AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

#Install the headers in library directory - e.g. examplelib-1.0:
library_includedir=$(includedir)
library_include_HEADERS = phase1_d2c_remap.h

lib_LTLIBRARIES= libd2cmap.la

libd2cmap_la_SOURCES = phase1_d2c_remap.c
libd2cmap_la_CFLAGS  = -Wall -fpic -pipe -I$(top_srcdir)/../utils $(MEMMGR_CFLAGS)
libd2cmap_la_LIBTOOLFLAGS = --tag=disable-static
libd2cmap_la_LDFLAGS = -version-info 0:0:0

if !STUB_SYSLINK
libd2cmap_la_CFLAGS  += $(SYSLINK_CFLAGS)
endif

noinst_HEADERS = 

if UNIT_TESTS
SUBDIRS = . tests
endif

