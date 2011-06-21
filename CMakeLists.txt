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
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
set (VERSION 2.0.5)
set (SOVERSION 2)

set(INSTALL_INCLUDEDIR include)
set(INSTALL_BINDIR bin)

if(WIN32)
    set(MODULE_EXT .mll)
    set(INSTALL_MANDIR man)
    set(INSTALL_DOCDIR doc/${PROJECT_NAME})
    set(INSTALL_LOCALE locale)
    set(INSTALL_LIBDIR bin)
    set(INSTALL_INFODIR info)
    set(INSTALL_SBINDIR bin)
    set(SYSCONFDIR "etc" CACHE PATH "config directory")
    set(STATEDIR "var" CACHE PATH "state directory")
else()
    set(MODULE_EXT .so)
    set(INSTALL_MANDIR share/man)
    set(INSTALL_LOCALE share/locale)
    set(INSTALL_DOCDIR share/doc/${PROJECT_NAME})
    set(INSTALL_INFODIR share/info)
    set(INSTALL_LIBDIR lib${LIB_POSTFIX})
    set(INSTALL_SBINDIR sbin)
    set(SYSCONFDIR "/etc" CACHE PATH "config directory")
    set(STATEDIR "/var" CACHE PATH "state directory")
endif()

set(UCOMMON_LOCALE ${CMAKE_INSTALL_PREFIX}/${INSTALL_LOCALE})

if(WIN32)
    set(INSTALL_PLUGINS ccaudio)
    set(DEFAULT_CONFIGDIR "${CMAKE_INSTALL_PREFIX}/${SYSCONFDIR}")
    set(DEFAULT_STATEDIR "${CMAKE_INSTALL_PREFIX}/${STATEDIR}")
else()
    set(INSTALL_PLUGINS ${INSTALL_LIBDIR}/ccaudio)
    set(DEFAULT_CONFIGDIR ${SYSCONFDIR})
    set(DEFAULT_STATEDIR ${STATEDIR})
endif()

set(DEFAULT_LIBPATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBDIR})

if(MSVC60)
    set(BUILD_STATIC ON CACHE BOOL "static linking only" FORCE)
    MARK_AS_ADVANCED(FORCE BUILD_STATIC)
elseif(WIN32)
    option(BUILD_STATIC "Set to OFF to build shared libraries" ON)
else()
    option(BUILD_STATIC "Set to ON to build static libraries" OFF)
endif()

MESSAGE( STATUS "Configuring GNU ccaudio ${VERSION}...")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} library")
SET(CPACK_PACKAGE_VENDOR              "David Sugar")
SET(CPACK_PACKAGE_DESCRIPTION_FILE    "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(PACKAGE_FILE_NAME                 ${PROJECT_NAME})
set(PACKAGE_FILE_VERSION              ${VERSION})

IF (WIN32)
    SET(CPACK_PACKAGE_FILE_NAME "${PACKAGE_FILE_NAME}-${PACKAGE_FILE_VERSION}")
ELSE (WIN32)
    SET(CPACK_PACKAGE_FILE_NAME "${PACKAGE_FILE_NAME}-bin-${PACKAGE_FILE_VERSION}")
    if (NOT CPACK_GENERATOR)
        SET(CPACK_GENERATOR "TBZ2")
    endif()
ENDIF (WIN32)

IF(WIN32 AND NOT UNIX)
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_FILE_NAME}")

    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    # SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
    # SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
    SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} ${PROJECT_NAME}$")
    SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.gnutelephony.org")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.gnutelephony.org")
    SET(CPACK_NSIS_CONTACT "dyfet@gnutelephony.org")
    SET(CPACK_NSIS_MODIFY_PATH ON)

ENDIF(WIN32 AND NOT UNIX)

set(CPACK_COMPONENTS_ALL libraries headers)
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME   "C++ Headers")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION  "Dynamic library")
set(CPACK_COMPONENT_HEADERS_DESCRIPTION
    "Header files needed to build applications using ucommon library")
