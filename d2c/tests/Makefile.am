AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

CFLAGS=-Wall -pipe -ansi -I../../utils -I../../memmgr

## sources
bin_PROGRAMS = d2c_test

d2c_test_SOURCES = d2c_test.c testlib.c
d2c_test_LDADD = ../../memmgr/libmemmgr.la ../libd2cmap.la $(SYSLINK_LIBS)

