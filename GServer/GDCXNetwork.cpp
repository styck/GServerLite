//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// GDCXNetwork.cpp: implementation of the CGDCXNetwork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GServer.h"
#include "GDCXNetwork.h"
#include "Module.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include  "GServerDoc.h"		// Includes many other header files
#include	"DCXDeviceMap.h"

extern void    CTekSleep(DWORD dwDelay);	// see vuthread2.cpp

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGDCXNetwork::CGDCXNetwork(CGServerDoc *pDoc)
{
	m_pDoc = pDoc;
	m_pAssListener  = NULL; //new CCorTekAsyncSocket(this);
	m_iPort = 9191;
	m_csIPAddress = "";

	// Array of Connection Objects
	//----------------------------

	m_connections.SetSize(MAX_ASYNC_SONNECTIONS);
	m_iConnInUse = 0;
	m_bIsClient = FALSE;
	m_pDeviceMap = &pDoc->m_dcxdevMap;		// Point to the device map setup
																				// CHANGE THIS SO WE DON'T USE THIS NEW VARIABLE AND
																				// JUST USE THE pDoc-> member variable
}

CGDCXNetwork::~CGDCXNetwork()
{
	ShutDown();
}

/////////////////////////////////////////////////////////////////////////////
// CGServerDoc serialization

void CGDCXNetwork::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// Store this Network Settings Settings
		// ------------------------------------
		ar <<  NETWORK_OBJ_ID;
		ar <<  NETWORK_VERSION;  
		ar <<  m_bIsClient;
		ar <<  m_iPort;
		ar <<  m_csIPAddress;
	}
	else
	{
		DWORD     dwVersion;
		CString   csObjID;
		// Load This Document Settings
		//----------------------------
		ar >>  csObjID;  

		if(csObjID == NETWORK_OBJ_ID)
		{
			ar >>  dwVersion;
			ar >>  m_bIsClient;
			ar >>  m_iPort;
			ar >>  m_csIPAddress;
		}
	}
}


////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: IsStartedAsClient(void)
//
//
BOOL    CGDCXNetwork::IsStartedAsClient(void)
{
	return (m_pAssListener != NULL && m_bIsClient);
}

////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: IsStartedAsServer(void)
//
//
BOOL    CGDCXNetwork::IsStartedAsServer(void)
{
	return (m_pAssListener != NULL && ! m_bIsClient);
}

////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: StartAsClient(void)
//
//
BOOL    CGDCXNetwork::StartAsClient(void)
{
	return StartAsClient(LPCTSTR (m_csIPAddress), m_iPort);
}

BOOL    CGDCXNetwork::StartAsClient(LPCTSTR lpcs, UINT iPort)
{
BOOL    bRet = FALSE;
int			iErr;

	// First make sure the Listener is not opened already ... 
	//-------------------------------------------------------
	if(m_pAssListener == NULL)
	{
		if(InitListener())
		{
			if(m_pAssListener->Create(iPort,SOCK_STREAM))
			{
				m_pAssListener->SetOptions();
				iErr = m_pAssListener->Connect(lpcs, iPort);

		// For the CAsnycSocket, iErr can be zero but the last error
		// will be WSAEWOULDBLOCK

				if( iErr || (GetLastError( ) == WSAEWOULDBLOCK) )
				{
					m_iPort = iPort;
					m_csIPAddress = lpcs;
					bRet = TRUE;
				}
				else
				{
					iErr = GetLastError( );
				}
			}
		}
	}

	if(bRet == FALSE)
	{
		ShutDown();
	}
	else
		m_bIsClient = TRUE;

return bRet;
}

////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: StartAsServer
//
//
BOOL    CGDCXNetwork::StartAsServer(void)
{

	return  StartAsServer(LPCTSTR (m_csIPAddress), m_iPort);
}

