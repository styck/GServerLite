//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// CorTekAsyncSocket.cpp : implementation file
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GServer.h"
#include "GServerDoc.h"
#include "CorTekAsyncSocket.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "GDCXNetwork.h" // Implementation part so we can call the 
                         // CGDCXNetwork  member functions

////////////////////////////////////////////////////////////////////
// CCorTekAsyncSocket

CCorTekAsyncSocket::CCorTekAsyncSocket(CGDCXNetwork * pnet)
{
	m_pNet = pnet;
	m_bOpened = FALSE;
  m_bSendVuData = FALSE;
	m_hFile = NULL;
  iSocketNumber = -1; // Invalidate the socket number

	HowManyInSendBuf = 0;							// How much data in send buffer
	OffsetToSend = 0;									// Index into data that needs to be sent
	m_bSendingControlStates = FALSE;	// not sending the control states yet

}

CCorTekAsyncSocket::~CCorTekAsyncSocket()
{
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CCorTekAsyncSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CCorTekAsyncSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

////////////////////////////////////////////////////////////////////
// CCorTekAsyncSocket member functions

CCorTekAsyncSocket::Abort()
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////
//
// Set member variable to signal open
//

BOOL CCorTekAsyncSocket::Init(void) 
{
if(m_bOpened == FALSE)
  m_bOpened = TRUE;
else
  return FALSE;
    
return TRUE; 
}


////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: SetOptions
//  IMPORTANT FUNCTION
//
//  Set the Default Options for this Socket
//
BOOL CCorTekAsyncSocket::SetOptions(void) 
{
BOOL    bRet = TRUE;

// these are used to set the socket Options

int iSize=16384;
BOOL	bOpt = TRUE;

	if(!SetSockOpt(SO_RCVBUF, &iSize, sizeof(int), SOL_SOCKET))
			bRet=FALSE;

	if(!SetSockOpt(SO_SNDBUF, &iSize, sizeof(int), SOL_SOCKET))
			bRet=FALSE;

//	if( ! SetSockOpt(SO_DONTLINGER, &bOpt, sizeof(BOOL), SOL_SOCKET))
//			bRet=FALSE;

	if( ! SetSockOpt(TCP_NODELAY , &bOpt, sizeof(BOOL), IPPROTO_TCP))
			bRet=FALSE;

return bRet;
}
////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL CCorTekAsyncSocket::IsOpened(void) 
{
	return m_bOpened; 
}

////////////////////////////////////////////////////////////////////
//
//
//
//
void CCorTekAsyncSocket::OnAccept(int nErrorCode) 
{

//CSocket::OnAccept(nErrorCode);
	CAsyncSocket::OnAccept(nErrorCode);


	// Let the Network Object handle the Connection
//---------------------------------------------	
	m_pNet->OnAcceptConnection(nErrorCode);

}

////////////////////////////////////////////////////////////////////
//
//
//
//
void CCorTekAsyncSocket::OnClose(int nErrorCode) 
{

#ifdef _DEBUG
char chBuffer[64];
#endif

// handle the Close Connection message ...
//----------------------------------------
//CSocket::OnClose(nErrorCode);

	CAsyncSocket::OnClose(nErrorCode);

	Close();

	m_bOpened = FALSE;
  m_bSendVuData = FALSE; // Force the VU flag to FALSE ...
                         // Until the Client requests the VU data to start
  
  /////////////////////////////////////////////////
  // clear the lock flag for VUs that this client
  // was looking at

	// Negate the current locks and add to the global locks

	for(int i=0;i < MAX_VU_READ_DATA; i++)
	{
		m_pNet->m_pDoc->m_VUMetersArray.m_aVUReadData[i].cLock+=-m_pNet->m_pDoc->m_SocketVULocks[iSocketNumber][i];
		if(m_pNet->m_pDoc->m_VUMetersArray.m_aVUReadData[i].cLock < 0)
      m_pNet->m_pDoc->m_VUMetersArray.m_aVUReadData[i].cLock = 0;

		m_pNet->m_pDoc->m_SocketVULocks[iSocketNumber][i] = 0;

		// Show the resulting VU locks for DEBUG
#ifdef _DEBUG
						wsprintf(chBuffer,"%d",m_pNet->m_pDoc->m_VUMetersArray.m_aVUReadData[i].cLock);
							OutputDebugString(chBuffer);
#endif

	}

#ifdef _DEBUG
							OutputDebugString("\n");
#endif

  iSocketNumber = -1; // Invalidate the socket number

	State = 0;					// Clear the recieve state

	if(m_hFile != NULL)
				CloseHandle(m_hFile);
	m_hFile = NULL;

// Let the Network Object handle the Close
//----------------------------------------
	m_pNet->OnCloseConnection(nErrorCode);
	
}

////////////////////////////////////////////////////////////////////
//
//	Get here when a client connects to the server
//

void CCorTekAsyncSocket::OnConnect(int nErrorCode) 
{

	CAsyncSocket::OnConnect(nErrorCode);
  
	if(nErrorCode)
  {
		m_bOpened = FALSE;
		Close();
  }

  m_bSendVuData = FALSE; // Force the VU flag to FALSE ...
                         // Until the Client requests the VU data to start
	m_pNet->OnConnect(nErrorCode);
}

////////////////////////////////////////////////////////////////////
//
//	Call the real Receive routine
//
//
void CCorTekAsyncSocket::OnReceive(int nErrorCode) 
{
	CAsyncSocket::OnReceive(nErrorCode);
	m_pNet->OnReceive(nErrorCode, this);

}

////////////////////////////////////////////////////////////////////
//
// Set flag to accept/no accept VU data
// Client sends message saying if it accepts VU data or not

void CCorTekAsyncSocket::AcceptVuData(BOOL b)
{
  m_bSendVuData = b;  
}

////////////////////////////////////////////////////////////////////
//
//	Return flag for sending VU data
//

BOOL CCorTekAsyncSocket::DoesAcceptVuData(void)
{
  return m_bSendVuData;
}

////////////////////////////////////////////////////////////////////
//
// This gets called whenever we can send data.
//
//
void CCorTekAsyncSocket::OnSend(int nErrorCode) 
{
		switch(State)
		{

				////////////////////////////////////////////////////////////////////
			  // We are sending the dcx.bin file
				//
				// Probably should be a subroutine

				case	DCX_TCP_SEND_TABLE:
				{

					DCX_CTRL_FILE_HDR   dcxFileHeader = { 0 };
					DWORD               dwRead;
					DWORD               dwFileSize;
					BOOL								bResult;
					HDR_DCXTCP					*pHdr=(HDR_DCXTCP *)SendBuf;
					int									iSent;
					int									iErr;


					// If we haven't opened the file yet then do so and validate it
					//--------------------------------------------------------------

					if(m_hFile == NULL)	
					{
						char szBinDirectory[128];		// MAX_FILE_PATH

					//
					// Load in the DCX.BIN file
					//
					// Need to make this look in the current directory where the program is ran
					// for the dcx.bin file.

							GetCurrentDirectory(sizeof(szBinDirectory),szBinDirectory);
							strcat(szBinDirectory,"\\dcx.bin");

//							m_hFile = CreateFile(m_pNet->m_pDoc->m_dcxBinTable.m_csBinFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							m_hFile = CreateFile(szBinDirectory, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							dwFileSize = GetFileSize(m_hFile, NULL);

						if(( dwFileSize != 0 ) && ( dwFileSize != 0xFFFFFFFF) && (m_hFile != INVALID_HANDLE_VALUE))
						{
							SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
							::ReadFile(m_hFile, &dcxFileHeader, sizeof(DCX_CTRL_FILE_HDR), &dwRead, NULL);

							// verify the File Header
							//-----------------------

							if( (dcxFileHeader.dwID == DCX_TCP_ID) && (dcxFileHeader.iSize == sizeof(DCX_CTRL_FILE_HDR)) )
							{

								// File is has correct header, move back to the begining of the file
								//--------------------------------------------------------------------
								SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);

								pHdr->iID = DCX_TCP_ID;                      
								pHdr->wMessage  = DCX_TCP_RECEIVE_TABLE;

							}
							else
							{
								m_pNet->m_pDoc->DisplayGeneralMessage("@@ Send: DCX.BIN Bad Header @@");
								CloseHandle(m_hFile);
								m_hFile = NULL;
								State = 0;
								break;
							}

						}
						else
						{
							m_pNet->m_pDoc->DisplayGeneralMessage("@@ Send: DCX.BIN Bad FileSize @@");
							CloseHandle(m_hFile);
							m_hFile = NULL;
							State = 0;
							break;
						}

					}

					// Loop until all the file is sent or until some socket error
					//------------------------------------------------------------

					while(1)
					{
						// If send buf is empty then get some more data
						//--------------------------------------------
						if(HowManyInSendBuf == 0)
						{
								// now read MAX_NET_BUFFER 
								//-----------------------------------
								bResult=::ReadFile(m_hFile, &SendBuf[sizeof(HDR_DCXTCP)], MAX_NET_BUFFER , &dwRead, NULL);

								HowManyInSendBuf = dwRead+sizeof(HDR_DCXTCP);	// This is how much we want to send

								// Check for end of file. 
								//-----------------------------------
								if (bResult &&  (dwRead == 0) )
								{ 
									m_pNet->m_pDoc->DisplayGeneralMessage("@@ Send DCX.BIN: EOF : Send complete @@");
									CloseHandle(m_hFile);
									m_hFile = NULL;
									State = 0;
									m_pNet->m_pDoc->m_pdcxNetwork->BroadcastMsgType(NULL, 0, DCX_TCP_RECEIVE_TABLE_DONE, this, CURRENT);

									break;
								} 

								pHdr->wSize = dwRead;	// Update the header with new amount of data read
						}


								// At this point we either have data from before
								// or we read in some new data. Now go ahead
								// and send it to the client requesting it
								//------------------------------------------
								iSent=CAsyncSocket::Send(SendBuf+OffsetToSend, HowManyInSendBuf,0);

									// Check for error
									if(iSent == SOCKET_ERROR)
									{
										if(iErr=GetLastError() == WSAEWOULDBLOCK)
										{
												return;		// We were blocked. Return and we will be called
										}							// again when its ok to send
										else
										{							// A read error occured
											m_pNet->m_pDoc->DisplayGeneralMessage("@@ Send DCX.BIN: Socket Error  @@");
											CloseHandle(m_hFile);
											m_hFile = NULL;
											State = 0;
											return;
										}
									}

									// We sent some data
									// Now check if we only sent some of it
									// or all of it.
									//---------------------------------------
									if(iSent != HowManyInSendBuf)
									{
											// Did not send all the data so calculate offset

											OffsetToSend = OffsetToSend + iSent;
											HowManyInSendBuf = HowManyInSendBuf - iSent;
									}
									else
									{
											// We sent it all, reset everything
											//------------------------------------
											HowManyInSendBuf = 0;
											OffsetToSend = 0;

									}
					}

				}
				break;

				// Send the control states so that other clients know what to set their control
				// positions to.
				//
				// Probably should be a subroutine.

				case	DCX_TCP_SEND_CONTROL_STATES:
				{
					HDR_DCXTCP					*pHdr=(HDR_DCXTCP *)SendBuf;
					DWORD               dwDataSize;
					int									iSent;
					int									iErr;

					if(m_bSendingControlStates == FALSE)	
					{
							m_bSendingControlStates = TRUE;
							dwDataSize = 512*80*sizeof(WORD);		// size of data array holding control states

						// Set the header information for the TCP packet

						pHdr->iID = DCX_TCP_ID;                      
						pHdr->wMessage  = DCX_TCP_RECEIVE_CONTROL_STATES;

					}
					

					// Loop until all the data is sent or until some socket error
					//------------------------------------------------------------

					while(1)
					{
						// If send buf is empty then get some more data
						//--------------------------------------------
						if(HowManyInSendBuf == 0)
						{
								// Copy the data into the send buffer (only MAX_NET_BUFFER amount)

								CopyMemory(&SendBuf[sizeof(HDR_DCXTCP)], &m_pNet->m_wCurrentState, MAX_NET_BUFFER);
								HowManyInSendBuf = MAX_NET_BUFFER+sizeof(HDR_DCXTCP);	// This is how much we want to send
								pHdr->wSize = MAX_NET_BUFFER;	// Update the header with new amount of data read
								dwDataSize = dwDataSize - MAX_NET_BUFFER;	// Subtract from amount needed to send
								if(dwDataSize < 0 ) dwDataSize = 0;

								// Check for end of data
								//-----------------------------------
								if (dwDataSize == 0)
								{ 
									m_pNet->m_pDoc->DisplayGeneralMessage("@@ Send CONTROL STATES: EOF : Send complete @@");
									m_bSendingControlStates = FALSE;
									State = 0;
//									m_pNet->m_pDoc->m_pdcxNetwork->BroadcastMsgType(NULL, 0, DCX_TCP_RECEIVE_TABLE_DONE, this, CURRENT);

									break;
								} 

						}

///////////////////////////////////////////////////

								// At this point we either have data from before
								// or we read in some new data. Now go ahead
								// and send it to the client requesting it
								//------------------------------------------
								iSent=CAsyncSocket::Send(SendBuf+OffsetToSend, HowManyInSendBuf,0);

									// Check for error
									if(iSent == SOCKET_ERROR)
									{
										if(iErr=GetLastError() == WSAEWOULDBLOCK)
										{
												return;		// We were blocked. Return and we will be called
										}							// again when its ok to send
										else
										{							// A read error occured
											m_pNet->m_pDoc->DisplayGeneralMessage("@@ Send DCX.BIN: Socket Error  @@");
											m_bSendingControlStates = NULL;
											State = 0;
											return;
										}
									}

									// We sent some data
									// Now check if we only sent some of it
									// or all of it.
									//---------------------------------------
									if(iSent != HowManyInSendBuf)
									{
											// Did not send all the data so calculate offset

											OffsetToSend = OffsetToSend + iSent;
											HowManyInSendBuf = HowManyInSendBuf - iSent;
									}
									else
									{
											// We sent it all, reset everything
											//------------------------------------
											HowManyInSendBuf = 0;
											OffsetToSend = 0;

									}
///////////////////////////////////////////////////


					}

				}

		}

}


