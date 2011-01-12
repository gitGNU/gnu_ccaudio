# Copyright (C) 2009 David Sugar, Tycho Softworks
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

cmake_minimum_required(VERSION 2.6)
PROJECT(ccaudio2)
set (VERSION 2.0.0)
set (SOVERSION 2)

if (WIN32)
    option(BUILD_STATIC "Set to OFF to build shared libraries" ON)
else()
    option(BUILD_STATIC "Set to ON to build static libraries" OFF)
endif()

MESSAGE( STATUS "Configuring GNU ccaudio ${VERSION}...")

# set to true for debug and trace during CMakeLists development
set(CMAKE_VERBOSE_MAKEFILE FALSE)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Set defaults and pass common options.  Common build options can be passed
# to cmake using cmake -DWITH_CFLAGS="...", WITH_LIBS, and WITH_INCLUDES
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc ${WITH_INCLUDES})
add_definitions(${WITH_CFLAGS})
link_libraries(${WITH_LIBS})

if (USES_UCOMMON_INCLUDE_DIRS)
    message(STATUS "  Using local ucommon dependency")
else()
    find_package(PkgConfig)
    pkg_check_modules(USES_UCOMMON REQUIRED ucommon>=2.0.7)
endif()

include_directories(${USES_UCOMMON_INCLUDE_DIRS})
link_directories(${USES_UCOMMON_LIBRARY_DIRS})
add_definitions(${USES_UCOMMON_CFLAGS})

# by default we build static libs for windows, shared libs for unix.
# we may also set this from a top level cmake or -DWITH_XX_LIBS

if(BUILD_STATIC)
    set(BUILD_LIBRARY_TYPE STATIC)
else()
    set(BUILD_LIBRARY_TYPE SHARED)
endif()

# app specific paths, -DWITH_LIBDIR, -DWITH_DATADIR effects...
if(WITH_LIBDIR)
    set(DEFAULT_LIBPATH ${WITH_LIBDIR})
else()
    set(DEFAULT_LIBPATH ${CMAKE_INSTALL_PREFIX}/lib)
endif()

if(WITH_DATADIR)
    set(DEFAULT_PHRASES ${WITH_DATADIR}/ccaudio})
else()
    set(DEFAULT_PHRASES ${CMAKE_INSTALL_PREFIX}/share/ccaudio)
endif()

check_include_files(sys/soundcard.h HAVE_SYS_SOUNDCARD_H)
check_include_files(speex/speex.h HAVE_SPEEX_SPEEX_H)
check_include_files(gsm.h HAVE_GSM_H)
check_include_files(gsm/gsm.h HAVE_GSM_GSM_H)

check_library_exists(ossaudio _oss_ioctl "" OSSAUDIO_LIB)
if(OSSAUDIO_LIB)
    set(AUDIO_LIBS ossaudio)
endif()

check_include_files(CoreAudio/CoreAudio.h OSX_AUDIO)
if(OSX_AUDIO)
    set(AUDIO_LIBS -framework CoreAudio)
endif()

check_library_exists(m sqrt "" MATH_LIB)
if(MATH_LIB)
    set(MATH_LIB m)
endif()

if(NOT HAVE_SPEEX_SPEEX_H AND EXISTS ${CMAKE_BINARY_DIR}/Frameworks/Speex)
    include_directories(${CMAKE_BINARY_DIR}/Frameworks/Speex/include)
    link_directories(${CMAKE_BINARY_DIR}/Frameworks/Speex/lib)
    set(HAVE_SPEEX_SPEEX_H 1)
endif()

if(HAVE_SPEEX_SPEEX_H)
    set(AUDIO_LIBS ${AUDIO_LIBS speex)
endif()

if(HAVE_GSM_H OR HAVE_GSM_GSM_H)
    set(AUDIO_LIBS ${AUDIO_LIBS} gsm)
endif()

if(LIB_SUFFIX)
    if (NOT LIB_SUFFIX EQUAL 64)
        message (FATAL_ERROR "LIB_SUFFIX must be empty (32 bits) or 64 (64 bits)")
    endif()
endif()

file(GLOB ccaudio_src src/*.cpp)
add_library(ccaudio ${BUILD_LIBRARY_TYPE} ${ccaudio_src})
set_target_properties(ccaudio PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
add_dependencies(ccaudio ucommon)
target_link_libraries(ccaudio ${USES_UCOMMON_LIBRARIES} ${AUDIO_LIBS} ${WITH_MATH_LIB} ${MATH_LIB})
install(FILES inc/ccaudio2.h DESTINATION include)
install(TARGETS ccaudio DESTINATION lib${LIB_SUFFIX})

add_executable(ccaudio-tonetool utils/tonetool.cpp)
add_dependencies(ccaudio-tonetool ccaudio ucommon)
set_target_properties(ccaudio-tonetool PROPERTIES OUTPUT_NAME tonetool)
target_link_libraries(ccaudio-tonetool ccaudio ${USES_UCOMMON_LIBRARIES})
install(TARGETS ccaudio-tonetool DESTINATION bin)

add_executable(ccaudio-audiotool utils/audiotool.cpp)
add_dependencies(ccaudio-audiotool ccaudio ucommon)
set_target_properties(ccaudio-audiotool PROPERTIES OUTPUT_NAME audiotool)
target_link_libraries(ccaudio-audiotool ccaudio ${USES_UCOMMON_LIBRARIES})
install(TARGETS ccaudio-audiotool DESTINATION bin)

add_executable(audiotool utils/audiotool.cpp)
add_dependencies(audiotool ccaudio ucommon)
target_link_libraries(audiotool ccaudio ${USES_UCOMMON_LIBRARIES})


