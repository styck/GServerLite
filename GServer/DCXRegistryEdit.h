// DCXRegistryEdit.h: interface for the CDCXRegistryEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCXREGISTRYEDIT_H__100BD2BD_2E6E_11D2_A3C8_00A0C96FD100__INCLUDED_)
#define AFX_DCXREGISTRYEDIT_H__100BD2BD_2E6E_11D2_A3C8_00A0C96FD100__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDCXRegistryEdit  
{
public:
	static BOOL SetDCXDelayData(DWORD * m_dwBasedelay, DWORD * m_dwVudelay, DWORD * m_dwCtrldelay);
	static BOOL GetDCXDelayData(DWORD * m_dwBasedelay, DWORD * m_dwVudelay, DWORD * m_dwCtrldelay);
	CDCXRegistryEdit();
	virtual ~CDCXRegistryEdit();

};

#endif // !defined(AFX_DCXREGISTRYEDIT_H__100BD2BD_2E6E_11D2_A3C8_00A0C96FD100__INCLUDED_)
