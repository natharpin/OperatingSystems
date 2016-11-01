IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR=../lib

LIBS=-pthread

_DEPS=chat.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_CLIENT = client.o
CLIENT = $(patsubst %,$(ODIR)/%,$(_CLIENT))

_SERVER = server.o
SERVER = $(patsubst %,$(ODIR)/%,$(_SERVER))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(CLIENT)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(SERVER)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