BOOL    CGDCXNetwork::StartAsServer(LPCTSTR lpcs, UINT  iPort)
{
  BOOL      bRet = FALSE;
  long      lEvent;
  int       i;
  CModule   cModule;
  int       iSubAux = 0xFFFFFFFF;
  int       iMatrix = 0xFFFFFFFF;


  // Find the master module and setup the VUMetersArray - TEMP TEMP ?????????
	// This is just hardcoding what VU data to send
	// The client will eventually set this array
  // ????????????????????????????? This has to be done   

  for(i=0;i<DCX_DEVMAP_MAXSIZE;i++)
  {

		/////////////////////////////////////////////////////////////////////
		// If there is a module there then set VU 0,1,2,3,4,6 to be updated
		//
		// If we remove the other code around here we still want to keep
		// the VU initialization below.

		if(m_pDoc->m_dcxdevMap.GetModuleType(i) != DCX_DEVMAP_MODULE_NA)
		{
			char	chBufferVUType[64];
			ULONG   ulIO;

				wsprintf(chBufferVUType, "*%03dXVUg\n", cModule.GetModuleAddress(i));
				m_pDoc->m_pDCXDevice->Write(cModule.GetModuleAddress(i), chBufferVUType, &ulIO);	// Write VU type command
				Sleep(10);
				m_pDoc->m_pDCXDevice->Read(chBufferVUType, 64, &ulIO);		// Read response and discard
		}

		// Setup to read MASTER VU's

    if(m_pDoc->m_dcxdevMap.GetModuleType(i) == DCX_DEVMAP_MODULE_MASTER)
    {

		  m_pDoc->m_VUMetersArray.m_aVUReadData[40+i].iAddr=cModule.GetModuleAddress(i);							// the VUthread Reads from here(module address)
		  m_pDoc->m_VUMetersArray.m_aVUReadData[40+i].iVUType=3;							// the VUthread Reads from here(Pre, Post, Comp, Gate)
		  m_pDoc->m_VUMetersArray.m_aVUReadData[40+i].iLock=1;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
      m_pDoc->m_VUMetersArray.m_aVUReadData[40+i].iModuleIdx = i;

		  m_pDoc->m_VUMetersArray.m_aVUReadData[40+i + 1].iAddr=cModule.GetModuleAddress(i);							// the VUthread Reads from here(module address)
		  m_pDoc->m_VUMetersArray.m_aVUReadData[40+i + 1].iVUType=4;							// the VUthread Reads from here(Pre, Post, Comp, Gate)
		  m_pDoc->m_VUMetersArray.m_aVUReadData[40+i + 1].iLock=1;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
      m_pDoc->m_VUMetersArray.m_aVUReadData[40+i + 1].iModuleIdx = i;

    }
   
    if(m_pDoc->m_dcxdevMap.GetModuleType(i) == DCX_DEVMAP_MODULE_AUX)
    {

      if(iSubAux == 0xFFFFFFFF)
      {
        iSubAux = i;

		    m_pDoc->m_VUMetersArray.m_aVUReadData[60+i].iAddr=cModule.GetModuleAddress(i);							// the VUthread Reads from here(module address)
		    m_pDoc->m_VUMetersArray.m_aVUReadData[60+i].iVUType=3;							// the VUthread Reads from here(Pre, Post, Comp, Gate)
		    m_pDoc->m_VUMetersArray.m_aVUReadData[60+i].iLock=1;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
        m_pDoc->m_VUMetersArray.m_aVUReadData[60+i].iModuleIdx = i;

		    m_pDoc->m_VUMetersArray.m_aVUReadData[60+i+1].iAddr=cModule.GetModuleAddress(i);							// the VUthread Reads from here(module address)
		    m_pDoc->m_VUMetersArray.m_aVUReadData[60+i+1].iVUType=4;							// the VUthread Reads from here(Pre, Post, Comp, Gate)
		    m_pDoc->m_VUMetersArray.m_aVUReadData[60+i+1].iLock=1;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
        m_pDoc->m_VUMetersArray.m_aVUReadData[60+i+1].iModuleIdx = i;
      }
    }
    // Store the Matrix Index in the global Array
    //
    if(m_pDoc->m_dcxdevMap.GetModuleType(i) == DCX_DEVMAP_MODULE_MATRIX)
      if(iMatrix == 0xFFFFFFFF )
        iMatrix = i;
    
  }

  if(iSubAux != 0xFFFFFFFF && iMatrix  != 0xFFFFFFFF)
  {
		m_pDoc->m_VUMetersArray.m_aVUReadData[iMatrix + 80].iAddr=cModule.GetModuleAddress(iMatrix);							// the VUthread Reads from here(module address)
		m_pDoc->m_VUMetersArray.m_aVUReadData[iMatrix + 80].iVUType=7;							// the VUthread Reads from here(Pre, Post, Comp, Gate)
		m_pDoc->m_VUMetersArray.m_aVUReadData[iMatrix + 80].iLock=1;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
    m_pDoc->m_VUMetersArray.m_aVUReadData[iMatrix + 80].iModuleIdx = iSubAux; // Index for VACS Client

		m_pDoc->m_VUMetersArray.m_aVUReadData[iMatrix + 81].iAddr=cModule.GetModuleAddress(iMatrix);							// the VUthread Reads from here(module address)
		m_pDoc->m_VUMetersArray.m_aVUReadData[iMatrix + 81].iVUType=8;							// the VUthread Reads from here(Pre, Post, Comp, Gate)
		m_pDoc->m_VUMetersArray.m_aVUReadData[iMatrix + 81].iLock=1;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
    m_pDoc->m_VUMetersArray.m_aVUReadData[iMatrix + 81].iModuleIdx = iSubAux; // Index for VACS Client
  }

// First make sure the Listener is not opened already ... 
//-------------------------------------------------------
	if(m_pAssListener == NULL)
	{
		if(InitListener() && InitClients())
		{
// This is for Async sockets
			if(m_pAssListener->Create(iPort,SOCK_STREAM,lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE ,lpcs))
// This is for CSocket
//			if(m_pAssListener->Create(iPort,SOCK_STREAM,lpcs))
			{
				if(m_pAssListener->Listen())
				{
					m_iPort = iPort;
					m_csIPAddress = lpcs;
					bRet = TRUE;
				}        
			}
		}
	}

///////////////////////////////////////////////////
// Clear out our state memory for the controls
// We should fill this in with the register zero values
//
	
			ZeroMemory(&m_wCurrentState[0][0],512*80*sizeof(WORD));

//////////////////////////////////////////////////

if(bRet == FALSE)
  ShutDown();
else
  m_bIsClient = FALSE;

return bRet;
}

