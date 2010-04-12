AM_CPPFLAGS = @LIBCURL_CFLAGS@ @LOOMLIB_CFLAGS@
gurls_LDADD = @LIBCURL_LIBS@ @LOOMLIB_LIBS@

bin_PROGRAMS = gurls
gurls_SOURCES =   \
  gurls.c         \
  gurls.h         \
  main.c
gurls_CFLAGS = $(AM_CFLAGS)

lib_LTLIBRARIES = libgurls.la
libgurls_la_SOURCES =	\
  gurls.c             \
  gurls.h
include_HEADERS = gurls.h
