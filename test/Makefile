INCPATH=.:../src
LIBPATH=../src
SHAREDLIB+=-lmemmgr
PUBLIC_API=tilermgr.h

SRC=\
	memmgr_test.c\
	utils_test.c\
	tiler_test.c

BIN=$(SRC:.c=)

INCLUDE+=$(patsubst %,-I%,$(subst :, ,$(INCPATH)))
LIBINCLUDE+=$(patsubst %,-L%,$(subst :, ,$(LIBPATH)))

CFLAGS+=-Wall -fpic -pipe -ansi $(INCLUDE)

CC=$(CROSS)gcc
OBJ=$(SRC:.c=.o)

all:: $(BIN)

install::
	cp -f $(BIND) $(BINDIR)

%: %.c
	$(CC) $(CFLAGS) $(LIBINCLUDE) $(SHAREDLIB) $< -o $@

clean::
	rm -f $(BIN)
	rm -f $(OBJ)

distclean:: clean
	rm -f $(BINDIR)/$(BIN)

