LIB=libtilermgr.so
PUBLIC_HDR=tilermgr.h

SRC=\
	tilermgr.c

CFLAGS+=-Wall -fpic -pipe -ansi -I./ -I$(TILER_DRIVER_INC_PATH)

CC=$(CROSS)gcc
OBJ=$(SRC:.c=.o)

all:: $(LIB)

install::
	cp -f $(LIB) $(LIBDIR)
	cp -f $(PUBLIC_HDR) $(INCDIR)

$(LIB):: $(OBJ)
	$(CC) $(CFLAGS) -shared -o $(LIB) $(OBJ)

clean::
	rm -f $(LIB)
	rm -f $(OBJ)

distclean:: clean
	rm -f $(LIBDIR)/$(LIB)
	rm -f $(INCDIR)/$(PUBLIC_HDR)
