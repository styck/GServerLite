// DCXReadRegistry.h: interface for the CDCXReadRegistry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCXREADREGISTRY_H__7992CECA_2CE1_11D2_A3C6_00A0C96FD100__INCLUDED_)
#define AFX_DCXREADREGISTRY_H__7992CECA_2CE1_11D2_A3C6_00A0C96FD100__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDCXReadRegistry  
{
public:
static	BOOL GetDCXDelayData(DWORD * m_dwBasedelay,DWORD * m_dwVudelay,DWORD * m_dwCtrldelay);
	CDCXReadRegistry();
	virtual ~CDCXReadRegistry();

};

#endif // !defined(AFX_DCXREADREGISTRY_H__7992CECA_2CE1_11D2_A3C6_00A0C96FD100__INCLUDED_)