int CCorTekAsyncSocket::Receive(void* pBuffer, int iReadSize, int nFlags) 
{
////////////////////////////////////////////////////////////////////////////
// FUNCTION: Receive
//
// purpose: Read raw TCP data + implement partial reads for buffers
//          that are comming slow over the network
// 
// parms:
//  SOCKET    s;         // Socket to read from
//  LPSTR     pBuffer;   // pointer to the buffer
//  int       iReadSize; // how much to read
//                       // NOTE: The buffer should be big enough. This
//                                Function will not perform any checks
//                                for memory overruns
//
// return:
//  int       iTotalRead; // Number of bytes actualy read into the buffer pBuffer 
//

  LPSTR   pWalker;
  int     iCurRead = 0;
  int     iTotalRead = 0;

  pWalker = (LPSTR) pBuffer;

  while(iReadSize > 0) 
  {
    iCurRead = CAsyncSocket::Receive(pWalker, iReadSize, nFlags );
    if( (iCurRead >= 0) )//&& (iCurRead != SOCKET_ERROR))
    {
			pWalker += iCurRead;
			iReadSize -=  iCurRead;
			iTotalRead += iCurRead;
    }
    else
    {
      // This is some sort of an error ... we might have to check here
    break;
    }
  }

  return iTotalRead;

}

