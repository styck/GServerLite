//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// DCXDevice.cpp: implementation of the CDCXDevice class.
//
//
////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GServer.h"
#include "DCXDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "DCXBinTable.h"
#include "GServerDoc.h"


extern void    CTekSleep(DWORD m_dwBasedelay,DWORD dwDelay);	// see vuthread2.cpp


// #define DEMO      // DEMO SOFTWARE - No DCX hardware writes are allowed

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCXDevice::CDCXDevice(CGServerDoc *pDoc)
{

// Allocate the buffer for read. The Write buffer is not fully implemented yet
// because there seems to be no reason to do this
//----------------------------------------------------------------------------

  m_pchBuffWrite = (DCXPORT_WRITE_INPUT *)GlobalAlloc(GPTR, sizeof(DCXPORT_WRITE_INPUT)); 
	m_pchBuffRead  = (DCXPORT_WRITE_INPUT *)GlobalAlloc(GPTR, sizeof(DCXPORT_WRITE_INPUT));


	m_hDevice = INVALID_HANDLE_VALUE;
	m_pDoc    = pDoc;
}

CDCXDevice::~CDCXDevice()
{
	Close(); // Make sure the Device handle is Closed

// Free the read/write buffers

	if(m_pchBuffRead)
		GlobalFree((HGLOBAL)m_pchBuffRead);

	if(m_pchBuffWrite)
		GlobalFree((HGLOBAL)m_pchBuffWrite);
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: Open(void)
//
// Open the DCX device
//
/////////////////////////////////////////////////////////////////////

BOOL  CDCXDevice::Open(void)
{
BOOL    bRet = TRUE;

#ifndef DEMO    // If NOT demo then open the DCX device driver

	if(m_hDevice == INVALID_HANDLE_VALUE && m_pchBuffRead != NULL && m_pchBuffWrite != NULL)
  {
		m_hDevice   = CreateFile(
										DEVICE_FILE_NAME, // Open the Device "file"
										GENERIC_READ | GENERIC_WRITE,
										FILE_SHARE_READ | FILE_SHARE_WRITE,
										NULL,
										OPEN_EXISTING,
										0,
										NULL);

		if(m_hDevice == INVALID_HANDLE_VALUE)
			bRet = FALSE;
		else
			ResetBus();      
  }
	else
		bRet = FALSE;

#else
    bRet=TRUE;
#endif

return bRet;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: Close(void) the DCX driver
//                   
//  ALWAYS RETURNS TRUE ??????????????
//
/////////////////////////////////////////////////////////////////////

BOOL  CDCXDevice::Close(void) 
{
BOOL    bRet = TRUE;

#ifndef DEMO  // Close the device if NOT compiled as DEMO
	if(m_hDevice != INVALID_HANDLE_VALUE)
  {
		CloseHandle(m_hDevice);
		m_hDevice = INVALID_HANDLE_VALUE;
  }
#endif

return bRet;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: Write(int iAddr, LPSTR lpsz, UINT  *puiWrite)
//
//  Write data to the DCX device.
//
/////////////////////////////////////////////////////////////////////

BOOL  CDCXDevice::Write(int iAddr, LPSTR lpsz, ULONG  *pulWrite, BOOL bIsVU)
{

int                 iBufferLength;
BOOL                bIoctlResult = FALSE;

#ifndef DEMO  // Write to DCX if NOT a demo version

    m_DeviceIOLock.Lock();    // Lock this resource until we read the response

		if(m_hDevice  != INVALID_HANDLE_VALUE)
		{
			m_pchBuffWrite->PortNumber = 0;
			m_pchBuffWrite->Addr = iAddr;
  
			strcpy(m_pchBuffWrite->arChar, lpsz);

			m_pchBuffWrite->ulLength = lstrlen(m_pchBuffWrite->arChar);
			iBufferLength = sizeof(DCXPORT_WRITE_INPUT);

			bIoctlResult = DeviceIoControl(
													m_hDevice,                     // Handle to device
													IOCTL_GPD_WRITE_PORT_BUFFER,    // IO Control code for Write
													m_pchBuffWrite,                    // Buffer to driver.  Holds port & data.
													iBufferLength,                  // Length of buffer in bytes.
													NULL,                           // Buffer from driver.   Not used.
													0,                              // Length of buffer in bytes.
													pulWrite,                      // Bytes placed in outbuf.  Should be 0.
													NULL                            // NULL means wait till I/O completes.
													);


				if ( ! bIoctlResult)                            // Did the IOCTL succeed?
				{
					if(m_pDoc)
						m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_WRITE);
				}
				else
				{
					bIoctlResult = TRUE;
				}
		}

#else
					bIoctlResult = TRUE;
#endif
    if(bIsVU)
     CTekSleep(m_pDoc->m_dwBasedelay,m_pDoc->m_dwVudelay);
//     CTekSleep(9000,28);    // was 50
    else         
     CTekSleep(m_pDoc->m_dwBasedelay,m_pDoc->m_dwCtrldelay);
//     CTekSleep(9000,68);    // was 65

return bIoctlResult;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: Write(LPSTR lpsz, UINT  *puiWrite)
//
// If a string is passed then extract the module address and
// write the string to the DCX
//

BOOL  CDCXDevice::Write(LPSTR lpsz, ULONG  *pulWrite, BOOL bIsVU)
{
char                chAddr[4];
int                 iAddr;
BOOL                bIoctlResult = FALSE;

#ifndef DEMO

  m_DeviceIOLock.Lock();  // Lock this resource until we read the response

	if(m_hDevice  != INVALID_HANDLE_VALUE)
  {

// Get the module address from the command passed 

		chAddr[0] = *(lpsz + 1);
		chAddr[1] = *(lpsz + 2);
		chAddr[2] = *(lpsz + 3);
		chAddr[3] = 0;

		iAddr = atoi((LPSTR)chAddr);

	// Write the command to the DCX unit

		bIoctlResult = Write(iAddr, lpsz, pulWrite, bIsVU);
  
  }
#else
  bIoctlResult = TRUE;
#endif

    //CTekSleep(m_pDoc->m_dwBasedelay,m_pDoc->m_dwCtrldelay);

return bIoctlResult;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: Read(LPSTR lpsz, UINT uiSize, UINT  *puiRead)
//
//	Parse the string passed and extract the module address and read data into
//  the pointer passed
//
BOOL  CDCXDevice::Read(LPSTR lpsz, UINT uiSize, ULONG *pulRead)
{
char                chAddr[4];
int                 iBufferLength;
BOOL                bIoctlResult = FALSE;

#ifndef DEMO

	if(m_hDevice != INVALID_HANDLE_VALUE)
	{

// Get the module address from the command passed 

		chAddr[0] = *(lpsz + 1);
		chAddr[1] = *(lpsz + 2);
		chAddr[2] = *(lpsz + 3);
		chAddr[3] = 0;

		m_pchBuffWrite->PortNumber = 0;				// This is always port 0
		m_pchBuffWrite->Addr = atoi((LPSTR)chAddr);	// Set the module address
  
		strcpy(m_pchBuffWrite->arChar, lpsz);
 
		m_pchBuffWrite->ulLength = lstrlen(m_pchBuffWrite->arChar);
		iBufferLength = sizeof(DCXPORT_WRITE_INPUT);


		bIoctlResult = DeviceIoControl(
                      m_hDevice,                      // Handle to device
                      IOCTL_GPD_READ_PORT_BUFFER,     // IO Control code for Write
                      m_pchBuffWrite,                 // Buffer to driver.  Holds port & data.
                      iBufferLength,                  // Length of buffer in bytes.
                      m_pchBuffRead,                  // Buffer from driver.   Not used.
                      iBufferLength,                  // Length of buffer in bytes.
                      pulRead,                        // Bytes placed in outbuf.  Should be 0.
                      NULL                            // NULL means wait till I/O completes.
                      );

		if ( ! bIoctlResult)                            // Did the IOCTL succeed?
		{
			if(m_pDoc)
				m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_READ);
		}
		else
		{
    // Copy the data into the Buffuer supplied by the Caller
    //-----------------------------------------------------
			CopyMemory(lpsz, m_pchBuffRead->arChar, uiSize);
			bIoctlResult = TRUE;
		}
	}
			m_DeviceIOLock.Unlock();  // Unlock this resource after reading result.

#else

    bIoctlResult = TRUE;

#endif

return bIoctlResult;
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION:  ResetBus(void);
//
//  Resets the DCX Bus
//

BOOL    CDCXDevice::ResetBus(void)
{

BOOL                bIoctlResult;
DWORD               dwReturnedLength;
int                 iBufferLength;
DCXPORT_WRITE_INPUT dcx_buffer;       // buffer for DCX i\o Control

	dcx_buffer.ulLength = 0;
	dcx_buffer.PortNumber = 0;

	dcx_buffer.CharData = CS_RESET;
	dcx_buffer.Addr = DCX_START_ADDRESS; // this means nothing here

	iBufferLength = sizeof(DCXPORT_WRITE_INPUT);

#ifndef DEMO

	if(m_hDevice  != INVALID_HANDLE_VALUE)
	{
		bIoctlResult = DeviceIoControl(
                      m_hDevice,                     // Handle to device
                      IOCTL_GPD_WRITE_PORT_UCHAR,     // IO Control code for Write
                      &dcx_buffer,                      // Buffer to driver.  Holds port & data.
                      iBufferLength,                  // Length of buffer in bytes.
                      NULL,                           // Buffer from driver.   Not used.
                      0,                              // Length of buffer in bytes.
                      &dwReturnedLength,                // Bytes placed in outbuf.  Should be 0.
                      NULL                            // NULL means wait till I/O completes.
                      );

  
		if ( ! bIoctlResult)                            // Did the IOCTL succeed?
		{
			if(m_pDoc)
				m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_READ);
		}

		Sleep(10);	// 10 ms delay

		dcx_buffer.ulLength = 0;
		dcx_buffer.PortNumber = 0;
		dcx_buffer.CharData = 0;

		bIoctlResult = DeviceIoControl(
                      m_hDevice,                     // Handle to device
                      IOCTL_GPD_WRITE_PORT_UCHAR,     // IO Control code for Write
                      &dcx_buffer,                      // Buffer to driver.  Holds port & data.
                      iBufferLength,                  // Length of buffer in bytes.
                      NULL,                           // Buffer from driver.   Not used.
                      0,                              // Length of buffer in bytes.
                      &dwReturnedLength,                // Bytes placed in outbuf.  Should be 0.
                      NULL                            // NULL means wait till I/O completes.
                      );

		if ( ! bIoctlResult)                            // Did the IOCTL succeed?
		{
			if(m_pDoc)
				m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_READ);
		}
		else
			bIoctlResult = TRUE;

		Sleep(10);
	}

#else

  bIoctlResult = TRUE;

#endif

return bIoctlResult;

}

