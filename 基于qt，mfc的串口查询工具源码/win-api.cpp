#define _AFXDLL

#include"win-api.h"
#include<SetupAPI.h>

//---------------------------------------------------------------
// Routine for enumerating the available serial ports.
// Throws a CString on failure, describing the error that
// occurred. If bIgnoreBusyPorts is TRUE, ports that can't
// be opened for read/write access are not included.

void EnumSerialPorts(CArray<SSerInfo, SSerInfo&>& asi, BOOL bIgnoreBusyPorts)
{
    // Clear the output array
    asi.RemoveAll();

    // Use different techniques to enumerate the available serial
    // ports, depending on the OS we're using
    OSVERSIONINFO vi;//OSVERSIONINFO 能够包含系统版本信息的结构体。
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (!::GetVersionEx(&vi)) {
        CString str;
        str = "Could not get OS version. (err=%lx)";
        throw str;
    }
    // Handle windows 9x and NT4 specially
//    if (vi.dwMajorVersion < 5) {
        //if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
            //EnumPortsWNt4(asi);
        //else
            //EnumPortsW9x(asi);
//    }
//    else {
        // Win2k and later support a standard API for
        // enumerating hardware devices.
    EnumPortsWdm(asi);
    //    }

    for (int ii = 0; ii < asi.GetSize(); ii++)
    {
        SSerInfo& rsi = asi[ii];
        if (bIgnoreBusyPorts) {
            // Only display ports that can be opened for read/write
            HANDLE hCom = CreateFile(rsi.strDevPath,
                GENERIC_READ | GENERIC_WRITE,
                0,    /* comm devices must be opened w/exclusive-access */
                NULL, /* no security attrs */
                OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
                0,    /* not overlapped I/O */
                NULL  /* hTemplate must be NULL for comm devices */
            );
            if (hCom == INVALID_HANDLE_VALUE) {
                // It can't be opened; remove it.
                asi.RemoveAt(ii);
                ii--;
                continue;
            }
            else {
                // It can be opened! Close it and add it to the list
                ::CloseHandle(hCom);
            }
        }

        // Come up with a name for the device.
        // If there is no friendly name, use the port name.
        if (rsi.strFriendlyName.IsEmpty())
            rsi.strFriendlyName = rsi.strPortName;

        // If there is no description, try to make one up from
        // the friendly name.
        if (rsi.strPortDesc.IsEmpty()) {
            // If the port name is of the form "ACME Port (COM3)"
            // then strip off the " (COM3)"
            rsi.strPortDesc = rsi.strFriendlyName;
            int startdex = rsi.strPortDesc.Find(CString(" ("));
            int enddex = rsi.strPortDesc.Find(CString(")"));
            if (startdex > 0 && enddex ==
                (rsi.strPortDesc.GetLength() - 1))
                rsi.strPortDesc = rsi.strPortDesc.Left(startdex);
        }
    }
}




// Helpers for EnumSerialPorts

void EnumPortsWdm(CArray<SSerInfo, SSerInfo&>& asi)
{
    CString strErr;
    // Create a device information set that will be the container for
    // the device interfaces.
    GUID* guidDev = (GUID*)&GUID_CLASS_COMPORT;

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DETAIL_DATA* pDetData = NULL;

    try {
        //SetupDiGetClassDevs 函数返回一个设备信息集的句柄，该设备信息集包含本地计算机请求的设备信息元素。
        //DIGCF_PRESENT Return only devices that are currently present in a system.
        hDevInfo = SetupDiGetClassDevs(guidDev,
            NULL,
            NULL,
            DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

        if (hDevInfo == INVALID_HANDLE_VALUE)
        {
            //            strErr.Format("SetupDiGetClassDevs failed. (err=%lx)");
                            //GetLastError（） Retrieves the calling thread's last-error code value
            throw strErr;
        }

        // Enumerate the serial ports
        BOOL bOk = TRUE;
        SP_DEVICE_INTERFACE_DATA ifcData;
        DWORD dwDetDataSize =( sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256)*2;
        pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
        // This is required, according to the documentation. Yes,
        // it's weird.
        ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        for (DWORD ii = 0; bOk; ii++) {
            bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
                NULL, guidDev, ii, &ifcData);
            if (bOk) {
                // Got a device. Get the details.
                SP_DEVINFO_DATA devdata = { sizeof(SP_DEVINFO_DATA) };
                //get details about a device interface.
                bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
                    &ifcData, pDetData, dwDetDataSize, NULL, &devdata);
                if (bOk) {
                    CString strDevPath(pDetData->DevicePath);
                    // Got a path to the device. Try to get some more info.
                    TCHAR fname[256];
                    TCHAR desc[256];
                    //检索指定的（即插即用设备）属性。查询哪种属性由第三个参数指定
                    //这个查询the friendly name of a device.
                    BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
                        (PBYTE)fname, sizeof(fname), NULL);
                    //查询 the description of a device.
                    bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
                        (PBYTE)desc, sizeof(desc), NULL);
                    //查询the hardware location of a device.
                    BOOL bUsbDevice = FALSE;
                    TCHAR locinfo[256];
                    if (SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
                        (PBYTE)locinfo, sizeof(locinfo), NULL))
                    {
                        // Just check the first three characters to determine
                        // if the port is connected to the USB bus. This isn't
                        // an infallible method; it would be better to use the
                        // BUS GUID. Currently, Windows doesn't let you query
                        // that though (SPDRP_BUSTYPEGUID seems to exist in
                        // documentation only).
                        bUsbDevice = (strncmp((const char*)locinfo, "USB", 3) == 0);
                    }
                    if (bSuccess) {
                        // Add an entry to the array
                        SSerInfo si;
                        si.strDevPath = strDevPath;
                        si.strFriendlyName = fname;
                        si.strPortDesc = desc;
                        si.bUsbDevice = bUsbDevice;
                        asi.Add(si);
                    }
                    //下面都是错误处理和内存释放了
                }
                else {
                                        strErr.Format("SetupDiGetDeviceInterfaceDetail failed. (err=%lx)",
                                            GetLastError());
                    throw strErr;
                }
            }
            else {
                DWORD err = GetLastError();
                if (err != ERROR_NO_MORE_ITEMS) {
                    //                    strErr.Format("SetupDiEnumDeviceInterfaces failed. (err=%lx)", err);
                    throw strErr;
                }
            }
        }
    }
    catch (CString strCatchErr) {
        strErr = strCatchErr;
    }

    if (pDetData != NULL)
        delete[](char*)pDetData;
    if (hDevInfo != INVALID_HANDLE_VALUE)
        SetupDiDestroyDeviceInfoList(hDevInfo);//销毁一个信息释放集合，释放关联的内存

    if (!strErr.IsEmpty())
        throw strErr;
}