int CCorTekAsyncSocket::Send(const void* pBuffer, int iSendSize, int nFlags) 
{
////////////////////////////////////////////////////////////////////////////
// FUNCTION: Send
//
// purpose: Send raw TCP data + implement partial reads for buffers
//          that are comming slow over the network
// 
// parms:
//  SOCKET    s;         // Socket to read from
//  LPSTR     pBuffer;   // pointer to the buffer
//  int       iSendSize; // how much to read
//                       // NOTE: The buffer should be big enough. This
//                                Function will not perform any checks
//                                for memory overruns
//
// return:
//  int       iTotalSend; // Number of bytes actualy read into the buffer pBuffer 
//

  LPSTR   pWalker;
  int     iCurSend = 0;
  int     iTotalSend = 0;

  pWalker = (LPSTR) pBuffer;		// Point to our data

  while(iSendSize > 0) 
  {
    iCurSend = CAsyncSocket::Send(pWalker, iSendSize, nFlags );
    if( (iCurSend >= 0) && (iCurSend != SOCKET_ERROR))
    {
			pWalker += iCurSend;
			iSendSize -=  iCurSend;
			iTotalSend += iCurSend;
    }
    else
    {
      // This is some sort of an error ... we might have to check here
			
			if(GetLastError()  != WSAEWOULDBLOCK)
				return -1;
			else
				Sleep(1);	// Sleep and try again
    }
  }

  return iTotalSend;

}
