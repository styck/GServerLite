#if !defined(AFX_CORTEKASYNCSOCKET_H__09D6C138_75C5_11D1_A3B4_22EA62000000__INCLUDED_)
#define AFX_CORTEKASYNCSOCKET_H__09D6C138_75C5_11D1_A3B4_22EA62000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CorTekAsyncSocket.h : header file

#include "DCXBinTable.h"

class     CGDCXNetwork; // external class

/////////////////////////////////////////////////////////////////////////////
// CCorTekAsyncSocket command target

class CCorTekAsyncSocket : public CAsyncSocket 
{
// Attributes
public:
			short		State;				// Current state of the socket
			CString	Inbuf;
			char		SendBuf[MAX_NET_BUFFER+sizeof(HDR_DCXTCP)];
			int			HowManyInSendBuf;
			int			OffsetToSend;
      int     iSocketNumber;  // Keep track of the socket number 0-7

			HANDLE	m_hFile;
			BOOL		m_bSendingControlStates;

// Operations
public:
	CCorTekAsyncSocket(CGDCXNetwork * pnet);
	virtual ~CCorTekAsyncSocket();

  BOOL    Abort();
  BOOL    SetOptions(void);
  BOOL    IsOpened(void); 
  BOOL    Init(void);

  void    AcceptVuData(BOOL b);
  BOOL    DoesAcceptVuData(void);

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCorTekAsyncSocket)
	public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CCorTekAsyncSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
  CGDCXNetwork * m_pNet; // the Owner of this class ... 
  BOOL           m_bOpened;
  BOOL           m_bSendVuData;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CORTEKASYNCSOCKET_H__09D6C138_75C5_11D1_A3B4_22EA62000000__INCLUDED_)
