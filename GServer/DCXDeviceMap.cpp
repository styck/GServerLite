//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// DCXDeviceMap.cpp: implementation of the CDCXDeviceMap class.
//
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GServer.h"
#include "DCXDeviceMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCXDeviceMap::CDCXDeviceMap()
{
	// allocate the space needed
	//--------------------------
	m_dwarrMap.SetSize(DCX_DEVMAP_MAXSIZE);		// Holds all the types of modules
	SetDefaults();
}

CDCXDeviceMap::~CDCXDeviceMap()
{
	m_dwarrMap.RemoveAll();
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: Serialize(CArchive& ar) 
//
//
//
void CDCXDeviceMap::Serialize(CArchive& ar)
{
CString   cs;
	if (ar.IsStoring())
	{	// storing code
		cs = DCX_DEVMAP_ID_STR;
		ar <<  cs;
		m_dwarrMap.Serialize(ar);  
	}
	else
	{	// loading code
		ar >>  cs;
		// Check if it is the correct ID .. so we can read safely
		//-------------------------------------------------------
		if(cs == DCX_DEVMAP_ID_STR)
		{
			m_dwarrMap.Serialize(ar);
		}
	}
}

DWORD * CDCXDeviceMap::GetData()
{
return m_dwarrMap.GetData();
}