////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION:   Init
//
//
BOOL    CGDCXNetwork::InitListener(void)
{
BOOL                bRet = TRUE;

	if(m_pAssListener == NULL)
  {
		m_pAssListener  = new CCorTekAsyncSocket(this);
		if(m_pAssListener == NULL)  
			bRet = FALSE;
  }
	else
	  bRet = FALSE;  

return bRet;

}
////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION:   Init
//
//
BOOL    CGDCXNetwork::InitClients(void)
{
BOOL                bRet = TRUE;
CCorTekAsyncSocket* pSocket = NULL;
int                 iCount;

	// Create all Sockets
	//-------------------------------
	for(iCount = 0; iCount < MAX_ASYNC_SONNECTIONS; iCount++)
  {
		pSocket = new CCorTekAsyncSocket(this);
  
		if(pSocket)
    {
			// Set the Default settings for the socket
			//----------------------------------------
			pSocket->SetOptions();
			m_connections[iCount] = (void *)pSocket; // Set this pointer to newly created object
    }
		else
    {
			bRet = FALSE;
			// oops an Error occured .. Cleanup here
			//--------------------------------------
			ShutDownClients();
    }
  }
return bRet;
}

////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION:   ShutDownClients
//
//
BOOL    CGDCXNetwork::ShutDownClients(void)
{
BOOL                bRet = TRUE;
CCorTekAsyncSocket* pSocket = NULL;
int                 iCount;

	// now Close off all connections
	//-------------------------------
	for(iCount = 0; iCount < MAX_ASYNC_SONNECTIONS; iCount++)
  {
	  pSocket = (CCorTekAsyncSocket*)m_connections[iCount];

	  if(pSocket)
    {
		  pSocket->Abort(); // abort all messages
			delete pSocket;
	    m_connections[iCount] = NULL; // Set this pointer to NULL!!!
    }
  }
return bRet;
}    


////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION:   ShutDown
//
//
BOOL    CGDCXNetwork::ShutDown(void)
{
BOOL                bRet = TRUE;

	if(m_pAssListener)
  {
		m_pAssListener->Close();
		delete m_pAssListener;
		m_pAssListener  = NULL;
  }

	bRet = ShutDownClients();
	m_bIsClient = FALSE;

	return bRet;
}    


