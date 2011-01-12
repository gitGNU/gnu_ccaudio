# Copyright (C) 2009 David Sugar, Tycho Softworks.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

MAINTAINERCLEANFILES = Makefile.in Makefile
AM_CXXFLAGS = -I$(top_srcdir)/inc @CCAUDIO_FLAGS@ $(CHECKFLAGS)
INCLUDES = -I$(top_srcdir)/inc
LDADD = ../src/libccaudio.la @CCAUDIO_LIBS@
EXTRA_DIST = audiotool.1 tonetool.1 tones.conf

man_MANS = audiotool.1 tonetool.1
bin_PROGRAMS = tonetool audiotool

tonetool_SOURCES = tonetool.cpp
audiotool_SOURCES = audiotool.cpp

