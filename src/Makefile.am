AM_CPPFLAGS = @PACKAGE_CFLAGS@
gurls_LDADD = @PACKAGE_LIBS@

bin_PROGRAMS = gurls
gurls_SOURCES =		\
    common.h        \
    download.c		\
    download.h      \
    gurls.c         \
    gurls.h         \
	main.c			\
    queue.c         \
    queue.h
gurls_CFLAGS = $(AM_CFLAGS)

lib_LTLIBRARIES = libgurls.la
libgurls_la_SOURCES =	\
	common.h			\
	download.c			\
	download.h			\
	gurls.c				\
	gurls.h				\
	queue.c				\
	queue.h
include_HEADERS = gurls.h
