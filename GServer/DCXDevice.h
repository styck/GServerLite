//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// DCXDevice.h: interface for the CDCXDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCXDEVICE_H__EBB36D37_79C7_11D1_A3B5_6663AF000000__INCLUDED_)
#define AFX_DCXDEVICE_H__EBB36D37_79C7_11D1_A3B5_6663AF000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "..\portio\sys\Gpioctl.h"
#include <winioctl.h>
#include <afxmt.h>
#include "..\portio\dcxoctl.h"

class CDCXBinTable;
class CGServerDoc;

class CDCXDevice : public CObject  
{
#define     DEVICE_FILE_NAME    "\\\\.\\DcxDev"


// DCX Flags Genearl Flags ....
//
#define CS_RCVCTRL		0x01
#define CS_XMITCTRL		0x02
#define CS_RESET		  0x04
#define CS_USER1		  0x08
#define CS_ACKERR		  0x10
#define CS_RFEMPTY		0x20
#define CS_OEO			  0x40
#define CS_NEWDEVICE	0x80

private:
  HANDLE        m_hDevice;
  
  // The driver has problems writing data back to a local(on the stack) buffer
  // and there seems to be no problem with this type of buffer to write 
  // data out. However both buffers are defined here.
  //---------------------------------------------------------------------------
  DCXPORT_WRITE_INPUT * m_pchBuffWrite;
  DCXPORT_WRITE_INPUT * m_pchBuffRead;

public:
	CDCXDevice(CGServerDoc *pDoc);
	virtual ~CDCXDevice();


  BOOL      Open(void);
  BOOL      Close(void);
  BOOL      IsOpen(){ return (m_hDevice != INVALID_HANDLE_VALUE);};

  BOOL      Write(int iAddr, LPSTR lpsz, ULONG  *pulWrite);
  BOOL      Write(LPSTR lpsz, ULONG  *pulWrite);
  BOOL      Read(LPSTR lpsz, UINT uiSize, ULONG  *pulRead);

  BOOL      ResetBus(void);

public:
  CDCXBinTable    *m_pBinTable;
  CGServerDoc     *m_pDoc;
	CCriticalSection  m_DeviceIOLock;

};

#endif // !defined(AFX_DCXDEVICE_H__EBB36D37_79C7_11D1_A3B5_6663AF000000__INCLUDED_)
