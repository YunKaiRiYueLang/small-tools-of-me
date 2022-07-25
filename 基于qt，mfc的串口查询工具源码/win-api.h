#pragma once
#ifndef WIN_INF_H
#define WIN_INF_H
#include<afxtempl.h>
#include<Windows.h>
#include <initguid.h>  
// The following define is from ntddser.h in the DDK. It is also
// needed for serial port enumeration.
#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, \
    0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif
struct SSerInfo {
    SSerInfo() : bUsbDevice(FALSE) {}
    CString strDevPath;          // Device path for use with CreateFile()
    CString strPortName;         // Simple name (i.e. COM1)
    CString strFriendlyName;     // Full name to be displayed to a user
    BOOL bUsbDevice;             // Provided through a USB connection?
    CString strPortDesc;         // friendly name without the COMx
};

void EnumPortsWdm(CArray<SSerInfo, SSerInfo&>& asi);
void EnumSerialPorts(CArray<SSerInfo, SSerInfo&>& asi, BOOL bIgnoreBusyPorts);
#endif // WIN_INF_H
