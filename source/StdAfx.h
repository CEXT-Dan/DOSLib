/////////////////////////////////////////////////////////////////////////////
// StdAfx.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

#pragma pack (push, 8)
#pragma warning(disable: 4786 4996)

#define STRICT

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN			    //  Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER
#define WINVER 0x0601
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0601
#endif

#ifndef _WIN32_IE				      // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0650		  // Change this to the appropriate value to target IE 5.5 or later.
#endif

//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include "ArxVer.h"           // Defines ARX and BRX versions

#include <map>                // ObjectARX and OMF headers needs this
#include <cmath>
#include <vector> 
#include <filesystem>

#include <afxwin.h>				    // MFC core and standard components
#include <afxext.h>				    // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>				    // MFC OLE classes
#include <afxodlgs.h>			    // MFC OLE dialog classes
#include <afxdisp.h>			    // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>				    // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>				    // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>			    // MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>				    // MFC support for Windows Common Controls


//#define _UNITTEST

#include "DosSettings.h"
#include "afxdialogex.h"
#include "CDLDialogEx.h"
#include "MfcExtensionModule.h"

extern HINSTANCE _hdllInstance;

#define WM_ACAD_KEEPFOCUS 1001

typedef struct _dlgControlTag {
    DWORD id;
    DWORD flags;
    DWORD pct;
} DLGCTLINFO, * PDLGCTLINFO;

#include <atlbase.h>          // ATL base classes
#include <atlimage.h>         // CImage support
#include <shlwapi.h>          // Shell Lightweight API
#include <objbase.h>          // Component object model defintions
#include <shellapi.h>         // Shell functions
#include <shlobj.h>           // Shell definitions
#include <direct.h>           // Directory handling/creation
#include <winnetwk.h>         // Standard WINNET Header File for WIN32
#include <psapi.h>            // Include file for APIs provided by PSAPI.DLL
#include <mmsystem.h>         // Include file for Multimedia API's
#include <afxmt.h>            // MFC Multithreaded Extensions
#include <vfw.h>              // Video for windows include file for WIN32
#include <gdiplus.h>          // GDI+ declarations
#include <winspool.h>         // Printing functions
#include <vector>             // Used by CDosReadDir
#include <htmlhelp.h>         // HTML help support
#include <winsock.h>          // Windows sockets
#include <Iphlpapi.h>         // IP Helper API
#include <uxtheme.h>          // CToasterWnd support
#include <cderr.h>            // for FNERR_BUFFERTOSMALL
#include <mshtmhst.h>         // MSTHML host interfaces
#include <math.h>             // Math functions
#include <limits.h>
#include <float.h>
#include <stack>


#include "DosMath.h"
#include "DosString.h"
#include "DosPathString.h"
#include "DosStringArray.h"
#include "DosAdsHelpers.h"
#include "Resource.h"

#define _TOOLKIT_IN_DLL_
#include "OdaCommon.h"
#include "OdArray.h"
#include "OdString.h"

#define STL_USING_ALL
#include "OdaSTL.h" 
#include "OdError.h"

#include "RxModule.h"
#include "RxDynamicModule.h"

#include "Ed/EdCommandStack.h"
#include "Ed/EdCommandContext.h"
#include "Ed/EdUserIO.h"

#include "DbCommandContext.h"
#include "DbUserIO.h"
#include "DbLayerTable.h"
#include "DbLayerTableRecord.h"
#include "DbBlockTable.h"
#include "DbBlockTableRecord.h"
#include "DbHostAppProgressMeter.h"
#include "DbHostAppServices.h"
#include "DbDictionary.h"
#include "DbLinetypeTable.h"

#include "summinfo.h"

#include "Db2dPolyline.h"
#include "DbPolyline.h"
#include "DbRay.h"
#include "DbHatch.h"
#include "DbOle2Frame.h"
#include "DbRasterImageDef.h"
#include "DbProxyEntity.h"

#include "XRefMan.h"

#include "DbCurve.h"
#include "Db3dSolid.h"

#include "ge/GeCompositeCurve2d.h"
#include "ge/GeCircArc2d.h"
#include "ge/GeLineSeg2d.h"

#include "StaticRxObject.h"
#include "SmartPtr.h"

#include "sds.h"
#include "drx.h"
#include "od_doc_man.h"


inline OdApDocument* curDoc()
{
    return odapDocManager()->curDocument();
}

inline OdDbDatabase* curDb()
{
    if (curDoc())
        return curDoc()->database();
    return nullptr;
}

#define acedRetNil sds_retnil
#define acedRetT sds_rett
#define acedRetStr sds_retstr
#define acedRetList sds_retlist
#define acedRetInt sds_retint
#define acedRetReal sds_retreal
#define acedRetVal sds_retval
#define acedRetPoint sds_retpoint

#define acutNewRb sds_newrb
#define acutRelRb sds_relrb
#define acutBuildList sds_buildlist
#define acedGetArgs sds_getargs

#define acutPrintf sds_printf
#define acedGetVar sds_getvar
#define acedSetVar sds_setvar


//-------------------------------------------------------------------------------------
//AcResBufPtr
using AcResBufPtr = std::unique_ptr < resbuf, decltype([](resbuf* ptr) noexcept
{
    if (ptr != nullptr)
        acutRelRb(ptr);
}) > ;

#pragma comment( lib , "odapi.lib" )
#pragma comment( lib , "bricscadapi.lib" )
#pragma comment( lib, "axbricscaddb1.lib" )
#pragma comment( lib, "ISM.lib" )
#pragma comment( lib, "TD_Alloc.lib" )
#pragma comment( lib, "TD_Db.lib" )
#pragma comment( lib, "TD_DbCore.lib" )
#pragma comment( lib, "TD_DbEntities.lib" )
#pragma comment( lib, "TD_DbIO.lib" )
#pragma comment( lib, "TD_DbRoot.lib" )
#pragma comment( lib, "TD_Ge.lib" )
#pragma comment( lib, "TD_Gi.lib" )
#pragma comment( lib, "TD_Gs.lib" )
#pragma comment( lib, "TD_jpeg.lib" )
#pragma comment( lib, "TD_RasterExport.lib" )
#pragma comment( lib, "TD_Root.lib" )
#pragma comment( lib, "TD_Sm.lib" )
#pragma comment( lib, "TG_Db.lib" )
#pragma comment( lib, "TG_DwgDb.lib" )
#pragma comment( lib, "ThreadPool.lib" )


// Linker pragmas
#pragma comment( lib, "Mpr.lib" )
#pragma comment( lib, "Psapi.lib" )
#pragma comment( lib, "Rpcrt4.lib" )
#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "vfw32.lib" )
#pragma comment( lib, "shlwapi.lib" )
#pragma comment( lib, "shell32.lib" )
#pragma comment( lib, "htmlhelp.lib" )
#pragma comment( lib, "gdiplus.lib" )
#pragma comment( lib, "wsock32.lib" )
#pragma comment( lib, "iphlpapi.lib" )
#pragma comment( lib, "Version.lib" )
#pragma comment( lib, "UxTheme.lib" )

#pragma pack (pop)
