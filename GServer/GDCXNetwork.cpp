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

#include "cputicker.h"

// #define HIGHRESOLUTIONTIMER   // Use MultiMedia timers if defined


extern void    CTekSleep(DWORD m_dwBasedelay,DWORD dwDelay);	// see vuthread2.cpp

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGDCXNetwork::CGDCXNetwork(CGServerDoc *pDoc)
{
	char szHostName[80];
	struct hostent *host;
    u_long InetAddr;                      // Inet-style address

    struct sockaddr_in SockAddrInet;                 // Inet socket address

	m_pDoc = pDoc;
	m_pAssListener  = NULL; 
	m_iPort = 9191;         // Default Port address
	iShowControlData = 0;	// Default don't show control data

	// Lets get our host name and find the IP address to display

	SockAddrInet.sin_family=AF_INET;
	SockAddrInet.sin_port = htons(9191);
	gethostname(szHostName,80);
	host=gethostbyname(szHostName);

	if(host)
	{
		InetAddr=*((u_long *)host->h_addr_list[0]);
	}

   SockAddrInet.sin_addr.S_un.S_addr = InetAddr;

	m_csIPAddress = inet_ntoa(SockAddrInet.sin_addr);

	
//	m_csIPAddress = "";     // Default Server address


	// Array of Connection Objects
	//----------------------------

	m_connections.SetSize(MAX_ASYNC_SONNECTIONS);
	m_iConnInUse = 0;
	m_pDeviceMap = &pDoc->m_dcxdevMap;		// Point to the device map setup
										// CHANGE THIS SO WE DON'T USE THIS NEW VARIABLE AND
										// JUST USE THE pDoc-> member variable


	// Indicate that we are NOT sending VU data on socket (for display)

	for(int i=0;i<MAX_ASYNC_SONNECTIONS;i++)
		m_bSendingVUData[i]=FALSE;

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
			ar >>  m_iPort;
			ar >>  m_csIPAddress;
		}
	}
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


  // First lets initialize the precision of our multimedia timer
  // used for read/write delays

#ifdef HIGHRESOLUTIONTIMER    // NEED TO TEST THIS MORE. CURRENTLY NOT WORKING
  if(timeBeginPeriod(1) != TIMERR_NOERROR) // Set it to 1ms
  {
    AfxMessageBox(_T("High resolution timing is not available on this CPU"));
  }
#endif


		/////////////////////////////////////////////////////////////////////
		// If there is a module there then set VU 0,1,2,3,4,6 to be updated
		//
		//  Initialize each module to send all the VU data


  for(i=0;i<DCX_DEVMAP_MAXSIZE;i++)
  {
		if(m_pDoc->m_dcxdevMap.GetModuleType(i) != DCX_DEVMAP_MODULE_NA)
		{
			char	chBufferVUType[64];
			char	chBufferDataType[64];
			ULONG   ulIO;

				wsprintf(chBufferVUType, "*%03dXVUg\n", cModule.GetModuleAddress(i));
				m_pDoc->m_pDCXDevice->Write(cModule.GetModuleAddress(i), chBufferVUType, &ulIO, FALSE);	// Write VU type command
				Sleep(20);
				m_pDoc->m_pDCXDevice->Read(chBufferVUType, 64, &ulIO);		// Read response and discard

        wsprintf(chBufferDataType, "*%03dXXW470000\n",cModule.GetModuleAddress(i));
        m_pDoc->m_pDCXDevice->Write(cModule.GetModuleAddress(i), chBufferDataType, &ulIO, FALSE); // Write Control Data command
        Sleep(20);
				m_pDoc->m_pDCXDevice->Read(chBufferVUType, 64, &ulIO);		// Read response and discard

		}			
  }


