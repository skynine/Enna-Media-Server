##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [1])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v '\(export\|Unversioned directory\)' || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
dnl m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
dnl m4_define([relname], [ver-pre-svn-07])
dnl m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])], [m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_INIT([enna-media-server], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef EMS_CONFIG_H__
#define EMS_CONFIG_H__
])
AH_BOTTOM([
#endif /* EMS_CONFIG_H__ */
])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_GNU_SOURCE
AC_SYS_LARGEFILE
AC_PROG_CXX

LT_PREREQ([2.2])
LT_INIT([win32-dll])

m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
version_info="lt_cur:lt_rev:lt_age"
release_info="v_rel"
AC_SUBST([version_info])
AC_SUBST([release_info])


### Needed information

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

requirement_ems=""

have_macosx="no"
case "$host_os" in
   darwin*)
      have_macosx="yes"
      ;;
esac
AM_CONDITIONAL([HAVE_MACOSX], [test "x${have_macosx}" = "xyes"])

case "$host_os" in
   mingw32ce*)
      MODULE_ARCH="$host_os-$host_cpu"
      ;;
   *)
      MODULE_ARCH="$host_os-$host_cpu-v_maj.v_min.v_mic"
      ;;
esac
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "${MODULE_ARCH}", "Module architecture")

case "$host_os" in
   mingw*)
      MODULE_EXT=".dll"
      ;;
   *)
      MODULE_EXT=".so"
      ;;
esac

AC_DEFINE_UNQUOTED(SHARED_LIB_SUFFIX, "${MODULE_EXT}", [Suffix for shared objects])

### Checks for programs
AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])

### Checks for libraries

requirements_ems="eina >= 1.7.0 eet >= 1.7.0 ecore >= 1.7.0 ecore-file >= 1.7.0 ecore-con >= 1.7.0 eio >= 1.7.0 avahi-client >= 0.6.0 avahi-glib >= 0.6.0 uuid"
PKG_CHECK_MODULES([EMS], [${requirements_ems}])
AC_SUBST([requirements_ems])

requirements_enna="elementary >= 1.7.0 emotion >= 1.7.0 ecore-input >= 1.7.0"
PKG_CHECK_MODULES([ENNA], [${requirements_enna}], [have_enna_bin_dep="yes"], [have_ems_enna_dep="no"])
AM_CONDITIONAL([HAVE_ENNA_BIN], [test "x${have_enna_bin_dep}" = "xyes"])

PKG_CHECK_MODULES([ECORE_CON_EET], [ecore-con >= 1.7.99], [have_ecore_con_eet="yes"], [have_ecore_con_eet="no"])
AM_CONDITIONAL([HAVE_ECORE_CON_EET], [test "x${have_ecore_con_eet}" = "xyes"])

### Checks for compiler characteristics
AC_C_BIGENDIAN
AC_C_INLINE
EFL_ATTRIBUTE_UNUSED

m4_ifdef([v_mic],
   [
    EFL_COMPILER_FLAG([-Wall])
    EFL_COMPILER_FLAG([-Wextra])
   ])

EFL_COMPILER_FLAG([-Wshadow])


### Checks for linker characteristics
lt_enable_auto_import=""
case "${host_os}" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
   ;;
esac
AC_SUBST([lt_enable_auto_import])

EFL_LINKER_FLAG([-fno-strict-aliasing])


### Checks for header files
AC_HEADER_ASSERT
AC_CHECK_HEADERS([unistd.h])
EFL_CHECK_PATH_MAX


AC_CONFIG_FILES([
ems.pc
Makefile
src/Makefile
src/lib/Makefile
src/lib/server/Makefile
src/lib/extras/Makefile
src/bin/Makefile
src/bin/ems/Makefile
src/bin/enna/Makefile
src/modules/Makefile
src/modules/tmdb/Makefile
src/modules/thetvdb/Makefile
src/modules/betaseries/Makefile
src/modules/t411/Makefile
src/tests/Makefile
data/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/images/Makefile
])

AC_OUTPUT

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
