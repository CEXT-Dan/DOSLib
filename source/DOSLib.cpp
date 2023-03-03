/////////////////////////////////////////////////////////////////////////////
// DOSLib.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include <afxdllx.h>

// Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(DOSLibDLL)

// Please do not remove the 3 following lines. These are here to make .NET MFC Wizards
// running properly. The object will not compile but is require by .NET to recognize
// this project as being an MFC project
#ifdef NEVER
AFX_EXTENSION_MODULE DOSLibExtDLL = { NULL, NULL };
#endif

// Now you can use the CAcModuleResourceOverride class in
// your application to switch to the correct resource instance.
// Please see the ObjectARX Documentation for more details

HINSTANCE _hdllInstance = 0;

// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    // Remove this if you use lpReserved
    UNREFERENCED_PARAMETER(lpReserved);
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            _hdllInstance = hInstance;
            DOSLibDLL.AttachInstance(hInstance);
        }
        break;
        case DLL_PROCESS_DETACH:
        {
            _hdllInstance = nullptr;
            DOSLibDLL.DetachInstance();
        }
        break;
    }
    return TRUE;
}
