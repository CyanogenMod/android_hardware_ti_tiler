BIN=tilertest

SRC=\
	tiler_test.c

INCLUDE+=-I./ -I$(SHAREDINCDIR)

CFLAGS+=-Wall -pipe -ansi $(INCLUDE)
SHAREDLIB+=-ltilermgr

CC=$(CROSS)gcc
OBJ=$(SRC:.c=.o)

all:: $(BIN)

install::
	cp -f $(BIN) $(BINDIR)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -L$(LIBDIR) $(SHAREDLIB) -o $(BIN) $(OBJ)

clean::
	rm -f $(BIN)
	rm -f $(OBJ)

distclean:: clean
	rm -f $(BINDIR)/$(BIN)
