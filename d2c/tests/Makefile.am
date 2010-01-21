AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS=-I m4

## sources
bin_PROGRAMS = d2c_test

d2c_test_CFLAGS = $(D2C_CFLAGS) -ansi -I$(top_srcdir)/../utils $(MEMMGR_CFLAGS)
d2c_test_SOURCES = d2c_test.c testlib.c
d2c_test_LDADD = ../libd2cmap.la $(SYSLINK_LIBS) $(MEMMGR_LIBS)

