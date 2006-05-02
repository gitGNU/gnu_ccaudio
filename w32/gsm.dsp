# Microsoft Developer Studio Project File - Name="gsm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=gsm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gsm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gsm.mak" CFG="gsm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gsm - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gsm - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gsm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release/gsm"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GSM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\w32" /I "..\src" /I "..\gsm" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GSM_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.4 /dll /machine:I386 /out:"Release/gsm.rll"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug/gsm"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GSM_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\w32" /I "..\src" /I "..\gsm" /D "SASR" /D "USE_FLOAT_MUL" /D "FAST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GSM_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/gsm.rll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "gsm - Win32 Release"
# Name "gsm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\gsm\add.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\code.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\debug.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\decode.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\codecs\gsm.cpp

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\gsm_create.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\gsm_decode.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\gsm_destroy.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\gsm_encode.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\gsm_explode.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\gsm_implode.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\gsm_option.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\gsm_print.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\long_term.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\lpc.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\preprocess.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\rpe.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\short_term.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\gsm\table.c

!IF  "$(CFG)" == "gsm - Win32 Release"

!ELSEIF  "$(CFG)" == "gsm - Win32 Debug"

# ADD CPP /D "HAS_STDLIB_H"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\gsm\config.h
# End Source File
# Begin Source File

SOURCE=..\gsm\gsm.h
# End Source File
# Begin Source File

SOURCE=..\gsm\private.h
# End Source File
# Begin Source File

SOURCE=..\gsm\proto.h
# End Source File
# Begin Source File

SOURCE=..\gsm\toast.h
# End Source File
# Begin Source File

SOURCE=..\gsm\unproto.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
