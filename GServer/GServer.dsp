# Microsoft Developer Studio Project File - Name="GServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GServer.mak" CFG="GServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/GServer", BAAAAAAA"
# PROP Scc_LocalPath "."
# PROP WCE_FormatVersion ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\vacs\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 uuid.lib winmm.lib version.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\vacs\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 uuid.lib winmm.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "GServer - Win32 Release"
# Name "GServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ControlDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CorTekAsyncSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\CorTekTBCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\cputicker.cpp
# End Source File
# Begin Source File

SOURCE=.\DCXBinTable.cpp
# End Source File
# Begin Source File

SOURCE=.\DCXDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\DCXDeviceMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DCXLoadReg0Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\DCXRegistryEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\GDCXNetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\GServer.cpp
# End Source File
# Begin Source File

SOURCE=.\GServer.rc
# End Source File
# Begin Source File

SOURCE=.\GServerDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\GServerView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Module.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VUMeterArray.cpp
# End Source File
# Begin Source File

SOURCE=.\vuthread2.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ControlDlg.h
# End Source File
# Begin Source File

SOURCE=.\CorTekAsyncSocket.h
# End Source File
# Begin Source File

SOURCE=.\CorTekTBCtrl.h
# End Source File
# Begin Source File

SOURCE=.\cputicker.h
# End Source File
# Begin Source File

SOURCE=.\DCXBinTable.h
# End Source File
# Begin Source File

SOURCE=.\DCXDevice.h
# End Source File
# Begin Source File

SOURCE=.\DCXDeviceMap.h
# End Source File
# Begin Source File

SOURCE=.\DCXLoadReg0Dialog.h
# End Source File
# Begin Source File

SOURCE=.\DCXparser.h
# End Source File
# Begin Source File

SOURCE=.\DCXRegistryEdit.h
# End Source File
# Begin Source File

SOURCE=.\GDCXNetwork.h
# End Source File
# Begin Source File

SOURCE=.\GServer.h
# End Source File
# Begin Source File

SOURCE=.\GServerDoc.h
# End Source File
# Begin Source File

SOURCE=.\GServerView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Module.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VUMeterArray.h
# End Source File
# Begin Source File

SOURCE=.\VUthread2.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\auxSelection.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bvu_input.bmp
# End Source File
# Begin Source File

SOURCE=.\res\circuit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ComputerModule.bmp
# End Source File
# Begin Source File

SOURCE=".\res\CorTek260x89@72ppi-rgb-indexedcolor.BMP"
# End Source File
# Begin Source File

SOURCE=.\res\CorTekLogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cue.bmp
# End Source File
# Begin Source File

SOURCE=.\res\GServer.ico
# End Source File
# Begin Source File

SOURCE=.\res\GServer.rc2
# End Source File
# Begin Source File

SOURCE=.\res\GServerDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\inputSelection.bmp
# End Source File
# Begin Source File

SOURCE=.\res\masterSelection.bmp
# End Source File
# Begin Source File

SOURCE=.\res\matrix.bmp
# End Source File
# Begin Source File

SOURCE=.\res\module_a.bmp
# End Source File
# Begin Source File

SOURCE=.\res\module_b.bmp
# End Source File
# Begin Source File

SOURCE=.\res\module_basemap.bmp
# End Source File
# Begin Source File

SOURCE=.\res\module_c.bmp
# End Source File
# Begin Source File

SOURCE=.\res\module_disabled.bmp
# End Source File
# Begin Source File

SOURCE=.\res\module_i.bmp
# End Source File
# Begin Source File

SOURCE=.\res\module_m.bmp
# End Source File
# Begin Source File

SOURCE=.\res\noSelection.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ppwr_in.bmp
# End Source File
# Begin Source File

SOURCE=.\res\softblu2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\verde.bmp
# End Source File
# Begin Source File

SOURCE=.\res\watermark.bmp
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\AfxPrint.rtf

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\GServer.cnt

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp

!IF  "$(CFG)" == "GServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GServer - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\deleteMe.txt
# End Source File
# Begin Source File

SOURCE=.\GServer.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section GServer : {00000000-0000-0000-0000-000000000000}
# 	1:10:IDB_SPLASH:102
# 	2:21:SplashScreenInsertKey:4.0
# End Section
# Section GServer : {975C6675-648B-11D3-850E-006008BD93E7}
# 	1:14:IDD_REG0DIALOG:108
# 	2:16:Resource Include:resource.h
# 	2:14:IDD_REG0DIALOG:IDD_REG0DIALOG
# 	2:10:ENUM: enum:enum
# 	2:19:DCXLoadReg0Dialog.h:DCXLoadReg0Dialog.h
# 	2:24:CLASS: DCXLoadReg0Dialog:DCXLoadReg0Dialog
# 	2:21:DCXLoadReg0Dialog.cpp:DCXLoadReg0Dialog.cpp
# 	2:19:Application Include:gserver.h
# End Section
