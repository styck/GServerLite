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

#define NUM_VU_READ   4   // HARDCODE NUMBER OF VU'S TO READ : NOT USED ANYMORE

CVUMeterArray::CVUMeterArray()
{

  CModule   cModule;

// Initialize our VU data structure

	m_iCurrentReadIndex = 0;

	for(int i=0;i<MAX_VU_READ_DATA;i++)
	{
		m_aVUReadData[i].wAddr=cModule.GetModuleAddress(i);							// the VUthread Reads from here(module address)
		m_aVUReadData[i].cLock=0;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig

#ifdef NOTUSED
		m_aVUReadData[i].iVUValue[0]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[1]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[2]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[3]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[4]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[5]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[6]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[7]=0;			// data. VUthread writes here
#endif
		m_aVUReadData[i].wPeakClipValue=0;		// data
    m_aVUReadData[i].wModuleIdx = i;
	}

  /////////////////////////////////////////////////
  // Hardcode modules to read vu data from
#ifdef JUNK
	for(int j=0;j<NUM_VU_READ;j++)
	{
		m_aVUReadData[j].wAddr=33+j;							// the VUthread Reads from here(module address)
//		m_aVUReadData[j].iVUType=1;							// the VUthread Reads from here(Pre, Post, Comp, Gate)
		m_aVUReadData[j].wLock=1;							// if Zero nobody monitors this VU, so we don't need to read the damn thnig
#ifdef NOTUSED
		m_aVUReadData[i].iVUValue[0]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[1]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[2]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[3]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[4]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[5]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[6]=0;			// data. VUthread writes here
		m_aVUReadData[i].iVUValue[7]=0;			// data. VUthread writes here
#endif
		m_aVUReadData[j].wPeakClipValue=0;		// data
    m_aVUReadData[j].wModuleIdx = j;
	}
#endif

}

CVUMeterArray::~CVUMeterArray()
{

}

int CVUMeterArray::GetFirstReadIdx()
{
	return 0;
}

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
  if(m_aVUReadData[m_iCurrentReadIndex].cLock == 0)
  {
    return 0xffffffff;
  }

/*
	if(m_iCurrentReadIndex >= NUM_VU_READ)
	{
		m_iCurrentReadIndex = 0;
		return 0xffffffff;
	}
	else
		return	m_iCurrentReadIndex;
*/
  return	m_iCurrentReadIndex;
}

VU_READ * CVUMeterArray::GetDataPtr(int iIndex)
{
	return &m_aVUReadData[iIndex];
}

