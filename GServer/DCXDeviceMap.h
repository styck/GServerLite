// DCXDeviceMap.h: interface for the CDCXDeviceMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCXDEVICEMAP_H__68075F64_7DE6_11D1_A3B5_6663AF000000__INCLUDED_)
#define AFX_DCXDEVICEMAP_H__68075F64_7DE6_11D1_A3B5_6663AF000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDCXDeviceMap : public CObject  
{

// #define DCX_DEVMAP_MODULE_COMPUTER  100


// #define DCX_DEVMAP_COMPUTER_MODULE_IDX  19
// #define DCX_DEVMAP_COMPUTER_MODULE		DCX_DEVMAP_COMPUTER_MODULE_IDX+1

public:
  CDWordArray   m_dwarrMap;

public:
	DWORD * GetData();
	CDCXDeviceMap();
	virtual ~CDCXDeviceMap();
  
	virtual void Serialize(CArchive& ar);

  void    SetDefaults(void)
  {
    int   iCount;
    
    // setup some default values
    //--------------------------
    for(iCount = 0; iCount < DCX_DEVMAP_MAXSIZE; iCount ++)
      m_dwarrMap[iCount] = DCX_DEVMAP_MODULE_NA;  

    // Set the Computer module default position
    //-----------------------------------------
	// Jim Gamble doesn't want computer shown
	//    m_dwarrMap[DCX_DEVMAP_COMPUTER_MODULE_IDX] = DCX_DEVMAP_MODULE_COMPUTER;
    
  };

  DWORD   GetModuleType(DWORD dwIdx)
					{ 
						return m_dwarrMap[dwIdx];
					};

  void    SetModuleType(DWORD dwIdx, int iType){ m_dwarrMap[dwIdx] = iType;};
};

#endif // !defined(AFX_DCXDEVICEMAP_H__68075F64_7DE6_11D1_A3B5_6663AF000000__INCLUDED_)
