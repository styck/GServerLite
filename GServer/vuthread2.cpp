//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// VUthread.cpp : implementation file
//

#include "stdafx.h"
#include  "mmsystem.h"
#include "GServer.h"
#include "VUthread2.h"
#include "gserverdoc.h"
#include	"VUMeterArray.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define	RW_DELAY	2


////////////////////////////////////////////
//
// Delay by milliseconds.
// 
// The value for timeGetTime() wraps around to 0 every 2^32 milliseconds (49.71 days)
// That is why the abs() call is there to check the difference
//
// The default precision can be 5 ms or more for windows NT, for window 95 it is 1 ms.
// The precision is set in the GDCXNetwork.cpp file in the StartAsServer() function
//
void    CTekSleep(DWORD dwDelay )
{
  DWORD  dwStartTime;

  dwStartTime = timeGetTime();
  while(abs( (timeGetTime() - dwStartTime) < dwDelay));
};

/////////////////////////////////////////////////////////////////////////////

UINT ThreadProc(LPVOID pParam)
{
	// TODO: Add your specialized code here and/or call the base class

	
BOOL	bErr;
int		iAddr;
char	chClip1[4];
char	chClip2[4];

char	chPeakVU[5];
char	chAverageVU[5];

char	chBufferVUType[64];
char	chBufferVUType2[64];

char    chBuffer1[64];
char    chBuffer2[64];



ULONG   ulIO;
int     iVUtoRead;
VU_READ *pVUData;		// Data structure for our VU data


CGServerDoc*	m_pDoc = (CGServerDoc*)pParam;

	if(m_pDoc != NULL)
	{

		/////////////////////////////////////////////////////////////////////////
		// Loop until we get an EventKill from someone
		// Read the VU's for each module and place in its associated array

		while(TRUE)
		{

// Look for the EventKill to exit the thread

			if(WaitForSingleObject(m_pDoc->m_hEventKillVUThread,0) == WAIT_OBJECT_0)
					break;

// Build the VU meter read command based on user selections 

			// Set a loop to go through all VU ...

			iVUtoRead = m_pDoc->m_VUMetersArray.GetFirstReadIdx();

			while(iVUtoRead >= 0)
			{
				// Get a pointer to this VU data

				pVUData = m_pDoc->m_VUMetersArray.GetDataPtr(iVUtoRead);

				// Set the module address

				iAddr = pVUData->iAddr;
		
				bErr = FALSE;		

#ifdef NOTUSED
				char szTemp[80];
        wsprintf(szTemp,"%d %d %d %d ",iVUtoRead,pVUData->iAddr, pVUData->iVUType, pVUData->iLock);
        m_pDoc->DisplayGeneralMessage(szTemp);
#endif

				if(pVUData->iLock == 1)	// Use this to determine if we should read this
				{

					// Depending on the type of VU, send commands to DCX
					// Comments are for INPUT MODULE ONLY

					switch(pVUData->iVUType)
					{
						// Pre fader VU
						case 1:	
								wsprintf(chBufferVUType, "*%03dXVUa\n", iAddr);			// PRE VU
								wsprintf(chBuffer1, "*%03dXVU0\n", iAddr);					// Peak
								wsprintf(chBuffer2, "*%03dXVU1\n", iAddr);					// Avg.
						break;
				
						// Post fader VU
						case 2:	
								wsprintf(chBufferVUType, "*%03dXVUb\n", iAddr);			// POST VU
								wsprintf(chBuffer1, "*%03dXVU2\n", iAddr);					// Peak 
								wsprintf(chBuffer2, "*%03dXVU3\n", iAddr);					// Avg. 
						break;
				
						// Comp fader VU
						case 3:	
						case 7:	
								wsprintf(chBufferVUType, "*%03dXVUc\n", iAddr);			// COMP VU
								wsprintf(chBuffer1, "*%03dXVU4\n", iAddr);					// Peak
								// Comp/Gate Averages not needed
								wsprintf(chBufferVUType2, "*%03dXVUd\n", iAddr);		// NO COMP VU
								wsprintf(chBuffer2, "*%03dXVU5\n", iAddr);					// Avg. 
						break;
				
						// Gate fader VU
						case 4:	
						case 8:	
								wsprintf(chBufferVUType, "*%03dXVUe\n", iAddr);		// GATE VU
								wsprintf(chBuffer1, "*%03dXVU6\n", iAddr);				// Peak
								// Comp/Gate Averages not needed
								wsprintf(chBufferVUType2, "*%03dXVUf\n", iAddr);	// NO GATE VU
								wsprintf(chBuffer2, "*%03dXVU7\n", iAddr);				// Avg. 
						break;
				
						default:
							bErr = TRUE;
						break;
				
					}

	///////////////////////////////////////////////////////////////////////

					if( ! bErr)
					{

	///////////////////////////////////////////////////////////////////////
	// Handle pre and post the same

						if((pVUData->iVUType == 1) || (pVUData->iVUType == 2))
						{

	///////////////////////////////////////////////////////////////////////
	// Write VU0 read command for PEAK data

								m_pDoc->m_pDCXDevice->Write(iAddr, chBuffer1, &ulIO);

						// Delay between write and read as selected by user

                CTekSleep(RW_DELAY);

						// Read VU0 peak data

								m_pDoc->m_pDCXDevice->Read(chBuffer1, 64, &ulIO);

	///////////////////////////////////////////////////////////////////////
	// Write VU1 read command for AVERAGE data

								m_pDoc->m_pDCXDevice->Write(iAddr, chBuffer2, &ulIO);

						// Delay between write and read as selected by user

                CTekSleep(RW_DELAY);
								
						// Read VU1 average data

								m_pDoc->m_pDCXDevice->Read(chBuffer2, 64, &ulIO);

						}

	//////////////////////////////////////////////////////////////////
	// Handle comp and gate the same
						if(pVUData->iVUType == 3 || pVUData->iVUType == 7) 
						{
	///////////////////////////////////////////////////////////////////////
	// Write VU read command for Peak data

								m_pDoc->m_pDCXDevice->Write(iAddr, chBuffer1, &ulIO);

						// Delay between write and read as selected by user

                CTekSleep(RW_DELAY);

						// Read VU peak data

								m_pDoc->m_pDCXDevice->Read(chBuffer1, 64, &ulIO);

						}
              
             if (pVUData->iVUType == 4 || pVUData->iVUType == 8)
						{

	///////////////////////////////////////////////////////////////////////
	// Write VU read command for Peak data

								m_pDoc->m_pDCXDevice->Write(iAddr, chBuffer1, &ulIO);

						// Delay between write and read as selected by user

                CTekSleep(RW_DELAY);

						// Read VU peak data

								m_pDoc->m_pDCXDevice->Read(chBuffer1, 64, &ulIO);


	///////////////////////////////////////////////////////////////////////
	// Write VU read command for Average data

								m_pDoc->m_pDCXDevice->Write(iAddr, chBuffer2, &ulIO);

						// Delay between write and read as selected by user

                CTekSleep(RW_DELAY);
								
						// Read VU average data

								m_pDoc->m_pDCXDevice->Read(chBuffer2, 64, &ulIO);

						}

	////////////////////////////////////////////////////

					// Now parse the data and dipslay it ...
					// "!0000/000"  ... this parsing needs
					// to be done better .. maybe
					//--------------------------------------

										chPeakVU[0] = chBuffer1[1];
										chPeakVU[1] = chBuffer1[2];
										chPeakVU[2] = chBuffer1[3];
										chPeakVU[3] = chBuffer1[4];
										chPeakVU[4] = 0;

										chClip1[0] = chBuffer1[6];
										chClip1[1] = chBuffer1[7];
										chClip1[2] = chBuffer1[8];
										chClip1[3] = 0;
  
										chAverageVU[0] = chBuffer2[1];
										chAverageVU[1] = chBuffer2[2];
										chAverageVU[2] = chBuffer2[3];
										chAverageVU[3] = chBuffer2[4];
										chAverageVU[4] = 0;

										chClip2[0] = chBuffer2[6];
										chClip2[1] = chBuffer2[7];
										chClip2[2] = chBuffer2[8];
										chClip2[3] = 0;


				// Set the VU Peak and Average data

										pVUData->iVUPeakValue = atoi(chPeakVU);

										if(pVUData->iVUPeakValue < 0 || pVUData->iVUPeakValue > 4095)
											pVUData->iVUPeakValue = 0;

										pVUData->iVUAverageValue = atoi(chAverageVU);

										if(pVUData->iVUAverageValue < 0 || pVUData->iVUAverageValue > 4095)
											pVUData->iVUAverageValue = 0;

				// Set clip values

										pVUData->iPeakClipValue = atoi(chClip1);

										if(pVUData->iPeakClipValue < 0 || pVUData->iPeakClipValue > 4095)
											pVUData->iPeakClipValue = 0;

										pVUData->iAverageClipValue = atoi(chClip2);

										if(pVUData->iAverageClipValue < 0 || pVUData->iAverageClipValue > 4095)
											pVUData->iAverageClipValue = 0;
					}
					else
					{
									pVUData->iVUPeakValue = 0;
									pVUData->iVUAverageValue = 0;

							// Set clip values

									pVUData->iPeakClipValue = 0;
									pVUData->iAverageClipValue = 0;
					}

					// Send the vu data to the clients
					// only if we didn't get and ACK character from the read
					// since the client is sending control data and we are not
					// locking our resource at this time because of speed

//					if( (chPeakVU[1] != '$') || (chAverageVU[1] != '$') )

	          m_pDoc->m_pdcxNetwork->BroadcastMsgType(pVUData, sizeof(VU_READ), DCX_VU_DATA, NULL, 0);

				}

				// Get the index to the next VU meter data

					iVUtoRead = m_pDoc->m_VUMetersArray.GetNextReadIdx(iVUtoRead);

			}

		} // END WHILE
	}

	SetEvent(m_pDoc->m_hEventVUThreadKilled);		// Tell the GServerDOC destructor that we have died

	return 0;
}



