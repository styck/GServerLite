// DCXBinTable.h: interface for the CDCXBinTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCXBINTABLE_H__129D671B_661E_11D1_A3B1_C4D3CD000000__INCLUDED_)
#define AFX_DCXBINTABLE_H__129D671B_661E_11D1_A3B1_C4D3CD000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "mmsystem.h" // for MMTIME structure ....

#include  "dcxtcp_protocol.h"	// definition of the TCP protocol messages, headers ...
#include "DCXParser.h"				// general stuff for the parsing routines for the DCX


typedef struct DCX_POT_CTRLDATA_tag
  {
  int   iAddr;
  char  szData[32];
  }DCX_POT_CTRLDATA;

typedef struct DCX_CTRLDATA_tag
  {
  int               iPotCount;   // valid pot data available        
  char              szRdOut[25];
  DCX_POT_CTRLDATA  arPotData[6];// pot data information
  }DCX_CTRLDATA;




class CDCXBinTable : public CObject  
{
// Error Code definitions
//-----------------------
#define   DBT_ERR_NOERROR               0

#define   DBT_ERR_CANT_WRITE_DCX_FILE   1024
#define   DBT_ERR_CANT_WRITE_DEST_FILE  1025
#define   DBT_ERR_CANT_CONVERT_FILE     1026




public:
	CDCXBinTable();
	virtual ~CDCXBinTable();


// Attributes
//-----------
public:
  DCX_MEM_MAP     m_DcxMemMap;
  int             m_iLastError;
  LPSTR           m_pDCXBuffer;
                  
public:
	virtual void Serialize(CArchive& ar);

// Operations
//-----------
public:

  int     GetControlsCount(void);
  BOOL    GetControlDesc(int iIdx, LPDCX_CTRL_DESC lpCtrlDesc); // get control information by index
  BOOL    GetControlDesc(LPSTR  lpsName, LPDCX_CTRL_DESC lpCtrlDesc);// get control information by name

  int     ReadDCXTableFile(LPSTR  lpFName);

  int     CompileDCXTable(LPSTR szSourceDir, LPSTR szDest, BOOL bReverse);
  int     ReadDCXRawFile(LPSTR lpRawFileName, LPDCX_CTRL_DESC *ppCtrl, int  *piSize, BOOL bReverse);

  int     ReadFile(LPSTR  lpFName);//, LPDCX_MEM_MAP pMemMap);

  BOOL    RemapControlData(int iChannel, int iCtrl, int iVal, DCX_CTRLDATA  *pctrldata);
  BOOL    RemapControlData(CONTROLDATA * pctrld, DCX_CTRLDATA  *pctrldata);


  int     FreeDCXMapBuffers(void); // Free the Buffers

};

#endif // !defined(AFX_DCXBINTABLE_H__129D671B_661E_11D1_A3B1_C4D3CD000000__INCLUDED_)
