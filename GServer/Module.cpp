// Module.cpp : implementation file
//

#include "stdafx.h"
#include "GServer.h"
#include "Module.h"
#include "DCXparser.h"		
#include "DCXDeviceMap.h"	// DCXparser.h must be include before this one


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModule

CModule::CModule()
{
}

CModule::~CModule()
{
}

/////////////////////////////////////////////////////////////////////////////
// CModule message handlers

UINT CModule::GetModuleAddress(UINT iModule)
{

// The card-cage addresses jump from one cage to another
// iModule is the module number (0-79)
//
// Return the DCX address given the module number

	if(iModule < 20)	// 0-19
		return(iModule + (DCX_START_ADDRESS) );

	if( (iModule >= 20) && (iModule < 40) )				// 20 -39
		return(iModule + (DCX_START_ADDRESS) + 12);

	if( (iModule >= 40) && (iModule < 60) )				// 40 - 59
		return(iModule + (DCX_START_ADDRESS) + 24);

	if( (iModule >= 60) && (iModule < DCX_DEVMAP_MAXSIZE) )
		return(iModule + (DCX_START_ADDRESS) + 36);

		return(0);
}

////////////////////////////////////////////////
// Returns 0-79 depending on DCX address iAddr

UINT CModule::GetModuleNumber(UINT iAddr)
{

	if( (iAddr >= DCX_START_ADDRESS) && (iAddr <= DCX_START_ADDRESS + 19) )
		return(iAddr - DCX_START_ADDRESS);

	if( (iAddr >= DCX_START_ADDRESS + 32) && (iAddr <= DCX_START_ADDRESS + 51) )
		return(iAddr - DCX_START_ADDRESS - 12);

	if( (iAddr >= DCX_START_ADDRESS + 64 ) && (iAddr <= DCX_START_ADDRESS + 83) )
		return(iAddr - DCX_START_ADDRESS - 24);

	if( (iAddr >= DCX_START_ADDRESS + 96) && (iAddr <= DCX_START_ADDRESS + 115) )
		return(iAddr - DCX_START_ADDRESS - 36);
	
	return(0);

}
