SHAREDINCPATH=-I.
LIB=libmemmgr.so
PUBLIC_API=tilermgr.h

SRC=\
	memmgr.c\
	tilermgr.c

INCLUDE+=$(SHAREDINCPATH)

CFLAGS+=-Wall -fpic -pipe $(INCLUDE)

CC=$(CROSS)gcc
OBJ=$(SRC:.c=.o)

all:: $(LIB)

install::
	cp -f $(LIB) $(LIBDIR)
	cp -f $(PUBLIC_API) $(INCDIR)

$(LIB):: $(OBJ)
	$(CC) $(CFLAGS) -shared -o $(LIB) $(OBJ)

clean::
	rm -f $(LIB)
	rm -f $(OBJ)

distclean:: clean
	rm -f $(LIBDIR)/$(LIB)
	rm -f $(INCDIR)/$(PUBLIC_API)
