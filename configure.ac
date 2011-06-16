# Copyright (C) 2009 David Sugar, Tycho Softworks.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT(autogen.sh)

VERSION="2.0.5"
LT_VERSION="2:0:0"
UCOMMON_REQUIRES="4.1.0"

AC_CONFIG_AUX_DIR(autoconf)
AC_CANONICAL_SYSTEM
AC_PROG_CPP
AC_PROG_CC
AC_PROG_CXXCPP
AC_PROG_CXX
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AM_INIT_AUTOMAKE(ccaudio2, [$VERSION])
AM_CONFIG_HEADER(config.h)

AC_C_RESTRICT
AC_C_VOLATILE
AC_C_INLINE

AC_SUBST(LT_VERSION)

if test -z "$PKG_CONFIG_PATH" ; then
    case "$prefix" in
    NONE|/usr/local|/usr)
        PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:/usr/lib/pkgconfig"
        ;;
    *)
        PKG_CONFIG_PATH="$prefix/lib/pkgconfig"
        ;;
    esac
else
    case "$prefix" in
    NONE|/usr/local|/usr)
        PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig:/usr/lib/pkgconfig"
        ;;
    *)
        PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$prefix/lib/pkgconfig"
        ;;
    esac
fi

export PKG_CONFIG_PATH

EXPORT_LIBS=""
EXPORT_FLAGS=""
CCAUDIO_LIBS=""
CCAUDIO_FLAGS=""

test -z "$plugindir" && plugindir='${libdir}'
test -z "$phrasedir" && phrasedir='${datadir}/ccaudio2'

AC_DEFUN([AC_SUBST_DIR], [
    ifelse($2,,,$1="[$]$2")
    result="***"
    prior="A"
    while test "$prior" != "$result" ; do
        prior=`(echo "[$]$1")`
        $1=`(
        test "x$prefix" = xNONE && prefix="$ac_default_prefix"
        test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
        eval echo \""[$]$1"\"
        )`
        result=`(echo "[$]$1")`
    done
    AC_SUBST($1)
])

AC_ARG_ENABLE(debug,
    AC_HELP_STRING([--enable-debug],[compile for debugging]))
if test -z "$enable_debug" ; then
    enable_debug="no"
elif test $enable_debug = "yes" ; then
    CXXFLAGS="${CXXFLAGS} -g -DDEBUG"
fi

AC_ARG_WITH(pkg-config,
    AC_HELP_STRING([--with-pkg-config],[enable support for pkg-config]),[
    PKG_CHECK_MODULES(UCOMMON, ucommon >= $UCOMMON_REQUIRES)
    UCOMMON_CLINK=`pkg-config --variable=clink ucommon`
    UCOMMON_MODEL=`pkg-config --variable=model ucommon`
],[
    AC_PATH_PROG([UCOMMON],[ucommon-config],[none])
    if test $UCOMMON = "none" ; then
        AC_ERROR("required ucommon library missing")
    fi
    UCOMMON_CLINK=`$UCOMMON --clink`
    UCOMMON_MODEL=`$UCOMMON --model`
    UCOMMON_CFLAGS=`$UCOMMON --cflags`
    UCOMMON_LIBS=`$UCOMMON --libs`

])

if test "$UCOMMON_MODEL" = "CC" ; then
    LIBTOOL='$(SHELL) $(top_srcdir)/cmodel.sh'" ${LIBTOOL}" ; fi

AC_LANG_CPLUSPLUS
COMPILER_FLAGS=""
for flag in $CXXFLAGS ; do
    case "$flag" in
    -f*exceptions|-f*rtti|-f*check-new|-f*enforce-eh-specs|-finline|-f*except)
        if test "$UCOMMON_MODEL" = "CXX" ; then
            COMPILER_FLAGS="$COMPILER_FLAGS $flag" ; fi
        ;;
    -fvisibility=*)
        ;;
    *)
        COMPILER_FLAGS="$COMPILER_FLAGS $flag"
        ;;
    esac
done
export CXXFLAGS="$COMPILER_FLAGS"

AC_CHECK_HEADERS(endian.h)
AC_CHECK_HEADERS(sys/soundcard.h)
AC_CHECK_HEADERS(CoreAudio/CoreAudio.h, [
    AC_DEFINE(OSX_AUDIO, [1], [OSX Core Audio])
    AUDIO_LIBS="-framework CoreAudio"
])

AC_CHECK_LIB([m], [sqrt], [EXPORT_LIBS="$EXPORT_LIBS -lm"])
AC_CHECK_LIB([ossaudio], [_oss_ioctl], [AUDIO_LIBS="-lossaudio"])

ac_gsm_codec=false
ac_spx_codec=false

AC_CHECK_HEADER(gsm/gsm.h, [
    ac_gsm_codec=true
    AC_DEFINE(HAVE_GSM_GSM_H, [1], [gsm header found])
    AUDIO_LIBS="$AUDIO_LIBS -lgsm"
],[
    AC_CHECK_HEADER(gsm.h, [
        AUDIO_LIBS="$AUDIO_LIBS -lgsm"
        AC_DEFINE(HAVE_GSM_H, [1], [gsm default header])
    ])
])

AC_ARG_ENABLE(speex, [ --disable-speex          disable speex support])
if test "$enable_speex" != "no" ; then
    AC_CHECK_HEADER(speex/speex.h, [
        AC_CHECK_LIB(speex, speex_encode_int, [
            AC_DEFINE(HAVE_SPEEX_SPEEX_H, [1], [speex codec support])
            AUDIO_LIBS="$AUDIO_LIBS -lspeex"
        ])
    ])
fi

if test "$enable_shared" != "yes" ; then
    AC_DEFINE(BUILD_STATIC, [1], [to disable plugin support])
fi

EXPORT_LIBS="$AUDIO_LIBS $EXPORT_LIBS"
CCAUDIO_FLAGS="$EXPORT_FLAGS $UCOMMON_CFLAGS"
CCAUDIO_LIBS="$EXPORT_LIBS $UCOMMON_LIBS $ac_with_malloc $UCOMMON_CLINK"

case "$CCAUDIO_FLAGS -I/usr/include" in
*-I${default_incpath}*)
    ;;
*)
    EXPORT_FLAGS="$EXPORT_FLAGS -I$default_incpath"
    ;;
esac

AC_SUBST_DIR(default_libpath, plugindir)
AC_SUBST_DIR(default_phrases, phrasedir)

AC_DEFINE_UNQUOTED(DEFAULT_LIBPATH, "$default_libpath", [lib path])
AC_DEFINE_UNQUOTED(DEFAULT_PHRASES, "$default_phrases", [phrase path])

AC_SUBST(plugindir)
AC_SUBST(phrasedir)
AC_SUBST(UCOMMON_REQUIRES)
AC_SUBST(UCOMMON_MODEL)
AC_SUBST(DAEMON_LIBS)
AC_SUBST(EXPORT_FLAGS)
AC_SUBST(EXPORT_LIBS)
AC_SUBST(CCAUDIO_FLAGS)
AC_SUBST(CCAUDIO_LIBS)
AC_SUBST(CXXFLAGS)
AC_OUTPUT(Makefile inc/Makefile src/Makefile utils/Makefile
ccaudio2-config ccaudio2.pc ccaudio2.spec)

