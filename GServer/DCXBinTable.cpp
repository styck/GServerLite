//////////////////////////////////////////////////////////////////////
//
// Copyright CorTek Software, Inc. 1997-2004. All rights reserved.
//
// DCXBinTable.cpp: implementation of the CDCXBinTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Module.h"

#include "GServer.h"
#include "DCXBinTable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCXBinTable::CDCXBinTable()
{
	ZeroMemory((void *)&m_DcxMemMap, sizeof (DCX_MEM_MAP));

	m_DcxMemMap.pDcxMap = NULL;
	m_DcxMemMap.pDcxCtrls = NULL;

	m_iLastError = DBT_ERR_NOERROR;


	m_pDCXBuffer = NULL;

}

CDCXBinTable::~CDCXBinTable()
{
// Free the Buffers
//-----------------
	FreeDCXMapBuffers();
}

// Save/Load settings

void CDCXBinTable::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	// storing code
	}
	else
	{	// loading code
	}
}

//////////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: GetControlDesc(int iIdx, LPDCX_CTRL_DESC lpCtrlDesc)
//
// Copy the description of the control at index iIdx into the pointer passed
// get control information by index

BOOL    CDCXBinTable::GetControlDesc(int iIdx, LPDCX_CTRL_DESC lpCtrlDesc) 
{
BOOL    bRet = TRUE;

// is this a valid Control
//------------------------

	if(GetControlsCount() > iIdx)
	{
		CopyMemory((LPSTR)lpCtrlDesc, (LPSTR)m_DcxMemMap.pDcxMap[iIdx].pCtrlDesc, sizeof(DCX_CTRL_DESC));
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

////////////////////////////////////////
// MEMBER FUNCTION: GetControlDesc(LPSTR  lpsName ..
//
// for now this function will return FALSE !!!!
//
BOOL    CDCXBinTable::GetControlDesc(LPSTR  lpsName, LPDCX_CTRL_DESC lpdcxctrl)// get control information by name
{
BOOL    bRet = FALSE;

if(GetControlsCount())
{
  // Scan through control to find this Control Name
  //------------------------------------------------
}
else
{
  bRet = FALSE;
}

return bRet;
}

////////////////////////////////////////
// MEMBER FUNCTION: GetControlsCount(void)
//
// Return the TOTAL number of controls
//
int     CDCXBinTable::GetControlsCount(void)
{
int	iRet = 0;

  iRet = m_DcxMemMap.dcxHdr.iCtrlCount;

return iRet;
}

////////////////////////////////////////
// MEMBER FUNCTION: CompileDCXTable
//
// This function builds the DCX.BIN file
// from the various text control files

int     CDCXBinTable::CompileDCXTable(LPSTR szSourceDir, LPSTR szDest,BOOL bReverse)
{

DCX_CTRL_FILE_HDR dcxFileHeader = { 0 };
HANDLE            hfDest;
WIN32_FIND_DATA   ffData;
HANDLE            hNext;
LPDCX_CTRL_DESC   pCtrl;
int               iCtrlTableSize;
char              szFullName[MAX_PATH];
char              szSearchPath [ MAX_PATH];
DWORD             dwWrite;
int               iLine;
int               iMaxCtrlSteps = 0;
int               iCtrlCount = 0;

// Store those settings ...
//-------------------------

	if(strlen(szSourceDir) < 2)	// Make sure a source filename is given
		return 1;

	hfDest = CreateFile(szDest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hfDest == INVALID_HANDLE_VALUE )
		return 1;

	////////////////////////////////////////////////////////////
	// Write the File header

	if(WriteFile(hfDest, &dcxFileHeader, sizeof(DCX_CTRL_FILE_HDR), &dwWrite, NULL) == FALSE)
	{
		m_iLastError = DBT_ERR_CANT_WRITE_DCX_FILE; // ERROR! Couldn't write to DCX file !
		goto ON_EXIT;
	}

	wsprintf(szSearchPath, "%s\\*.dcx", szSourceDir);
	hNext = FindFirstFile(szSearchPath, &ffData);
	if(hNext == INVALID_HANDLE_VALUE)
		goto ON_EXIT;

	////////////////////////////////////////////////////////////
	// Loop thru the whole file

	do
	{
		wsprintf(szFullName, "%s\\%s", szSourceDir, ffData.cFileName);
		iLine = ReadDCXRawFile(szFullName, &pCtrl, &iCtrlTableSize, bReverse);

			// write the data to the Destination file
			//---------------------------------------
			if(pCtrl)
			{
				if(WriteFile(hfDest, (LPSTR)pCtrl, iCtrlTableSize, &dwWrite, NULL) == FALSE)
				{
					m_iLastError = DBT_ERR_CANT_WRITE_DEST_FILE; // Error writing to the Destination File !!!
					break;
				}

				if(iMaxCtrlSteps < pCtrl->iNumEntr)
					iMaxCtrlSteps = pCtrl->iNumEntr;
    
				iCtrlCount ++;    // keep track of how many controls were added

				GlobalFree((HANDLE)pCtrl);
				pCtrl = NULL;
				}
			else
			{
				m_iLastError = DBT_ERR_CANT_CONVERT_FILE; // Error Converting This File !!!
				break;
			}

	} while (FindNextFile(hNext, &ffData));


	// make sure the Search handle is closed ...
	//-------------------------------------------

	if(hNext != INVALID_HANDLE_VALUE)
		FindClose(hNext);

	if ( pCtrl )
		GlobalFree( (HANDLE) pCtrl);

	// Update the header for this
	// DCX file
	//---------------------------

	dcxFileHeader.dwID = DCX_TCP_ID; // the same as the TCP ID
	dcxFileHeader.dwVersion = DCX_FILE_VERSION;
	dcxFileHeader.iSize = sizeof(DCX_CTRL_FILE_HDR);
	dcxFileHeader.iCtrlCount = iCtrlCount;
	dcxFileHeader.iMaxCtrlSteps = iMaxCtrlSteps;
	SetFilePointer(hfDest, 0, NULL, FILE_BEGIN); // move to the begining of the file

	if(WriteFile(hfDest, &dcxFileHeader, sizeof(DCX_CTRL_FILE_HDR), &dwWrite, NULL) == FALSE)
	{
		m_iLastError = DBT_ERR_CANT_WRITE_DCX_FILE; // ERROR! Couldn't write to DCX file !
		goto ON_EXIT;
	}

	ON_EXIT:
	if(hfDest != INVALID_HANDLE_VALUE)
		CloseHandle(hfDest);

return 0;
};

///////////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: FreeDCXMapBuffers
//
// Free the DCX Buffers
//
int     CDCXBinTable::FreeDCXMapBuffers(void)
{

	if(m_DcxMemMap.pDcxCtrls)
	{
		GlobalFree((HANDLE)m_DcxMemMap.pDcxCtrls);
		m_DcxMemMap.pDcxCtrls = NULL;
	}

	if(m_DcxMemMap.pDcxMap)
	{
		GlobalFree((HANDLE)m_DcxMemMap.pDcxMap);
		m_DcxMemMap.pDcxMap = NULL;
	}

	ZeroMemory(&m_DcxMemMap.dcxHdr, sizeof(DCX_CTRL_FILE_HDR));

return 0;
} 

////////////////////////////////////////////////////////////////////////
// FUNCTION: ReadDCXTableFile
//
//
int   CDCXBinTable::ReadDCXTableFile(LPSTR  lpFName)
{
	int                 iRet = FALSE;			// ERROR by default
	LPDCX_CTRL_DESC     pdcxctrl;					// buffer were the controls data will be stored
	LPDCX_CTRL_DESC     pwalk = NULL;			// buffer were the controls data will be stored
	LPDCX_MAP_ENTRY     pdcxmap = NULL;		// the lookup table for the controls position in memory
	LPSTR               pend;
	DCX_CTRL_FILE_HDR   dcxFileHeader = { 0 };
	HANDLE              hf;
	DWORD               dwRead;
	int                 iCtrlCount;
	DWORD               dwSize;

	// Clear everything
	//------------------

	FreeDCXMapBuffers();

	// Open DCX.BIN file
	hf = CreateFile(lpFName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hf == INVALID_HANDLE_VALUE )
		goto ON_EXIT;

	// Validate the file size
	dwSize = GetFileSize(hf, NULL);

	if(( dwSize == 0 ) || ( dwSize == 0xFFFFFFFF))
		goto ON_EXIT;

	// Read in the DCX_CTRL_FILE_HDR
	SetFilePointer(hf, 0, NULL, FILE_BEGIN);
	::ReadFile(hf, &dcxFileHeader, sizeof(DCX_CTRL_FILE_HDR), &dwRead, NULL);

	if(dwRead != sizeof(DCX_CTRL_FILE_HDR))
		goto ON_EXIT;

	// Verify the DCX_CTRL_FILE_HDR
	//------------------------------
	if( (dcxFileHeader.dwID != DCX_TCP_ID) || (dcxFileHeader.iSize != sizeof(DCX_CTRL_FILE_HDR)) )
		goto ON_EXIT;

	// allocate the buffer to hold the File data
	//------------------------------------------

	dwSize -= sizeof(DCX_CTRL_FILE_HDR);
	pdcxctrl = (LPDCX_CTRL_DESC)GlobalAlloc(GPTR, dwSize); // allocate a buffer to hold the rest of the file data
	if(pdcxctrl == NULL)
		goto ON_EXIT;

	// now read the Entire file in memory
	//-----------------------------------

	::ReadFile(hf, pdcxctrl, dwSize, &dwRead, NULL);
	if(dwRead != dwSize)
		goto ON_EXIT;

	pwalk = pdcxctrl;
	pend = (LPSTR)pdcxctrl + dwSize;

	// allocate the index of the controls
	//-----------------------------------

	pdcxmap = (LPDCX_MAP_ENTRY)GlobalAlloc(GPTR, sizeof(DCX_MAP_ENTRY) * dcxFileHeader.iCtrlCount);
	if(pdcxmap == NULL) 
		goto ON_EXIT;

	// loop to read all of the controls and load them into memory ...
	//---------------------------------------------------------------
	iCtrlCount = 0;
	while( (LPSTR)pwalk < pend)
	{
		pdcxmap[iCtrlCount].iCtrlID   = pwalk->iCtrlID;
		pdcxmap[iCtrlCount].pCtrlDesc = pwalk; // store the pointer for the memory location of this control descriptor
		iCtrlCount++;
		pwalk = (LPDCX_CTRL_DESC)((LPSTR)pwalk + pwalk->iSize);
	}

	if( iCtrlCount == dcxFileHeader.iCtrlCount)
	{
		// store the header information and the buffer pointers
		// so that the control data can be found
		//-----------------------------------------------------
		m_DcxMemMap.dcxHdr     = dcxFileHeader;
		m_DcxMemMap.pDcxCtrls  = pdcxctrl;	// Pointer to the control description,
		m_DcxMemMap.pDcxMap    = pdcxmap;
		iRet = TRUE;						// Everything ok
	}
	else
	{
		// the controls read is not the same as it's specified in the File Header
		// handle this ERROR by returning FALSE
		//-----------------------------------------------------------------------
		iRet = FALSE;
	}


	ON_EXIT:

	// Free the buffers if an error occured
	// and they were already allocated
	//-------------------------------------
	if(!iRet)
	{
		if(pdcxctrl)  
			GlobalFree( (HANDLE) pdcxctrl);
		if(pdcxmap)
			GlobalFree( (HANDLE) pdcxmap);
	}

	if(hf != INVALID_HANDLE_VALUE)
		CloseHandle(hf);

return iRet;
};


////////////////////////////////////////////////
// MEMBER FUNCTION: RemapControlData
//
// return: 
//        FALSE  = Error
//        TRUE   = OK
//
// NOTE:   This function remaps the DCX control data
//  All pot values and strings are stored in the
//  DCX_POT_CTRLDATA data structure which is part of the
//  DCX_CTRLDATA.
//

BOOL     CDCXBinTable::RemapControlData(int iChannel, int iCtrl, int iVal, DCX_CTRLDATA  *pctrldata)
{
	CONTROLDATA ctrld;

	ctrld.wChannel = (WORD)iChannel;
	ctrld.wCtrl    = (WORD)iCtrl;
	ctrld.wVal     =  (WORD)iVal;

	return RemapControlData(&ctrld, pctrldata);
}


BOOL     CDCXBinTable::RemapControlData(CONTROLDATA * pctrld, DCX_CTRLDATA  *pctrldata)
{
	BOOL                bRet = FALSE;
	LPDCXCTRLENTRY      pctrlEntry;
	LPDCX_CTRL_DESC     pCtrlDesc;
	int                 i, iAddr;
	char                chBuff[80];
	CModule							cm_Module;		// Converts module number to DCX address

// do remaping and string manipulation here
//-----------------------------------------

	if(pctrld->wCtrl < m_DcxMemMap.dcxHdr.iCtrlCount)
	{
		pCtrlDesc = m_DcxMemMap.pDcxMap[pctrld->wCtrl].pCtrlDesc; // obtain the pointer to where in memory is this control information

		if(pctrld->wVal < pCtrlDesc->iNumEntr)
		{
			pctrlEntry = (LPDCXCTRLENTRY)((LPSTR)pCtrlDesc + sizeof(DCX_CTRL_DESC));    
			pctrlEntry += pctrld->wVal;

			// now loop on this controls pan pots and send the data out :-)))) simple stuff
			//-----------------------------------------------------------------------------

			pctrldata->iPotCount = pCtrlDesc->iNumPots;

			// Address is the Channel address sent from the CLIENT + channel address offset
			// that is set on the SERVER
			// Usually one side will need to set this to zero.

			// The card-cage addresses jump from one cage to another
			//
			// Client sends the channel number as 0-79

			// Channel number is base 0
			// Lookup the DCX addresss of this module

			iAddr = cm_Module.GetModuleAddress(pctrld->wChannel);

			ZeroMemory(chBuff, 80);
			lstrcpy(pctrldata->szRdOut, pctrlEntry->szRdOut);

			for(i = 0; i < pCtrlDesc->iNumPots; i++)
			{
				pctrldata->arPotData[i].iAddr = iAddr;
				// '*', Address 3, Command 3, Chip 2, pot 1, value 3, newline
				//------------------------------------------------------------
				wsprintf(chBuff, "*%03dXXW%s%s%s\n", iAddr,
																						 pCtrlDesc->achChip[i],
																						 pCtrlDesc->achPot[i],
																						 pctrlEntry->achPotVal[i]);
				lstrcpy(pctrldata->arPotData[i].szData, chBuff);
			}

			bRet = TRUE;
		}
	}

return bRet;
}


///////////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION:  ReadDCXRawFile
//
// This function is used to parse the raw control descriptor files when
// compiling the dcx.bin file.
//
// NOTE:   Do not modify unless You know what You are doing.
//

int     CDCXBinTable::ReadDCXRawFile(LPSTR lpRawFileName, LPDCX_CTRL_DESC *ppCtrl, int  *piSize, BOOL bReverse)
{
int             iRet;
int             i, ii, iLinesCount, iPotVal, iChip, iPot;
int             iLineCount = 0;
HANDLE          hf;
LPSTR           lpbuff;
LPSTR           lpWalk;
LPSTR           lpDelim;
LPSTR           lpCur;
LPSTR           lpEnd;
DWORD           dwSize;
DWORD           dwRead;
DCX_CTRL_DESC   dcxCtrl= { 0 }; // a temporary storage place
LPDCX_CTRL_DESC lpTable;
LPDCXCTRLENTRY  lpEntry;
LPDCXCTRLENTRY  lpEntry2;
DCXCTRLENTRY    dcxEntry;
char            szLine[512];
char            szREG0[24];

	iRet = 0xFFFFFFFFL; // Error

	*ppCtrl = NULL;

	hf = CreateFile(lpRawFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hf != INVALID_HANDLE_VALUE )
	{
		// allocate memory to read the entire file
		//-----------------------------------------
		dwSize = GetFileSize(hf, NULL);
		if((dwSize == 0) || (dwSize == 0xFFFFFFFF))
			goto ON_EXIT;
    
		lpbuff = (char *)GlobalAlloc(GPTR, dwSize);
		if( lpbuff == NULL)
			goto ON_EXIT;

		SetFilePointer(hf, 0, NULL, FILE_BEGIN);
		if( ::ReadFile(hf, lpbuff, dwSize, &dwRead, NULL) == FALSE)
			goto ON_EXIT;

		if( dwSize != dwRead )
			goto ON_EXIT;

		// start looping through the memory and parse the Strings  OUT
		//------------------------------------------------------------
		lpWalk = lpbuff;
		lpEnd  = lpbuff + dwSize;

  
		GET_NEXT_LINE; 
		if(szLine[0] == 0)
			goto ON_EXIT;

		// Get the name of the Control
		//----------------------------
		i = strlen(szLine);
		if(i > (sizeof(dcxCtrl.szName) - 1) )
			i = sizeof(dcxCtrl.szName) - 1;

		CopyMemory(dcxCtrl.szName, szLine, i); // we've got the name
 

		// skip the File Name
		//-------------------
		GET_NEXT_LINE; iLineCount ++;
		if(szLine[0] == 0)
			goto ON_EXIT;

		GET_NEXT_LINE; iLineCount ++;

		// Get the REG0 value ... as a string and latter we are going to find this string
		// into the possible values and pick only the offset to this string    
		//-------------------------------------------------------------------------------
		lpDelim = strchr(szLine, TABCHAR);

		if(lpDelim == NULL)
			goto ON_EXIT;

		lpDelim +=  1;

		lpCur = lpDelim;
		lpDelim = strchr(lpDelim, TABCHAR);

		if(lpDelim == NULL)
		{
			// this is a fix for the ones that do not use the TAB character as delimiter
			// for the REG0
			//--------------------------------------------------------------------------
			lpDelim = strchr(lpCur, ' '); // search for the first blank space
			if(lpDelim == NULL)
				goto ON_EXIT;
		}
		else
			if(lpDelim > strchr(lpCur, ' '))
				lpDelim = strchr(lpCur, ' '); // search for the first blank space

		if( (lpDelim - lpCur) > 23 )
			goto ON_EXIT;

		ZeroMemory(szREG0, 24);  
		CopyMemory(szREG0, lpCur, lpDelim - lpCur); // good now we have the REG0 string 

		// Now lets get the Number of pots used for this control
		//------------------------------------------------------

		GET_NEXT_LINE; iLineCount  ++;

		lpDelim = strchr(szLine, TABCHAR);

		if(lpDelim == NULL)
			goto ON_EXIT;

		*lpDelim = 0;
		dcxCtrl.iNumPots = atoi(szLine);

		if( dcxCtrl.iNumPots < 1 || dcxCtrl.iNumPots > 6 )
			goto ON_EXIT;


		// grab the pot values

		for(i = 0; i < dcxCtrl.iNumPots; i++)
		{
			GET_NEXT_LINE; iLineCount  ++;

			// get the chip number 
			lpDelim = strchr(szLine, TABCHAR);
			if(lpDelim == NULL)
				goto ON_EXIT;

			*lpDelim = 0;

			iChip = atoi(szLine);
			if( (iChip < 0 ) || (iChip > 99))
				goto ON_EXIT;

			wsprintf(dcxCtrl.achChip[i], "%02d", iChip);

			lpDelim ++;
			lpCur = lpDelim;

			// get the pot 
			//------------
			lpDelim = strchr(lpCur, TABCHAR);
			if(lpDelim == NULL)
				goto ON_EXIT;

			*lpDelim = 0;

			iPot = atoi(lpCur);
			if( iPot < 0 || iPot > 3 )
				goto ON_EXIT;
			wsprintf(dcxCtrl.achPot[i], "%01d", iPot);

		}

		// ready to start getting the rest of the data
		// these should be the actual values for the pots
		// and the readouts
		//-----------------------------------------------

		// First lets count how many lines are available here

		lpDelim = lpWalk;
		lpCur = lpWalk; // save the currant Walk position
		iLinesCount = 0;

		GET_NEXT_LINE; // do not update the iLineCount here since we are going to go back ...

		while ( szLine[0] != 0)
		{
			iLinesCount ++;
			GET_NEXT_LINE;
		}

		if(iLinesCount < 1)  
			goto ON_EXIT;

		lpWalk = lpCur; // restorte the Walk position

		lpTable = (LPDCX_CTRL_DESC)GlobalAlloc(GPTR, (sizeof(DCXCTRLENTRY) * iLinesCount) + sizeof(DCX_CTRL_DESC));
		if( lpTable == NULL)
			goto ON_EXIT;


		*lpTable = dcxCtrl;
		lpTable->iSize = (sizeof(DCXCTRLENTRY) * iLinesCount) + sizeof(DCX_CTRL_DESC);
		lpTable->iNumEntr = iLinesCount;

		lpEntry = (LPDCXCTRLENTRY)((LPSTR)lpTable + sizeof(DCX_CTRL_DESC));

		for( i = 0; i < iLinesCount; i++)
		{
			GET_NEXT_LINE; iLineCount ++;
			if( szLine[0] == 0)
				break;

			// first get the readout
			lpDelim = strchr(szLine, TABCHAR);
			if(lpDelim == NULL)
				goto ON_EXIT;

			*lpDelim = 0;
			if( strlen(szLine)> 15)
				goto ON_EXIT;
    
			CopyMemory(lpEntry->szRdOut, szLine, strlen(szLine));
    
			lpDelim ++;
			lpCur = lpDelim;

			// copy and format the pot string values
			for( ii = 0; ii < dcxCtrl.iNumPots; ii++)
			{
				lpDelim = strchr(lpCur, TABCHAR);

				if(lpDelim != NULL)
					*lpDelim = 0;
				else
					if( *lpCur == 0)
						goto ON_EXIT;

				iPotVal = atoi(lpCur);
				if( (iPotVal < 0) || (iPotVal > 255) )
					goto ON_EXIT;
				// format the pot value
				wsprintf(lpEntry->achPotVal[ii], "%03d", iPotVal);

				lpDelim ++;
				lpCur = lpDelim;
			}

			if( (ii + 1) < dcxCtrl.iNumPots)
				goto ON_EXIT; // error didn't find all of the pot values

			lpEntry ++; // move the Entry Pointer ahead

		}

		if( (i + 1) < iLinesCount)
			goto ON_EXIT; // error didn't locate all of the Entries

		// reverse the table if needed 
		//----------------------------
		if(bReverse)  
		{
			lpEntry = (LPDCXCTRLENTRY)((LPSTR)lpTable + sizeof(DCX_CTRL_DESC));
			lpEntry2 = lpEntry + iLinesCount - 1;
			for( i = 0; i < iLinesCount/2; i++)
			{
				dcxEntry = *lpEntry;
				*lpEntry = *lpEntry2;
				*lpEntry2 = dcxEntry;
				lpEntry ++;
				lpEntry2 --;
			}
		}

		// Very cool we are almost done here
		// just need to locate the REG0 entry
		//-----------------------------------
		lpEntry = (LPDCXCTRLENTRY)((LPSTR)lpTable + sizeof(DCX_CTRL_DESC));
		for( i = 0; i < iLinesCount; i++ )
		{
			if( ! strcmp(szREG0, lpEntry->szRdOut) )
			{
				lpTable-> iREG0 = i;
				break;
			}      
			lpEntry ++;
		}

		if(i == iLinesCount)
		{
			iLineCount = -10;
			goto ON_EXIT; // couldn't locate the REG0 value
		}

		*ppCtrl = lpTable;
		*piSize = (sizeof(DCXCTRLENTRY) * iLinesCount) + sizeof(DCX_CTRL_DESC);
		iRet = 0;
	}

	ON_EXIT:

	if( hf != INVALID_HANDLE_VALUE )
		CloseHandle( hf );

	if( lpbuff )
		GlobalFree( (HANDLE)lpbuff );

	if(iRet)
		{
		iRet = iLineCount; // return the line at which the Error occured
		if(lpTable)
			{
			GlobalFree( (HANDLE) lpTable);
			*ppCtrl = NULL;
			*piSize = 0;
			}
		}
	return iRet;
}
