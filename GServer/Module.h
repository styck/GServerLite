#if !defined(AFX_MODULE_H__B7305527_BAB5_11D1_BED7_00608CF29120__INCLUDED_)
#define AFX_MODULE_H__B7305527_BAB5_11D1_BED7_00608CF29120__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Module.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModule 

class CModule : public CObject
{
// Construction
public:
	CModule();

// Attributes
public:

// Operations
public:

// Implementation
public:
	UINT GetModuleAddress(UINT iModule);
	UINT GetModuleNumber(UINT iAddr);
	virtual ~CModule();

	// Generated message map functions
protected:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULE_H__B7305527_BAB5_11D1_BED7_00608CF29120__INCLUDED_)
