CC		:= gcc
CFLAGS	:= --fast-math -O6
LIBS	:= -lcurl -lreadline
SOURCE	:= $(wildcard *.c)
BINDIR	:= /usr/local/bin/

all : $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o gurl $(LIBS)

install : gurl
	install -d $(BINDIR)
	install gurl $(BINDIR)

uninstall : gurl
	rm -f $(BINDIR)/gurl

clean : 
	rm -rf gurl
