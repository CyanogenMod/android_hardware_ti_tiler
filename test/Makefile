INCPATH=.:../src
LIBPATH=../src:.
SHAREDLIB+=-lmemmgr
TESTOBJ=testlib.o

SRC=\
	memmgr_test.c\
	utils_test.c\
	tiler_test.c

BIN=$(SRC:.c=)

INCLUDE+=$(patsubst %,-I%,$(subst :, ,$(INCPATH)))
LIBINCLUDE+=$(patsubst %,-L%,$(subst :, ,$(LIBPATH)))

CFLAGS+=-Wall -pipe -ansi $(INCLUDE)

CC=$(CROSS)gcc
OBJ=$(SRC:.c=.o) $(TESTOBJ)

all:: $(BIN)

install::
	cp -f $(BIND) $(BINDIR)

memmgr_test: memmgr_test.c $(TESTOBJ)
	$(CC) $(CFLAGS) $(LIBINCLUDE) $(SHAREDLIB) $(TESTOBJ) $< -o $@

utils_test: utils_test.c $(TESTOBJ)
	$(CC) $(CFLAGS) $(LIBINCLUDE) $(SHAREDLIB) $(TESTOBJ) $< -o $@

tiler_test: tiler_test.c
	$(CC) $(CFLAGS) $(LIBINCLUDE) $(SHAREDLIB) $< -o $@

clean::
	rm -f $(BIN)
	rm -f $(OBJ)

distclean:: clean
	rm -f $(BINDIR)/$(BIN)

