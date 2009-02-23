CC		:= gcc
CFLAGS	:= --fast-math -O6
LIBS	:= -lcurl -lpthread
OBJECTS	:= $(patsubst %.c,%.o,$(wildcard *.c))
HEADERS	:= $(wildcard *.h)
BINDIR	:= /usr/local/bin/

all : $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJECTS) -o gurls $(LIBS)

install : gurl
	install -d $(BINDIR)
	install gurls $(BINDIR)

uninstall : gurl
	rm -f $(BINDIR)/gurls

clean : 
	rm -rf gurls *.o
