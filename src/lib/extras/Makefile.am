AM_CFLAGS = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/include \
  -I$(top_srcdir)/src/include/extras \
  $(CC_PIC)
AM_CXXFLAGS = $(AM_CFLAGS) $(CXX_PIC)
LIBADD = -lm

noinst_LTLIBRARIES = \
libcJSON.la \
libtinyxml2.la \
libsha1.la

EXTRA_DIST = \
cJSON.h tinyxml2.h sha1.h

libcJSON_la_SOURCES = cJSON.c
libtinyxml2_la_SOURCES = tinyxml2.cpp
libsha1_la_SOURCES = sha1.c

MAINTAINERCLEANFILES = Makefile.in
