CC	= gcc
AR	= ar

INCDIR 		= /usr/local/include
LIBDIR	= /usr/local/lib
BINDIR	= /usr/local/bin

CFLAGS	= --fast-math -O6 -fPIC
LIBS	= -lcurl -lpthread

SRCS	= download.c gurls.c queue.c
INCLS	= download.h gurls.h queue.h common.h
MODULES	= $(SRCS:.c=.o)

TARGET		= gurls
STATICLIB	= lib$(TARGET).a
SHAREDLIB	= lib$(TARGET).so
HEADER		= gurls.h
EXEC		= gurls

all: $(STATICLIB) $(SHAREDLIB) $(EXEC)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(STATICLIB): $(MODULES)
	$(AR) r $@ $(MODULES)

$(SHAREDLIB): $(MODULES)
	$(CC) -s -shared -Wl,-soname,$(SHAREDLIB) -o $@ $(MODULES) $(LIBS)

$(EXEC): $(MODULES) main.c
	$(CC) -o $@ $(MODULES) main.c $(LIBS)

install:
	install -m 0644 -o root -g root $(HEADER) $(INCDIR)
	install -m 0644 -o root -g root $(STATICLIB) $(LIBDIR)
	install -m 0755 -o root -g root $(SHAREDLIB) $(LIBDIR)
	install $(EXEC) $(BINDIR)
	ldconfig

clean:
	rm -f $(MODULES) $(STATICLIB) $(SHAREDLIB) $(EXEC)

uninstall:
	rm -f $(INCDIR)/$(HEADER)
	rm -f $(LIBDIR)/$(STATICLIB)
	rm -f $(LIBDIR)/$(SHAREDLIB)
	rm -f $(BINDIR)/$(EXEC)