// First make sure the Listener is not opened already ... 
//-------------------------------------------------------
	if(m_pAssListener == NULL)
	{
		if(InitListener() && InitClients())
		{
// This is for Async sockets
			if(m_pAssListener->Create(iPort,SOCK_STREAM,lEvent = FD_READ | FD_WRITE | FD_ACCEPT | FD_CONNECT | FD_CLOSE ,lpcs))
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
// No more than MAX_NUM_CONTROLS unique controls

			ZeroMemory(&m_wCurrentState[0][0],MAX_NUM_CONTROLS*80*sizeof(WORD));

//////////////////////////////////////////////////

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
			bRet = pSocket->SetOptions();                    // Set the socket options

			m_connections[iCount] = (void *)pSocket; // Set this pointer to newly created object

			// Clear out local locks for each socket connection

			for(int i=0; i < MAX_VU_READ_DATA; i++)
				m_pDoc->m_SocketVULocks[iCount][i] = 0;

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
		m_iConnInUse ++ ;
		for(iCount = 0; iCount < MAX_ASYNC_SONNECTIONS; iCount++)
    {
			pSocket = (CCorTekAsyncSocket*)m_connections[iCount];
			if( ! pSocket->IsOpened() )
      {
				m_pAssListener->Accept(*pSocket);
				pSocket->Init();
        pSocket->iSocketNumber = iCount;  // Keep track of socket number

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

   // Force the view to update the client connection display
		m_pDoc->UpdateAllViews(NULL);

}

////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION: OnCloseConnection
//
//
//
void CGDCXNetwork::OnCloseConnection(int nErrorCode) 
{

	m_iConnInUse -- ;
	m_pDoc->UpdateAllViews(NULL);   // Force the view to update the client connection display

	m_pDoc->DisplayGeneralMessage(IDS_CONNECTION_CLOSED);


		LPVOID    lpvMsg = NULL;

		if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,    nErrorCode,
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR) &lpvMsg,    0,    NULL ))
    {
    
			m_pDoc->DisplayNetErrorMessage((LPSTR)lpvMsg);
			LocalFree(lpvMsg);
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
// NEVER CALLED ?????????????????????????????????????????///
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
#ifdef _DEBUG
						m_pDoc->DisplayGeneralMessage("@@ Starting send: DCX.BIN @@");
#endif
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
#ifdef _DEBUG
						m_pDoc->DisplayGeneralMessage("@@ Starting send: CONTROLS @@");
#endif
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
#ifdef _DEBUG
					m_pDoc->DisplayGeneralMessage("@@ Sending DCX Setup @@");
#endif
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

          // Cast the recieve buffer into the control data structure

					ctrld = (CONTROLDATA *)m_chNetBufferIn;

					// Do some error checking to keep the program from crashing
					// The control number should never be MAX_NUM_CONTROLS or greater
					// and we are currently limited to 80 channels

					if( (ctrld->wCtrl < MAX_NUM_CONTROLS) && (ctrld->wChannel < 80) )	
					{
							// Save the value of this control on this channel (module)
              // so that other clients have access to its settings

							m_wCurrentState[ctrld->wChannel][ctrld->wCtrl] = ctrld->wVal;

							// Resend the control data to all other clients so that they can update their displays

							m_pDoc->m_pdcxNetwork->BroadcastMsgType(ctrld, sizeof(CONTROLDATA), DCX_TCP_CONTROL_DATA, psocket, ALL);

              // Get the pot vaules from the dcx.bin file
              // (GOOD FUNCTION TO OPTIMIZE)

              if(m_pDoc->m_dcxBinTable.RemapControlData(ctrld, &dcxCtrlData))
							{
//								m_pDoc->DisplayGeneralMessage("&& Msg Data &&");

							/////////////////////////////////////////////////////////////////////////////
							// Loop thru all the pots for this control and write the XXW to the DCX

								for(iCount = 0; iCount < dcxCtrlData.iPotCount; iCount++)    
								{
                  if( ! m_pDoc->m_pDCXDevice->Write(dcxCtrlData.arPotData[iCount].iAddr,
																							dcxCtrlData.arPotData[iCount].szData, &ulWrite, FALSE) )
                  {
										m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_WRITE);
										break;
									}                                          
                 

                  //////////////////////////////////////////////////////////////////
                  // Delay between writing a control value and reading the response

//                  CTekSleep(m_pDoc->m_dwBasedelay,m_pDoc->m_dwCtrldelay);		// Delay 7 ms so that we can read our response

                    // Read the response which should be !ccggp\n. We do nothing with this
										// cc - target chip, gg - group on the DCX board the chip is in
										// p - the target pot

										lstrcpy(chBuffer,dcxCtrlData.arPotData[iCount].szData); // must pass the address in string
										m_pDoc->m_pDCXDevice->Read(chBuffer, 64, &ulIO);

										// If checkbox is checked on Control Test Dialog then
										// lets show what the client sent us
										if(iShowControlData)
											m_pDoc->DisplayGeneralMessage(dcxCtrlData.arPotData[iCount].szData);

								}
//								m_pDoc->DisplayGeneralMessage("&& Msg Data End &&");
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


				//////////////////////////////////////////////
				// Set the lock byte for each module

				case DCX_SHOW_VU_DATA:		// DCX_SHOW_VU_DATA

        	int			i;

					// Loop thru data received for showing the VU data and add it
					// to the Lock.  Client must turn OFF showing VU's when it EXITS

					for(i=0;i<MAX_VU_READ_DATA;i++)
					{		
            // If we know the module exists then set the lock 

            if(m_pDoc->m_dcxdevMap.GetModuleType(i) != DCX_DEVMAP_MODULE_NA)
            {
              // Get lock info into our vu data structure used in the vu thread


							// Also needs to be in the socket locks to turn them off
							// when client is closed

							m_pDoc->m_SocketVULocks[psocket->iSocketNumber][i] += m_chNetBufferIn[i];

              // validate the VU information on per Socket basis
						  if(m_pDoc->m_SocketVULocks[psocket->iSocketNumber][i] < 0)
								m_pDoc->m_SocketVULocks[psocket->iSocketNumber][i] = 0;
              else
                // the data is valid so we can use it
                m_pDoc->m_VUMetersArray.m_aVUReadData[i].cLock+=m_chNetBufferIn[i];
              
              // Take into account that its not a perfect world.  Clients open/close and
							// so can the server.  Everything may not be in sync. The total lock should
							// never be below zero.
						  if(m_pDoc->m_VUMetersArray.m_aVUReadData[i].cLock < 0)
                m_pDoc->m_VUMetersArray.m_aVUReadData[i].cLock = 0;



#ifdef _DEBUG
						wsprintf(chBuffer,"%d",m_pDoc->m_VUMetersArray.m_aVUReadData[i].cLock);
							TRACE0(chBuffer);
#endif

            }
				}

        // Update our views so bitmaps indicate which modules are 
        // sending VU data

        m_pDoc->UpdateAllViews(NULL);


#ifdef _DEBUG
						TRACE0("\n");
#endif
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


// Moved this here from class declaration
//This is the simplest way to ensure multitasking correctness. It might not be
//the best(!) performance, but it is efficient enough to be able to broadcast
//1100 messages a second.

char                    m_chNetBufferOut[MAX_NET_BUFFER+sizeof(HDR_DCXTCP)];

		if( iSize <= MAX_NET_BUFFER) 
    {
			hdrDCXTcp.iID = DCX_TCP_ID;
			hdrDCXTcp.wMessage  = uiType;
			hdrDCXTcp.wSize = iSize ;					// + sizeof(HDR_DCXTCP); Hristo
			hdrDCXTcp.mmt.wType = TIME_MS;
//			hdrDCXTcp.mmt.u.ms = GetTickCount();
    
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
				else if (uiWho == ALL)
				{			// uiWho == ALL so send to all other clients

// Loop thru all the socket connections, 1-8

					for(iCount = 0; iCount < MAX_ASYNC_SONNECTIONS; iCount++)
					{
						pSocket = (CCorTekAsyncSocket*)m_connections[iCount];

						if( pSocket->IsOpened() &&				// socket is opened
							  (pSocket != pCurrentSocket)		// and not the current socket
							) 														
						{

              // If its VU data and client accepts vu data then send it

							if( (uiType == DCX_VU_DATA) && pSocket->DoesAcceptVuData()) 
							{
								iSent=pSocket->Send(m_chNetBufferOut, iSize + sizeof(HDR_DCXTCP));
	
                // If it all didn't go out then report an error

							  if(iSent != iSize + (int) sizeof(HDR_DCXTCP) )
							  {
									bRet = FALSE;
	  								m_bSendingVUData[iCount]=FALSE;	// Display that VU data did not go out
									m_pDoc->DisplayGeneralMessage("*** BroadcastMsgType: incomplete ***");
							  }
							  else
	  								m_bSendingVUData[iCount]=TRUE;	// Display that VU data went out

							}
 							else if(uiType != DCX_VU_DATA)  // If it is not VU data then just send it
							{
								iSent=pSocket->Send(m_chNetBufferOut, iSize + sizeof(HDR_DCXTCP));

                // If it all didn't go out then report an error

							  if(iSent != iSize + (int) sizeof(HDR_DCXTCP) )
							  {
											  bRet = FALSE;
											  m_pDoc->DisplayGeneralMessage("*** BroadcastMsgType: incomplete ***");
							  }


							}

						} // end if not current socket and socke opened
					} // end for iCount
				} // sending to ALL?
      } // if iSize < MAX_NET_BUFFER
			else
			{
					m_pDoc->DisplayGeneralMessage("BroadcastMsgType: Size too big");
			}
return bRet;
}

