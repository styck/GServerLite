// DCXRegistryEdit.cpp: implementation of the CDCXRegistryEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DCXRegistryEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCXRegistryEdit::CDCXRegistryEdit()
{

}

CDCXRegistryEdit::~CDCXRegistryEdit()
{

}

BOOL CDCXRegistryEdit::GetDCXDelayData(DWORD * m_dwBasedelay, DWORD * m_dwVudelay, DWORD * m_dwCtrldelay)
{

		static LPCTSTR szRegKey = "Software\\CorTek\\GSERVER\\1.00"; 
		static LPCTSTR szRegValue; 

		// open/create the DCX GSERVER registry Key 
		HKEY hKey; 
		DWORD dwDisposition; 
		if ( ::RegCreateKeyEx(
					HKEY_LOCAL_MACHINE, 
					szRegKey, 
					0, 
					REG_NONE, 
					0, 
					KEY_ALL_ACCESS, 
					NULL, 
					&hKey, 
					&dwDisposition ) != ERROR_SUCCESS ) 
		{ 
			TRACE( "Couldn't create/open key in registry\n" ); 
			return FALSE; 
		}  

		DWORD dwType; 
		DWORD dwValue; 
		DWORD dwBufferSize = sizeof( dwValue );  

	// get the DCX base delay from the registry
		szRegValue = "BaseDelay";
		if ( ::RegQueryValueEx( 
					hKey, 
					szRegValue, 
					NULL, 
					&dwType, 
					reinterpret_cast<BYTE*>(&dwValue), 
					&dwBufferSize ) != ERROR_SUCCESS ) 
		{ 
			TRACE( "No delay set, setting default values\n" ); 
			dwValue = 90000;
			::RegSetValueEx( hKey, 
							szRegValue, 
							0, 
							REG_DWORD, 
							reinterpret_cast<BYTE*>(&dwValue), 
							sizeof( DWORD ) ); 

		}

		* m_dwBasedelay = dwValue; // copy registry value


	// get the DCX VU delay from the registry
		szRegValue = "VuDelay";
		if ( ::RegQueryValueEx( 
					hKey, 
					szRegValue, 
					NULL, 
					&dwType, 
					reinterpret_cast<BYTE*>(&dwValue), 
					&dwBufferSize ) != ERROR_SUCCESS ) 
		{ 
			TRACE( "No delay set, setting default values\n" ); 
			dwValue = 4;
			::RegSetValueEx( hKey, 
							szRegValue, 
							0, 
							REG_DWORD, 
							reinterpret_cast<BYTE*>(&dwValue), 
							sizeof( DWORD ) ); 

		}

		* m_dwVudelay = dwValue; // copy registry value


	// get the DCX Control delay from the registry
		szRegValue = "CtrlDelay";
		if ( ::RegQueryValueEx( 
					hKey, 
					szRegValue, 
					NULL, 
					&dwType, 
					reinterpret_cast<BYTE*>(&dwValue), 
					&dwBufferSize ) != ERROR_SUCCESS ) 
		{ 
			TRACE( "No delay set, setting default values\n" ); 
			dwValue = 6;
			::RegSetValueEx( hKey, 
							szRegValue, 
							0, 
							REG_DWORD, 
							reinterpret_cast<BYTE*>(&dwValue), 
							sizeof( DWORD ) ); 

		}

		* m_dwCtrldelay = dwValue; // copy registry value

		RegCloseKey(hKey); // close the registry
		return TRUE;

}

BOOL CDCXRegistryEdit::SetDCXDelayData(DWORD * m_dwBasedelay, DWORD * m_dwVudelay, DWORD * m_dwCtrldelay)
{

		static LPCTSTR szRegKey = "Software\\CorTek\\GSERVER\\1.00"; 
		static LPCTSTR szRegValue; 

		// open/create the DCX GSERVER registry Key 
		HKEY hKey; 
		DWORD dwDisposition; 
		if ( ::RegCreateKeyEx(
					HKEY_LOCAL_MACHINE, 
					szRegKey, 
					0, 
					REG_NONE, 
					0, 
					KEY_ALL_ACCESS, 
					NULL, 
					&hKey, 
					&dwDisposition ) != ERROR_SUCCESS ) 
		{ 
			TRACE( "Couldn't create/open key in registry\n" ); 
			return FALSE; 
		}  

		//DWORD dwType; 
		DWORD dwValue; 
		DWORD dwBufferSize = sizeof( dwValue );  

		// load base delay into registry
		szRegValue = "BaseDelay";
		dwValue= * m_dwBasedelay;

		::RegSetValueEx( hKey, 
						szRegValue, 
						0, 
						REG_DWORD, 
						reinterpret_cast<BYTE*>(&dwValue), 
						sizeof( DWORD ) ); 

		// load VU delay into registry
		szRegValue = "VuDelay";
		dwValue	= * m_dwVudelay;

		::RegSetValueEx( hKey, 
						szRegValue, 
						0, 
						REG_DWORD, 
						reinterpret_cast<BYTE*>(&dwValue), 
						sizeof( DWORD ) ); 

		// load control delay into registry
		szRegValue = "CtrlDelay";
		dwValue = * m_dwCtrldelay;

		::RegSetValueEx( hKey, 
						szRegValue, 
						0, 
						REG_DWORD, 
						reinterpret_cast<BYTE*>(&dwValue), 
						sizeof( DWORD ) ); 

		RegCloseKey(hKey); // close the registry
		return TRUE;

}