////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION: OnAcceptConnection
//
//  Try to accept this connection
//
void CGDCXNetwork::OnAcceptConnection(int nErrorCode) 
{
int                 iCount;
CCorTekAsyncSocket* pSocket = NULL;


	if(m_iConnInUse < MAX_ASYNC_SONNECTIONS)
  {
		for(iCount = 0; iCount < MAX_ASYNC_SONNECTIONS; iCount++)
    {
			pSocket = (CCorTekAsyncSocket*)m_connections[iCount];
			if( ! pSocket->IsOpened() )
      {
				m_pAssListener->Accept(*pSocket);
				pSocket->Init();                      
				break;
      }
    }
  }

	// Check if we handled this connection
	// if the connection is not handled
	// make sure it is closed ............
	//------------------------------------

	if(iCount == MAX_ASYNC_SONNECTIONS)
  {
		pSocket = new CCorTekAsyncSocket(this);
		m_pAssListener->Accept(*pSocket);
		pSocket->Close();
		delete (pSocket);
		m_pDoc->DisplayNetErrorMessage(IDS_MAX_CONNECTIONS);
  }
}

////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION: OnCloseConnection
//
//
//
void CGDCXNetwork::OnCloseConnection(int nErrorCode) 
{

	m_iConnInUse -- ;
	m_pDoc->DisplayGeneralMessage(IDS_CONNECTION_CLOSED);

// is this netobject opened as Client if so ShutDown
//--------------------------------------------------

	if(m_bIsClient)
		ShutDown();

	if(nErrorCode = 0)
  {
  }
	else
  {
		LPVOID    lpvMsg = NULL;

		if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,    nErrorCode,
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR) &lpvMsg,    0,    NULL ))
    {
    
			m_pDoc->DisplayNetErrorMessage((LPSTR)lpvMsg);
			LocalFree(lpvMsg);
    }
  }  

	// Notify the Document that the Connection was just closed
	//--------------------------------------------------------
	m_pDoc->HandleNetConnectionClose();


	// Broadcast this Event to all other connections ?!
	//-------------------------------------------------

}

////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION: OnConnect
//
//
//
void CGDCXNetwork::OnConnect(int nErrorCode) 
{
	if(nErrorCode == 0)
  {
		m_iConnInUse ++ ;

  // Broadcast this Event to all other connections ?!
  //-------------------------------------------------

  }
	else
  {
		LPVOID    lpvMsg = NULL;

		if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,    nErrorCode,
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR) &lpvMsg,    0,    NULL ))
    {
    
			m_pDoc->DisplayNetErrorMessage((LPSTR)lpvMsg);
			LocalFree(lpvMsg);
    }
  }  
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: OnReceive(int nErrorCode);
//
// This checks for data recieved from the client and acts on it 
// depending on the message type.
//

