CC	= gcc
AR	= ar

INCDIR 		= /usr/local/include
INSTALLDIR	= /usr/local/lib
BINDIR		= /usr/local/bin

CFLAGS	= --fast-math -O6 -fPIC
LIBS	= -lcurl -lpthread

SRCS	= download.c gurls.c queue.c
INCLS	= download.h gurls.h queue.h common.h
MODULES	= $(SRCS:.c=.o)

VER_MAJOR	= 1
VER_MINOR	= 0.1
TARGET		= gurls
STATICLIB	= lib$(TARGET).a
SHAREDLIB	= lib$(TARGET)-$(VER_MAJOR).$(VER_MINOR).so
LIBNAME 	= lib$(TARGET).so
VERLIBNAME	= $(LIBNAME).$(VER_MAJOR)
HEADER		= gurls.h
EXEC		= gurls

all: $(STATICLIB) $(SHAREDLIB) $(EXEC)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(STATICLIB): $(MODULES)
	$(AR) r $@ $(MODULES)

$(SHAREDLIB): $(MODULES)
	$(CC) -s -shared -Wl,-soname,$(VERLIBNAME) -o $@ $(MODULES) $(LIBS)

$(EXEC): $(MODULES) main.c
	$(CC) -o $@ $(MODULES) main.c $(LIBS)

install:
	install -m 0644 -o root -g root $(HEADER) $(INCDIR)
	install -m 0644 -o root -g root $(STATICLIB) $(INSTALLDIR)
	install -m 0755 -o root -g root $(SHAREDLIB) $(INSTALLDIR)
	ln -sf $(SHAREDLIB) $(INSTALLDIR)/$(VERLIBNAME)
	ln -sf $(VERLIBNAME) $(INSTALLDIR)/$(LIBNAME)
	install $(EXEC) $(BINDIR)
	ldconfig

clean:
	rm -f $(MODULES) $(STATICLIB) $(SHAREDLIB) $(LIBNAME) $(EXEC)
