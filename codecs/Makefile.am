# Copyright (C) 1999-2005 Open Source Telecom Corporation.
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AUTOMAKE_OPTIONS = no-dependencies dist-shar
LDADD = ../src/libccaudio2.la

pkglibdir = $(codec_libdir)
noinst_HEADERS = codecs.h

if MODULES
pkglib_PROGRAMS = g.711.codec g.721.codec 

if GSM_CODEC
pkglib_PROGRAMS += gsm.codec
endif

if SPEEX_CODEC
pkglib_PROGRAMS += speex.codec
endif

AM_CPPFLAGS = -I../src -I$(top_srcdir)/src -fPIC -DPIC

g_721_codec_SOURCES = g721.cpp
g_721_codec_LDFLAGS = @MODULE_FLAGS@ -lm

g_711_codec_SOURCES = g711.cpp
g_711_codec_LDFLAGS = @MODULE_FLAGS@ 

if GSM_CODEC
gsm_codec_SOURCES = gsm.cpp
gsm_codec_LDFLAGS = @MODULE_FLAGS@ -lgsm -lm
endif

if SPEEX_CODEC
speex_codec_SOURCES = speex.cpp
speex_codec_LDFLAGS = @MODULE_FLAGS@ -lspeex -lm
endif

endif


