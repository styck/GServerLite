//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// VUMeterArray.cpp: implementation of the CVUMeterArray class.
//
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GServer.h"
#include "GServerDoc.h"
#include "VUMeterArray.h"
#include "Module.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CVUMeterArray::CVUMeterArray()
{

  CModule   cModule;

// Initialize our VU data structure

	m_iCurrentReadIndex = 0;

	for(int i=0;i<MAX_VU_READ_DATA;i++)
	{
		m_aVUReadData[i].wAddr=cModule.GetModuleAddress(i);							// the VUthread Reads from here(module address)
		m_aVUReadData[i].cLock=0;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
		m_aVUReadData[i].wPeakClipValue=0;		// data
    m_aVUReadData[i].wModuleIdx = i;

	}

}


CVUMeterArray::~CVUMeterArray()
{

}


int CVUMeterArray::GetFirstReadIdx()
{
	return 0;
}

////////////////////////////////////////////
// Returns an index from 0-80 into the 
// vu data array.

int CVUMeterArray::GetNextReadIdx(int iNext)
{

	m_iCurrentReadIndex++;
  if(m_iCurrentReadIndex >= MAX_VU_READ_DATA)
  {
    m_iCurrentReadIndex = 0;
  }
  else
  {
    while(m_aVUReadData[m_iCurrentReadIndex].cLock == 0)
    {
      m_iCurrentReadIndex ++;  
      if(m_iCurrentReadIndex >= MAX_VU_READ_DATA)
      {
    		m_iCurrentReadIndex = 0;
        return 0xffffffff;
      }
    }
  }

  // Franchesko please check this logic again !! ???? Hristo
  //
  // If we get to the last one and it is not viewable then return -1

  if(m_aVUReadData[m_iCurrentReadIndex].cLock == 0)
  {
    return 0xffffffff;
  }
  return	m_iCurrentReadIndex;
}

VU_READ * CVUMeterArray::GetDataPtr(int iIndex)
{
	return &m_aVUReadData[iIndex];
}