void    CGDCXNetwork::OnReceive(int nErrorCode, CCorTekAsyncSocket *psocket)
{
HDR_DCXTCP  hdrDCXTcp;

char    chBuffer[64];	// throw away buffer to read the response from DCX
ULONG   ulIO;					// returned error code from read
int			iRecvd;				// Number of bytes recieved

	iRecvd=psocket->Receive(&hdrDCXTcp, sizeof(HDR_DCXTCP), MSG_PEEK );
	if( iRecvd> 0)
	{
		if(iRecvd != sizeof(HDR_DCXTCP))
			return;
		else
			psocket->Receive(&hdrDCXTcp, sizeof(HDR_DCXTCP));
		// Is it a correct message header
		//-------------------------------

		if(hdrDCXTcp.iID == DCX_TCP_ID)
		{

		// the Client is not adding the size of the HDR_DCXTCP
		// where the Server is adding it and expecting to have it
		// in place
		//=======================================================

		//
		//if(hdrDCXTcp.wSize > sizeof(HDR_DCXTCP))
			//  {
			//  psocket->Receive(m_chNetBufferIn, hdrDCXTcp.wSize - sizeof(HDR_DCXTCP));
			//  }

			iRecvd=psocket->Receive(m_chNetBufferIn, hdrDCXTcp.wSize );
			if(iRecvd != hdrDCXTcp.wSize )
				iRecvd=psocket->Receive(chBuffer, hdrDCXTcp.wSize - iRecvd);	// Get the rest of it

			switch(hdrDCXTcp.wMessage)
			{

// Server Recived message to send bin file, so do it

///////////////////////////////////////////////
				case	DCX_TCP_SEND_TABLE:
				{
						m_pDoc->DisplayGeneralMessage("@@ Starting send: DCX.BIN @@");
					  psocket->State = DCX_TCP_SEND_TABLE;
						psocket->OffsetToSend = 0;
						psocket->HowManyInSendBuf=0;
						psocket->OnSend(0);
				}
				break;
///////////////////////////////////////////////

// Server Recived message to send control states, so do it

///////////////////////////////////////////////
				case	DCX_TCP_SEND_CONTROL_STATES:
				{
						m_pDoc->DisplayGeneralMessage("@@ Starting send: CONTROLS @@");
					  psocket->State = DCX_TCP_SEND_CONTROL_STATES;
						psocket->OffsetToSend = 0;
						psocket->HowManyInSendBuf=0;
						psocket->OnSend(0);
				}
				break;


///////////////////////////////////////////////
// A request to send setup information has arrived

				case	DCX_TCP_SEND_SETUP:
				{
					m_pDoc->DisplayGeneralMessage("@@ Sending DCX Setup @@");
					m_pDoc->m_pdcxNetwork->BroadcastMsgType(m_pDeviceMap->GetData(), DCX_DEVMAP_MAXSIZE*sizeof(DWORD), DCX_TCP_RECEIVE_SETUP, psocket, CURRENT);
				}

				break;


////////////////////////////////////////////////////////////////////
//
// Client has sent a control value so we need to lookup the control
// and write the data to the dcx unit

				case  DCX_SET_CONTROL_VALUE:
				
					CONTROLDATA   *ctrld;
					DCX_CTRLDATA  dcxCtrlData;
					ULONG         ulWrite;
					int           iCount;
        
					ctrld = (CONTROLDATA *)m_chNetBufferIn;

					// Do some error checking to keep the program from crashing
					// The control number should never be 512 or greater
					// and we are currently limited to 80 channels

					if( (ctrld->wCtrl < 512) && (ctrld->wChannel < 80) )	
					{
							// Save the value of this control on this channel (module)

							m_wCurrentState[ctrld->wChannel][ctrld->wCtrl] = ctrld->wVal;

							// Resend the control data to all other clients so that they can update their displays
							// MAY NOT SEND DATA.  USE THE CALLBACK WAY OF SENDING

							m_pDoc->m_pdcxNetwork->BroadcastMsgType(ctrld, sizeof(CONTROLDATA), DCX_TCP_CONTROL_DATA, psocket, ALL);

							if(m_pDoc->m_dcxBinTable.RemapControlData(ctrld, &dcxCtrlData))
							{
								m_pDoc->DisplayGeneralMessage("&& Msg Data &&");

							/////////////////////////////////////////////////////////////////////////////
							// Loop thru all the pots for this control and write the XXW to the DCX

								for(iCount = 0; iCount < dcxCtrlData.iPotCount; iCount++)    
								{
									if( ! m_pDoc->m_pDCXDevice->Write(dcxCtrlData.arPotData[iCount].iAddr,
																							dcxCtrlData.arPotData[iCount].szData, &ulWrite))
									{
										m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_WRITE);
										break;
									}                                          

										CTekSleep(6);		// Delay 6 ms so that we can read our response
										
										// Read the response which should be !ccggp\n. We do nothing with this
										// cc - target chip, gg - group on the DCX board the chip is in
										// p - the target pot

										m_pDoc->DisplayGeneralMessage(dcxCtrlData.arPotData[iCount].szData);
										lstrcpy(chBuffer,dcxCtrlData.arPotData[iCount].szData); // must pass the address in string
										m_pDoc->m_pDCXDevice->Read(chBuffer, 64, &ulIO);

								}
								m_pDoc->DisplayGeneralMessage("&& Msg Data End &&");
							}
					}
					break;

				case DCX_TCP_TIME:		// Display the time that we received
					m_pDoc->DisplayTime(hdrDCXTcp.mmt);
					break;

        case DCX_START_VU_DATA:
          psocket->AcceptVuData(TRUE);
          break;
        case DCX_STOP_VU_DATA:
          psocket->AcceptVuData(FALSE);
          break;

				default:
					m_pDoc->DisplayGeneralMessage(IDS_NETMSG_NOT_IMPLEMET);
					break;
			}
		}
		else
			m_pDoc->DisplayGeneralMessage(IDS_NET_ERROR_INVALIDHEADER);
	}

