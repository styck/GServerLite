// GDCXNetwork.h: interface for the CGDCXNetwork class.
//
// Copyright 1998, CorTek Software, Inc.

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDCXNETWORK_H__09D6C136_75C5_11D1_A3B4_22EA62000000__INCLUDED_)
#define AFX_GDCXNETWORK_H__09D6C136_75C5_11D1_A3B4_22EA62000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class     CGServerDoc; // external class

#include "CorTekAsyncSocket.h"
#include "DCXDeviceMap.h"

class CGDCXNetwork : public CObject  
{
#define     NETWORK_VERSION           0x00010000
#define     NETWORK_OBJ_ID            "CT_NET_OBJ"// CorTek Network Object
#define     MAX_ASYNC_SONNECTIONS     8

#define     MAX_NET_BUFFER            488		// also defined in dcxtcp_protocol.h

#define			ALL				0						// flag to broadcast to ALL clients
#define			CURRENT		1						// flag to broadcast to CURRENT client requesting data

public:
	CGDCXNetwork(CGServerDoc *pDoc);
	virtual ~CGDCXNetwork();

private:
	CDCXDeviceMap						*m_pDeviceMap;

  char                    m_chNetBufferOut[MAX_NET_BUFFER+sizeof(HDR_DCXTCP)];
  char                    m_chNetBufferIn[MAX_NET_BUFFER+sizeof(HDR_DCXTCP)];
	
	
public:
  CGServerDoc             *m_pDoc;
  CCorTekAsyncSocket      *m_pAssListener;
  int                     m_iPort;					// TCP/IP port to transmit on
  CString                 m_csIPAddress;		// IP address
  BOOL                    m_bIsClient;			// Flag for client

	// saves current state of control for a particular module
	// [module][control number] = control value

	WORD										m_wCurrentState[DCX_DEVMAP_MAXSIZE][512];

  // Array of Connection Objects
  //----------------------------
	CPtrArray               m_connections;
  int                     m_iConnInUse;


public:
  // General Network Interface Funvtions
  //------------------------------------
  BOOL    IsStartedAsClient(void);
  BOOL    IsStartedAsServer(void);

  void    OnAcceptConnection(int nErrorCode);
  void    OnCloseConnection(int nErrorCode);
  void    OnConnect(int nErrorCode);

  void    OnReceive(int nErrorCode, CCorTekAsyncSocket *psocket);

  BOOL    StartAsServer(void);
  BOOL    StartAsServer(LPCTSTR lpcs, UINT iPort);

  BOOL    InitClients(void);
  BOOL    InitListener(void);
  BOOL    ShutDown(void);    
  BOOL    ShutDownClients(void);    

  BOOL    SendMsgType(void* lpv, int iSize, UINT  uiType);
  BOOL    BroadcastMsgType(void* lpv, int iSize, UINT  uiType, CCorTekAsyncSocket *pCurrentSocket, UINT uiWho);


  void    Serialize(CArchive& ar); // Serialiaze this Object

};

#endif // !defined(AFX_GDCXNETWORK_H__09D6C136_75C5_11D1_A3B4_22EA62000000__INCLUDED_)
