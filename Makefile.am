## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4 -I m4/common

SUBDIRS = src data

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
config.guess \
config.h.in \
config.h.in~ \
config.sub \
configure \
depcomp \
install-sh \
ltconfig \
ltmain.sh \
missing \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.bz2 \
m4/libtool.m4 \
m4/lt~obsolete.m4 \
m4/ltoptions.m4 \
m4/ltsugar.m4 \
m4/ltversion.m4

EXTRA_DIST = \
AUTHORS \
COPYING \
README \
autogen.sh \
ems.pc.in \
m4/common/efl_attribute.m4 \
m4/common/efl_compiler_flag.m4 \
m4/common/efl_path_max.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ems.pc