SET(CPACK_SOURCE_IGNORE_FILES ".bzr" "build" ".swp$" ".*~" ".svn" ".git")
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE_FILE_NAME}-${PACKAGE_FILE_VERSION}")

# This must always be last!
INCLUDE(CPack)

# set to true for debug and trace during CMakeLists development
set(CMAKE_VERBOSE_MAKEFILE FALSE)

# Set defaults and pass common options.  Common build options can be passed
# to cmake using cmake -DWITH_CFLAGS="...", WITH_LIBS, and WITH_INCLUDES
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc ${WITH_INCLUDES})
add_definitions(${WITH_CFLAGS})
link_libraries(${WITH_LIBS})

if (USES_UCOMMON_INCLUDE_DIRS)
    message(STATUS "  Using local ucommon dependency")
else()
    find_package(PkgConfig)
    pkg_check_modules(USES_UCOMMON REQUIRED ucommon>=4.1.0)
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


check_include_files(sys/soundcard.h HAVE_SYS_SOUNDCARD_H)
check_include_files(speex/speex.h HAVE_SPEEX_SPEEX_H)
check_include_files(gsm.h HAVE_GSM_H)
check_include_files(gsm/gsm.h HAVE_GSM_GSM_H)
check_include_files(endian.h HAVE_ENDIAN_H)

check_library_exists(ossaudio _oss_ioctl "" OSSAUDIO_LIB)
if(OSSAUDIO_LIB)
    set(AUDIO_LIBS ossaudio)
endif()

check_include_files(CoreAudio/CoreAudio.h OSX_AUDIO)
if(OSX_AUDIO)
    set(AUDIO_LIBS "-framework CoreAudio")
endif()

if(WIN32)
    set(AUDIO_LIBS winmm msacm32)
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
    set(AUDIO_LIBS ${AUDIO_LIBS} speex)
endif()

if(HAVE_GSM_H OR HAVE_GSM_GSM_H)
    set(AUDIO_LIBS ${AUDIO_LIBS} gsm)
endif()

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

file(GLOB ccaudio_src src/*.cpp)
set(ccaudio_inc inc/ccaudio2.h)
add_library(ccaudio ${BUILD_LIBRARY_TYPE} ${ccaudio_src} ${ccaudio_inc})
set_target_properties(ccaudio PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
add_dependencies(ccaudio ucommon)
target_link_libraries(ccaudio ${USES_UCOMMON_LIBRARIES} ${AUDIO_LIBS} ${WITH_MATH_LIB} ${MATH_LIB})

add_executable(ccaudio-tonetool utils/tonetool.cpp)
add_dependencies(ccaudio-tonetool ccaudio ucommon)
set_target_properties(ccaudio-tonetool PROPERTIES OUTPUT_NAME tonetool)
target_link_libraries(ccaudio-tonetool ccaudio ${USES_UCOMMON_LIBRARIES})

add_executable(ccaudio-audiotool utils/audiotool.cpp)
add_dependencies(ccaudio-audiotool ccaudio ucommon)
set_target_properties(ccaudio-audiotool PROPERTIES OUTPUT_NAME audiotool)
target_link_libraries(ccaudio-audiotool ccaudio ${USES_UCOMMON_LIBRARIES})

add_executable(ccaudio-phrasebook utils/phrasebook.cpp)
add_dependencies(ccaudio-phrasebook ccaudio ucommon)
set_target_properties(ccaudio-phrasebook PROPERTIES OUTPUT_NAME phrasebook)
target_link_libraries(ccaudio-phrasebook ccaudio ${USES_UCOMMON_LIBRARIES})

install(FILES inc/ccaudio2.h DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT headers)
install(TARGETS ccaudio DESTINATION ${INSTALL_LIBDIR} COMPONENT libraries)
install(TARGETS ccaudio-tonetool ccaudio-audiotool ccaudio-phrasebook DESTINATION ${INSTALL_BINDIR})