return;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: SendMsgType(void* lpv, int iSize, UINT  uiType);
//
//  Called by: HandleSliderMove() Only.
//
//  Remove if server sliders removed
//
//
BOOL    CGDCXNetwork::SendMsgType(void* lpv, int iSize, UINT  uiType)
{
BOOL        bRet = FALSE;
HDR_DCXTCP  hdrDCXTcp;

	if(IsStartedAsClient())	
  {
		if( iSize <= MAX_NET_BUFFER) 
    {
			hdrDCXTcp.iID = DCX_TCP_ID;
			hdrDCXTcp.wMessage  = uiType;
			hdrDCXTcp.wSize = iSize;// + sizeof(HDR_DCXTCP); Hristo
			hdrDCXTcp.mmt.wType = TIME_MS;
			hdrDCXTcp.mmt.u.ms = GetTickCount();
    
    // Put the message header and the actual message into one buffer
    // so we can write them out together
    //--------------------------------------------------------------

			CopyMemory(m_chNetBufferOut, &hdrDCXTcp, sizeof(HDR_DCXTCP));
			if(lpv != NULL)
			  CopyMemory(&m_chNetBufferOut[sizeof(HDR_DCXTCP)], lpv, iSize);

			// Send the message and check for errros

			if(m_pAssListener->Send(m_chNetBufferOut, iSize + sizeof(HDR_DCXTCP)) != SOCKET_ERROR)
				bRet = TRUE;
			else
				bRet = FALSE;
    }
  }
return bRet;
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: BroadcastMsgType
//
// lpv - point to data
// iSize - size of data
// uiType - predefined data type
// pCurrentSocket - if not NULL then it is the pointer to the current socket
// uiWho - ALL=0 broadcasts to all, except current.  CURRENT=1 broadcasts only to the pCurrentSocket
//

BOOL  CGDCXNetwork::BroadcastMsgType(void* lpv, int iSize, UINT  uiType, CCorTekAsyncSocket *pCurrentSocket, UINT uiWho)
{
BOOL                bRet = TRUE;
HDR_DCXTCP          hdrDCXTcp;
CCorTekAsyncSocket* pSocket = NULL;
int                 iCount;
int									iSent;

	if(IsStartedAsServer())
  {
		if( iSize <= MAX_NET_BUFFER) 
    {
			hdrDCXTcp.iID = DCX_TCP_ID;
			hdrDCXTcp.wMessage  = uiType;
			hdrDCXTcp.wSize = iSize ;					// + sizeof(HDR_DCXTCP); Hristo
			hdrDCXTcp.mmt.wType = TIME_MS;
			hdrDCXTcp.mmt.u.ms = GetTickCount();
    
    // Put the message header and the actual message into one buffer
    // so we can write them out together
    //--------------------------------------------------------------

			CopyMemory(m_chNetBufferOut, &hdrDCXTcp, sizeof(HDR_DCXTCP));

			if(lpv != NULL)
			  CopyMemory(&m_chNetBufferOut[sizeof(HDR_DCXTCP)], lpv, iSize);

				if(uiWho == CURRENT)	// Only send to the requesting client
				{
					if( pCurrentSocket->IsOpened())
							pCurrentSocket->Send(m_chNetBufferOut, iSize + sizeof(HDR_DCXTCP));
				}
				else
				{			// uiWho == ALL so send to all other clients
					for(iCount = 0; iCount < MAX_ASYNC_SONNECTIONS; iCount++)
					{
						pSocket = (CCorTekAsyncSocket*)m_connections[iCount];

						if( pSocket->IsOpened() &&				// socket is opened
							  (pSocket != pCurrentSocket)		// and not the current socket
							) 														
						{
			// If its VU data and client accepts vu data then send it
							if( (uiType == DCX_VU_DATA) && pSocket->DoesAcceptVuData() )
								iSent=pSocket->Send(m_chNetBufferOut, iSize + sizeof(HDR_DCXTCP));
			// If it is not VU data then just send it
							else if(uiType != DCX_VU_DATA)
								iSent=pSocket->Send(m_chNetBufferOut, iSize + sizeof(HDR_DCXTCP));

							// If it all didn't go out then report an error

							if(iSent != iSize + (int) sizeof(HDR_DCXTCP) )
							{
											bRet = FALSE;
											m_pDoc->DisplayGeneralMessage("*** BroadcastMsgType: incomplete ***");
							}
						}
					}
				}
			}
			else
			{
					m_pDoc->DisplayGeneralMessage("BroadcastMsgType: Size too big");
			}
  }
return bRet;
}

