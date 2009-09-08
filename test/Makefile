SHAREDINCPATH=-I. -I../src
TESTS=memmgr_test utils_test
PUBLIC_API=tilermgr.h

SRC=\
	memmgr_test.c
	utils_test.c

INCLUDE+=$(SHAREDINCPATH)

CFLAGS+=-Wall -fpic -pipe $(INCLUDE)

CC=$(CROSS)gcc
OBJ=$(SRC:.c=.o)

all:: $(TESTS)

install::
	cp -f $(TESTS) $(EXEDIR)

% : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean::
	rm -f $(LIB)
	rm -f $(OBJ)

distclean:: clean
	rm -f $(LIBDIR)/$(LIB)
	rm -f $(INCDIR)/$(PUBLIC_API)
