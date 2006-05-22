# Copyright (C) 2000-2001 Open Source Telecom Corporation.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT(reconfig)

cfg_options=""
if test -z "$*" ; then
        if test -f ~/.configure ; then
                cfg_options=`grep ^ccaudio2: ~/.configure | sed -e s/^ccaudio2://`
        elif test -f /etc/configure.conf ; then
		cfg_options=`grep ^ccaudio2: /etc/configure.conf | sed -e s/^ccaudio2://`
        fi
fi

if test ! -z "$cfg_options" ; then
        echo "using ~/.configure defaults...$cfg_options"
	case "$SHELL" in
	*/bash*)
		exec $SHELL $0 $cfg_options
		exit -1
		;;
	esac
	exec $0 $cfg_options
	exit -1
fi

AUDIO_LIBS=""

VERSION="0.9.5"
LT_RELEASE="0.9"
LT_VERSION="0:5"
AC_CONFIG_AUX_DIR(autoconf)
AC_CANONICAL_SYSTEM
AC_PROG_CPP
AC_PROG_CXXCPP
AC_PROG_CXX
AM_PROG_LIBTOOL
AM_INIT_AUTOMAKE(ccaudio2,[$VERSION])
AM_CONFIG_HEADER(private.h)
AC_CHECK_HEADERS(stdint.h sys/soundcard.h)
AC_CHECK_LIB(ossaudio, _oss_ioctl, 
	[AUDIO_LIBS="$AUDIO_LIBS -lossaudio"])
		
ac_gsm_codec=false
ac_spx_codec=false

AC_CHECK_HEADER(gsm/gsm.h,
	[ac_gsm_codec=true
	AC_DEFINE(HAVE_GSM_GSM_H, [1], [gsm header found])
	])

# speex 1.1.5 or later
AC_CHECK_HEADER(speex/speex.h,
	AC_CHECK_LIB(speex, speex_encode_int, [
		[ac_spx_codec=true])
	])

WINVERSION="`echo $VERSION | sed 's/\./,/g'`,0"
AC_SUBST(WINVERSION)

OST_DEBUG

case "$prefix" in
/opt/*)
        if test "$localstatedir" == '${prefix}/var' ; then
                localstatedir=/var${prefix} ; fi

        if test "$datadir" == '${prefix}/share' ; then
                if test "$mandir" == '${datadir}/man' ; then
                        mandir='${prefix}/man' ; fi
                if test "$infodir" == '${datadir}/info' ; then
                        infodir='${prefix}/info' ; fi
                datadir='${prefix}'
        fi
        AC_SUBST(mandir)
        AC_SUBST(infodir)
        AC_SUBST(datadir)
	AC_SUBST(localstatedir)
	;;
esac

if test "$localstatedir" == '${prefix}/var' ; then
	localstatedir="/var" ; fi
AC_SUBST(localstatedir)

codec_libdir='${libdir}/ccaudio2-'${LT_RELEASE}
eprefix=${exec_prefix}
if test "$eprefix" == "NONE" ; then
        eprefix=${prefix} ; fi

case "$eprefix" in
/opt/*)
        if test "$libdir" == '${exec_prefix}/lib' ; then
                codec_libdir='${exec_prefix}/lib/ccaudio2'
	fi
        ;;
esac

AC_SUBST(STAGE2)
AC_SUBST(codec_libdir)
AC_SUBST_DIR(codec_libpath, codec_libdir)

AC_DEFINE_UNQUOTED(CODEC_LIBPATH, "$codec_libpath", [define codec path])

AH_TOP(

#ifndef W32
#if defined(_WIN32) && defined(_MSC_VER)
#define W32
#endif
#if defined(__BORLANDC__) && defined(__Windows)
#define W32
#endif
#endif

#if !defined(__EXPORT) && defined(W32)
#define __EXPORT __declspec(dllexport)
#endif

#ifdef	W32
#include "w32/private.h"
#else	// w32 uses hardcoded config

)

AH_BOTTOM(

#ifndef	NO_CPP

// hidden mutex class

#ifdef	HAVE_PTHREAD_H
#include <pthread.h>
#endif

class ccAudio_Mutex_
{
private:
#ifdef	HAVE_PTHREAD_H
        pthread_mutex_t mutex;
#endif

public:
        ccAudio_Mutex_();
        ~ccAudio_Mutex_();
        void enter(void);
        void leave(void);
}; 

#endif
#endif // end of internal configures

)

OST_PROG_LIBVER
OST_PROG_LIBRARY(CCXX,[$LT_VERSION])
OST_CC_ENDIAN
OST_CC_STRING
OST_CC_DYNAMIC
OST_CC_THREADING
OST_AUTOMAKE_MODE
OST_MAINTAINER_MODE
AM_CONDITIONAL(MODULES, test $ost_cv_dynloader = yes)
KDOC_DIR="\$(top_srcdir)/doc"
AC_SUBST(LT_VERSION)
AC_SUBST(LT_RELEASE)
AC_SUBST(KDOC_DIR)

AC_CHECK_HEADERS(CoreAudio/CoreAudio.h, [
	 AC_DEFINE(OSX_AUDIO, [1], [OSX Core Audio])
	 AUDIO_LIBS="-framework CoreAudio"
])

SHARED_FLAGS=""
MODULE_FLAGS="-module -shared"
STAGE2=""      
case "$target_os" in
cygwin*)
        SHARED_FLAGS="-no-undefined"
        MODULE_FLAGS="-module -shared -no-undefined"
	AUDIO_LIBS="-lwinmm"
	;;
darwin6*)
	MODULE_FLAGS="-dynamic -bundle -undefined suppress -flat_namespace -read_only_relocs suppress"
        STAGE2="macosx"
        ;;
darwin*)
	MODULE_FLAGS="-dynamic -bundle -undefined suppress -flat_namespace -read_only_relocs suppress"
        ;;
esac

audio_library="-lccaudio2 $DYN_LOADER $AUDIO_LIBS -lm"
AC_DEFINE_UNQUOTED(CODEC_MODFLAGS, "$MODULE_FLAGS", [module flags])
AC_DEFINE_UNQUOTED(AUDIO_LIBRARY, "$audio_library", [linking flags])

AC_SUBST(AUDIO_LIBS)
AC_SUBST(MODULE_FLAGS)
AC_SUBST(SHARED_FLAGS)
AC_SUBST(STAGE2)

gsm_format="false"
if test "$ac_gsm_codec" ; then
	gsm_format="true" ; fi

speex_format="false"
if test "$ac_spx_codec" ; then
	speex_format="true" ; fi

AC_SUBST(gsm_format)
AC_SUBST(speex_format)

AC_PATH_PROG(DOXYGEN, doxygen, no)
AC_SUBST(DOXYGEN)
AM_CONDITIONAL(DOXY, test "$DOXYGEN" != "no")
AM_CONDITIONAL(MODULES, test "$ost_cv_dynloader" = yes)
AM_CONDITIONAL(GSM_CODEC, test "$ac_gsm_codec" = true)
AM_CONDITIONAL(SPEEX_CODEC, test "$ac_spx_codec" = true)

AC_OUTPUT(src/Makefile m4/Makefile doc/Makefile w32/Makefile codecs/Makefile 
utils/Makefile Makefile libccaudio2.pc ccaudio2.spec ccaudio2.list 
w32/ccaudio2.rc doc/Doxyfile)


