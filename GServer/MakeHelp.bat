@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by GSERVER.HPJ. >"hlp\GServer.hm"
echo. >>"hlp\GServer.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\GServer.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\GServer.hm"
echo. >>"hlp\GServer.hm"
echo // Prompts (IDP_*) >>"hlp\GServer.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\GServer.hm"
echo. >>"hlp\GServer.hm"
echo // Resources (IDR_*) >>"hlp\GServer.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\GServer.hm"
echo. >>"hlp\GServer.hm"
echo // Dialogs (IDD_*) >>"hlp\GServer.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\GServer.hm"
echo. >>"hlp\GServer.hm"
echo // Frame Controls (IDW_*) >>"hlp\GServer.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\GServer.hm"
REM -- Make help for Project GSERVER


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\GServer.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\GServer.hlp" goto :Error
if not exist "hlp\GServer.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\GServer.hlp" Debug
if exist Debug\nul copy "hlp\GServer.cnt" Debug
if exist Release\nul copy "hlp\GServer.hlp" Release
if exist Release\nul copy "hlp\GServer.cnt" Release
echo.
goto :done

:Error
echo hlp\GServer.hpj(1) : error: Problem encountered creating help file

:done
echo.
