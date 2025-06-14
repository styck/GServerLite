// Write a byte to an port using the wkd driver.
// Robert R. Howell             January 6, 1993
// Robert B. Nelson (Microsoft) January 12, 1993
// Robert B. Nelson (Microsoft) April 5, 1993
// Robert B. Nelson (Microsoft) May 6, 1993

#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>
#include <conio.h>
#include "..\\gpioctl.h"        // This defines the IOCTL constants.



void __cdecl main(int argc, char **argv)
{
    char    ch;
    BOOL    bValid = FALSE;
    char    szSend[255];
    char   *pSend;

    // The following is returned by IOCTL.  It is true if the write succeeds.
    BOOL    IoctlResult;

    // The following parameters are used in the IOCTL call
    HANDLE              hndFile;        // Handle to device, obtain from CreateFile
    DCXPORT_WRITE_INPUT InputBuffer;    // Input buffer for DeviceIoControl
    LONG                IoctlCode;
    ULONG               DataValue;
    ULONG               DataLength;
    ULONG               ReturnedLength; // Number of bytes returned in output buffer

    memset(szSend, 0, 255);
    pSend = szSend;

    if (argc < 4)
    {
        printf("GpdWrite -b|-w|-d <Port#> <Value>\n\n");
        printf("        The byte (-b), word (-w), or doubleword (-d) specified\n");
        printf("        as value is written to the given port.  Ports are numbered\n");
        printf("        as 0, 1, ... relative to the base set in the driver or\n");
        printf("        the registry.  The default driver uses ports 300h through\n");
        printf("        303h.  All numbers are read in hex.\n");
        getch();
        exit(1);
    }


    hndFile = CreateFile(
                    "\\\\.\\DcxDev",           // Open the Device "file"
                    GENERIC_WRITE,
                    FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);

    if (hndFile == INVALID_HANDLE_VALUE)        // Was the device opened?
      {
      printf("Unable to open the device.\n");
      getch();
      exit(1);
      }

    sscanf(argv[2], "%x", &InputBuffer.PortNumber);     // Get the port number
    sscanf(argv[3], "%x", &DataValue);          // Get the data to be written.
//    sprintf(szSend, "%s\n", argv[3]);
    sprintf(InputBuffer.arChar, "%s\n", argv[3]);

    ch = argv[1][1];

    while(ch != 'q')
      {
      InputBuffer.PortNumber = 0; // reset it back
      switch ( ch )     
      {
        default:
        case 'b':
            IoctlCode = IOCTL_GPD_WRITE_PORT_BUFFER;
            InputBuffer.Addr = 33;
            InputBuffer.ulLength = strlen(InputBuffer.arChar);
            DataLength = sizeof(DCXPORT_WRITE_INPUT);
            bValid = TRUE;
            break;
  
        case 'c':
            IoctlCode = IOCTL_GPD_WRITE_PORT_UCHAR;
            InputBuffer.CharData = (UCHAR)DataValue;
            InputBuffer.ulLength = strlen(InputBuffer.arChar);
            DataLength = sizeof(DCXPORT_WRITE_INPUT);
//            DataLength = offsetof(GENPORT_WRITE_INPUT, CharData) +
//                         sizeof(InputBuffer.CharData);
            bValid = TRUE;
            break;
/*          
        case 'w':
            IoctlCode = IOCTL_GPD_WRITE_PORT_USHORT;
            InputBuffer.ShortData = (USHORT)DataValue;
            DataLength = offsetof(GENPORT_WRITE_INPUT, ShortData) +
                         sizeof(InputBuffer.ShortData);
            bValid = TRUE;
            break;

        case 'd':
            IoctlCode = IOCTL_GPD_WRITE_PORT_ULONG;
            InputBuffer.LongData = (ULONG)DataValue;
            DataLength = offsetof(GENPORT_WRITE_INPUT, LongData) +
                         sizeof(InputBuffer.LongData);
            bValid = TRUE;
            break;
*/
        }



      if( bValid )
        {
        InputBuffer.PortNumber = 0; // set it to CTRLREG   
        IoctlResult = DeviceIoControl(
                            hndFile,                // Handle to device
                            IoctlCode,              // IO Control code for Write
                            &InputBuffer,           // Buffer to driver.  Holds port & data.
                            DataLength,             // Length of buffer in bytes.
                            NULL,                   // Buffer from driver.   Not used.
                            0,                      // Length of buffer in bytes.
                            &ReturnedLength,        // Bytes placed in outbuf.  Should be 0.
                            NULL                    // NULL means wait till I/O completes.
                            );
 
        if (IoctlResult)                            // Did the IOCTL succeed?
          {
          printf("Wrote to port %x the data %s\n", InputBuffer.PortNumber, InputBuffer.arChar);
          }
        else
          {
          printf("Ioctl failed with code %ld\n", GetLastError() );
          printf("Attempted Write to port %x data %s\n", InputBuffer.PortNumber, InputBuffer.arChar);
          }

        memset(szSend, 0, 255);
        pSend = szSend;
        bValid = FALSE;
        }
      else
        printf("invalid key");

      printf("Press a key to continue(b=BYTE,w=WORD,d=DWORD,q=QUIT");
      ch = getch();
      }

    /////////////////////////////////////////////////////////////////////

    if (!CloseHandle(hndFile))                  // Close the Device "file".
      {
      printf("Failed to close device.\n");
      getch();
      }

    exit(0);
}
