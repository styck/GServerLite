// VUMeterArray.h: interface for the CVUMeterArray class.
//
// Copyright 1998 CorTek Software, Inc.

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VUMETERARRAY_H__70C1BE44_978F_11D1_94D1_00608CF29120__INCLUDED_)
#define AFX_VUMETERARRAY_H__70C1BE44_978F_11D1_94D1_00608CF29120__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DCXparser.h"

class CVUMeterArray : public CObject  
{
#define		MAX_VU_READ_DATA		256
// define some offsets here within the global array of VU meters
// ... Francis ... ask me about this if I forget to tell you !!!!

public:
	VU_READ * GetDataPtr(int iIndex);
	int GetNextReadIdx(int iNext);
	int GetFirstReadIdx();
	VU_READ			m_aVUReadData[MAX_VU_READ_DATA];
	CVUMeterArray();
	virtual ~CVUMeterArray();

private:
	int m_iCurrentReadIndex;

};

#endif // !defined(AFX_VUMETERARRAY_H__70C1BE44_978F_11D1_94D1_00608CF29120__INCLUDED_)
