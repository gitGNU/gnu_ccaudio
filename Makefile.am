# Copyright (C) 2009 David Sugar, Tycho Softworks.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AUTOMAKE_OPTIONS = no-dependencies dist-shar dist-zip
EXTRA_DIST = autogen.sh README *.pc.in *.spec.in *.spec *-config.in \
	cmodel.sh ccaudio2-config.1 SUPPORT CMakeLists.txt config.h.cmake \
	COPYING* THANKS

SUBDIRS = src inc utils

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ccaudio2.pc

scriptdir = $(bindir)
script_SCRIPTS = ccaudio2-config

man_MANS = ccaudio2-config.1

