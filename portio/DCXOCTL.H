// dcxoctl.h    Include file for DCX Port I/O Driver
//
// Define the IOCTL codes we will use.  The IOCTL code contains a command
// identifier, plus other information about the device, the type of access
// with which the file must have been opened, and the type of buffering.
//
// Hristo Doichev 1997

// Device type           -- in the "User Defined" range."
#define GPD_TYPE 40000

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.

#define IOCTL_GPD_READ_PORT_UCHAR \
    CTL_CODE( GPD_TYPE, 0x900, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_GPD_READ_PORT_USHORT \
    CTL_CODE( GPD_TYPE, 0x901, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_GPD_READ_PORT_ULONG \
    CTL_CODE( GPD_TYPE, 0x902, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_GPD_READ_PORT_BUFFER \
    CTL_CODE( GPD_TYPE, 0x903, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_GPD_WRITE_PORT_UCHAR \
    CTL_CODE(GPD_TYPE,  0x910, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_GPD_WRITE_PORT_USHORT \
    CTL_CODE(GPD_TYPE,  0x911, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_GPD_WRITE_PORT_ULONG \
    CTL_CODE(GPD_TYPE,  0x912, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_GPD_WRITE_PORT_BUFFER \
    CTL_CODE(GPD_TYPE,  0x913, METHOD_BUFFERED, FILE_WRITE_ACCESS)



#define MAX_DCX_MESSAGE     512

#pragma pack(1)

typedef struct  _DCXPORT_WRITE_INPUT {
    ULONG   PortNumber;     // Port # to write to
                            // should be 0, 1, 2 or 3 
                            // the driver will add the base part
    union   {               // Data to be output to port
        ULONG   LongData;
        USHORT  ShortData;
        UCHAR   CharData;
    };
  UCHAR   Addr;
  ULONG  ulLength;                      // Valid data in the Buffer
  char   arChar[MAX_DCX_MESSAGE];       // Buffer to be output to port
}   DCXPORT_WRITE_INPUT;

#pragma pack()

