CC		:= gcc
CFLAGS	:= --fast-math -O6
LIBS	:= -lcurl -lreadline -lpthread
SOURCE	:= $(wildcard *.c)
HEADERS	:= $(wildcard *.h)
BINDIR	:= /usr/local/bin/

all : $(SOURCE) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCE) -o gurl $(LIBS)

install : gurl
	install -d $(BINDIR)
	install gurl $(BINDIR)

uninstall : gurl
	rm -f $(BINDIR)/gurl

clean : 
	rm -rf gurl *.o
