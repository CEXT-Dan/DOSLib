/////////////////////////////////////////////////////////////////////////////
// func_autocad.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosLayerListDialog.h"
#include "DosStringArray.h"

#define X 0
#define Y 1
#define Z 2


using namespace Gdiplus;

static int GetEncoderClsid(const wchar_t* pFormat, CLSID* pClsid)
{
    if (nullptr == pFormat || nullptr == pClsid)
        return -1;

    unsigned int count = 0;  // number of image encoders
    unsigned int size = 0;   // size of the image encoder array in bytes
    GetImageEncodersSize(&count, &size);
    if (0 == size)
        return -1;

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)malloc(size);
    if (nullptr == pImageCodecInfo)
        return -1;

    GetImageEncoders(count, size, pImageCodecInfo);

    for (unsigned int i = 0; i < count; ++i)
    {
        if (wcscmp(pImageCodecInfo[i].MimeType, pFormat) == 0)
        {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return i;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

////////////////////////////////////////////////////////////////
// dos_extractpreview
int CDOSLibApp::ads_dos_extractpreview()
{
#ifndef _TXAPP
    CAdsArgs args;

    AcApDocument* pDoc = odDocManager->curDocument();
    if (!pDoc)
        return RSERR;

    CDosPathString strFile = pDoc->fileName();
    CDosPathString strBitmap = strFile;
    strBitmap.RenameExtension(L".bmp");

    if (!args.IsEmpty())
    {
        if (!args.GetString(strBitmap))
            return RSERR;

        strBitmap.FullPath();

        args.Next();

        if (!args.IsEmpty())
        {
            if (!args.GetString(strFile))
                return RSRSLT;

            strFile.FullPath();
            if (!strFile.FileExists())
                return RSRSLT;
        }
    }

    wchar_t drive[_MAX_DRIVE];
    wchar_t dir[_MAX_DIR];
    wchar_t fname[_MAX_FNAME];
    wchar_t ext[_MAX_EXT];
    _wsplitpath_s(strBitmap, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

    CString strExt(ext);

    int nFormat = 0;
    if (strExt.CompareNoCase(L".bmp") == 0)
        nFormat = 0;
    else if (strExt.CompareNoCase(L".jpg") == 0)
        nFormat = 1;
    else if (strExt.CompareNoCase(L".jpeg") == 0)
        nFormat = 1;
    else if (strExt.CompareNoCase(L".gif") == 0)
        nFormat = 2;
    else if (strExt.CompareNoCase(L".tiff") == 0)
        nFormat = 3;
    else if (strExt.CompareNoCase(L".tif") == 0)
        nFormat = 3;
    else if (strExt.CompareNoCase(L".png") == 0)
        nFormat = 4;
    else
    {
        acutPrintf(L"DOSLib error: unknown bitmap format.\n");
        acedRetNil();
        return RSRSLT;
    }

    CLSID encoder;
    int rc;

    if (nFormat == 0)
        rc = GetEncoderClsid(L"image/bmp", &encoder);
    else if (nFormat == 1)
        rc = GetEncoderClsid(L"image/jpeg", &encoder);
    else if (nFormat == 2)
        rc = GetEncoderClsid(L"image/gif", &encoder);
    else if (nFormat == 3)
        rc = GetEncoderClsid(L"image/tiff", &encoder);
    else if (nFormat == 4)
        rc = GetEncoderClsid(L"image/png", &encoder);
    else
    {
        acutPrintf(L"DOSLib error: invalid bitmap format.\n");
        acedRetNil();
        return RSRSLT;
    }

    if (rc == -1)
    {
        acedRetNil();
        return RSRSLT;
    }

    HBITMAP hBitmap = 0;
    HPALETTE hPal = 0;
    if (!AcDbGetPreviewBitmapFromDwg(strFile, &hBitmap, &hPal))
    {
        acedRetNil();
        return RSRSLT;
    }

    CStringW wstrBitmap = strBitmap;
    Bitmap image(hBitmap, hPal);
    image.Save(wstrBitmap, &encoder, NULL);

    DeleteObject(hBitmap);
    DeleteObject(hPal);

    acedRetStr(strBitmap);
#endif // !_TXAPP
    acutPrintf(_T("\nNot implemented yet"));
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_layerlistbox
int CDOSLibApp::ads_dos_layerlistbox()
{
    CAdsArgs args;
    CAcModuleResourceOverride myResources;

    CDosLayerListDialog dlg(CWnd::FromHandle(adsw_acadMainWnd()));

    CString strTitle;
    if (!args.GetString(strTitle))
        return RSERR;

    dlg.SetTitle(strTitle);

    args.Next();

    CString strPrompt;
    if (!args.GetString(strPrompt))
        return RSERR;

    dlg.SetPrompt(strPrompt);

    args.Next();

    int nFlags = 0;
    if (!args.IsEmpty())
    {
        if (!args.GetInteger(nFlags))
            return RSERR;
        nFlags = abs(nFlags);
    }

    if (nFlags & 1) dlg.HideOnLayers();
    if (nFlags & 2) dlg.HideOffLayers();
    if (nFlags & 4) dlg.HideThawedLayers();
    if (nFlags & 8) dlg.HideFrozenLayers();
    if (nFlags & 16) dlg.HideUnlockedLayers();
    if (nFlags & 32) dlg.HideLockedLayers();
    if (nFlags & 64) dlg.HideStdLayers();
    if (nFlags & 128) dlg.HideXrefLayers();
    if (nFlags & 256) dlg.SetMultipleSelection();
    if (nFlags & 512) dlg.HidePlotLayers();
    if (nFlags & 1024) dlg.HideNoPlotLayers();
    if (nFlags & 2048) dlg.HideUsedLayers();
    if (nFlags & 4096) dlg.HideUnusedLayers();

    if (dlg.DoModal() == IDCANCEL)
    {
        acedRetNil();
        return RSRSLT;
    }

    if (dlg.MultipleSelection())
    {
        CAdsRetList result;

        int i;
        for (i = 0; i < dlg.m_layers.GetCount(); i++)
            result.Add(dlg.m_layers[i]);

        if (result.IsEmpty())
            acedRetNil();
        else
            acedRetList(result);
    }
    else
    {
        acedRetStr(dlg.m_layer);
    }

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_closeall
int CDOSLibApp::ads_dos_closeall()
{
    odapDocManager()->sendStringToExecute(odapDocManager()->curDocument(), L"_.CLOSEALL ", false, true);
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_saveall
int CDOSLibApp::ads_dos_saveall()
{
    odapDocManager()->sendStringToExecute(odapDocManager()->curDocument(), L"_.SAVEALL ", false, true);
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_isbreak
int CDOSLibApp::ads_dos_isbreak()
{
    DOS_Pump();
    if (sds_usrbrk())
        acedRetT();
    else
        acedRetNil();
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_lisplist
int CDOSLibApp::ads_dos_lisplist()
{
    resbuf* pHead = sds_arxloaded();
    acedRetList(pHead);
    acutRelRb(pHead);
#ifndef _TXAPP
    CAdsArgs args;

    bool bFullPath = false;
    if (!args.IsEmpty())
        bFullPath = true;

    AcApDocument* pDoc = curDoc();
    if (!pDoc)
        return RSERR;

    CAdsRetList result;

    int i;
    for (i = 0; i < pDoc->GetCountOfLispList(); i++)
    {
        AcLispAppInfo* pInfo = pDoc->GetItemOfLispList(i);
        if (!pInfo)
            continue;

        CString strPath = pInfo->appFileName;

        if (!bFullPath)
        {
            wchar_t drive[_MAX_DRIVE];
            wchar_t dir[_MAX_DIR];
            wchar_t fname[_MAX_FNAME];
            wchar_t ext[_MAX_EXT];
            _wsplitpath_s(strPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
            strPath = fname;
            strPath += ext;
        }

        result.Add(strPath);
    }

    if (result.IsEmpty())
        acedRetNil();
    else
        acedRetList(result);
#endif
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_arxlist
int CDOSLibApp::ads_dos_arxlist()
{
    CAdsArgs args;

    bool bFullPath = false;
    if (!args.IsEmpty())
        bFullPath = true;

    CAdsRetList result;

    struct resbuf* rb1 = 0, * rb2 = 0;
    for (rb2 = rb1 = ads_arxloaded(); rb2 != 0; rb2 = rb2->rbnext)
    {
        if (rb2->restype == RTSTR)
        {
            CString str(rb2->resval.rstring);
            if (bFullPath)
            {
                HMODULE hModule = GetModuleHandle(str);
                if (hModule != NULL)
                {
                    TCHAR tchFileName[_MAX_PATH];
                    DWORD dwSize = _MAX_PATH;
                    DWORD dwResult = GetModuleFileName(hModule, tchFileName, dwSize);
                    if (dwResult)
                        result.Add(tchFileName);
                }
            }
            else
            {
                result.Add(str);
            }
        }
    }
    acutRelRb(rb1);

    if (result.IsEmpty())
        return acedRetNil();
    else
        return acedRetList(result);

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_progbar
int CDOSLibApp::ads_dos_progbar()
{
    CAdsArgs args;

    // Finish
    if (args.IsEmpty())
    {
        sds_progressstop();
        acedRetNil();
        return RSRSLT;
    }

    // Step it
    if (args.IsInteger())
    {
        int nPos = 0;
        if (!args.GetInteger(nPos))
            return RSERR;
        if (nPos < 0)
            nPos = 10;
        if (sds_progresspercent(nPos) == 0)
            acedRetT();
        else
            acedRetNil();
        return RSRSLT;
    }

    // Start
    CString strLabel;
    if (!args.GetString(strLabel))
        return RSERR;

    args.Next();

    int nMaxPos = 0;
    if (!args.GetInteger(nMaxPos))
        return RSERR;

    if (sds_progressstart(strLabel) == 0)
        acedRetT();
    else
        acedRetNil();
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_show
int CDOSLibApp::ads_dos_show()
{
    CAdsArgs args;

    int nCmdShow;
    if (!args.GetInteger(nCmdShow))
        return RSERR;

    switch (nCmdShow)
    {
        case 0:
            nCmdShow = SW_SHOWMAXIMIZED;
            break;
        case 1:
            nCmdShow = SW_SHOWMINIMIZED;
            break;
        case 2:
            nCmdShow = SW_RESTORE;
            break;
        default:
            acutPrintf(L"DOSLib error: invalid argument.\n");
            acedRetNil();
            return RSRSLT;
    }

    ::ShowWindow(adsw_acadMainWnd(), nCmdShow);
    acedRetNil();

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cmdline
int CDOSLibApp::ads_dos_cmdline()
{
    CAdsArgs args;

    CWnd* pDock = CWnd::FromHandle(sds_gettextcmdlinehwnd());
    if (!pDock)
    {
        acedRetNil();
        return RSRSLT;
    }

    CWnd* pChild = pDock->GetWindow(GW_CHILD);
    if (!pChild)
    {
        acedRetNil();
        return RSRSLT;
    }

    CString str;
    if (args.IsEmpty())
    {
        pChild->GetWindowText(str);
        acedRetStr(str);
    }
    else
    {
        pChild = pChild->GetNextWindow();
        if (pChild)
        {
            pChild->GetWindowText(str);
            acedRetStr(str);
        }
        else
        {
            acedRetNil();
        }
    }

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_istextscr
int CDOSLibApp::ads_dos_istextscr()
{
    CAdsArgs args;

    CWnd* pDock = CWnd::FromHandle(sds_gettextcmdlinehwnd());
    if (!pDock)
    {
        acedRetNil();
        return RSRSLT;
    }

    CWnd* pChild = pDock->GetWindow(GW_CHILD);
    if (!pChild)
    {
        acedRetNil();
        return RSRSLT;
    }

    pChild = pChild->GetNextWindow();
    if (pChild && pChild->IsWindowVisible())
    {
        // AutoCAD never minimized the text window.  Instead, they move it way off of the 
        // screen.  Thus, we we cannot intersect the screen rect with the window rect, 
        // then the text window must be "pseudo-minimized."

        CRect rect, window_rect, screen_rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
        pChild->GetWindowRect(window_rect);
        if (rect.IntersectRect(screen_rect, window_rect))
            acedRetT();
        else
            acedRetNil();
    }
    else
    {
        acedRetNil();
    }

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_curvearea
int CDOSLibApp::ads_dos_curvearea()
{
    CAdsArgs args;

    OdDbObjectId entId;
    if (!args.GetEntity(entId))
        return RSERR;

    OdDbEntity* pEnt = NULL;

    if (!entId.objectClass()->isDerivedFrom(OdDbCurve::desc()))
    {
        acutPrintf(L"DOSLib error: entity not a curve.\n");
        return RSERR;
    }

    OdDbCurvePtr pCurve = entId.openObject();
    double area = 0.0;
    if (false == pCurve->isClosed() || eOk != pCurve->getArea(area))
    {
        acutPrintf(L"DOSLib error: entity not a closed curve.\n");
        return RSERR;
    }
    acedRetReal(area);

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_massprops
int CDOSLibApp::ads_dos_massprops()
{
    CAdsArgs args;

    OdDbObjectId entId;
    if (!args.GetEntity(entId))
        return RSERR;

    OdDb3dSolidPtr pSolid = entId.openObject();
    if (pSolid.isNull())
    {
        acutPrintf(L"DOSLib error: entity not a solid.\n");
        return RSERR;
    }

    double volume;
    OdGePoint3d centroid;
    double momInertia[3];
    double prodInertia[3];
    double prinMoments[3];
    OdGeVector3d prinAxes[3];
    double radiiGyration[3];
    OdGeExtents3d extents;

    if (pSolid->getMassProp(volume, centroid, momInertia, prodInertia, prinMoments, prinAxes, radiiGyration, extents) != OdResult::eOk)
    {
        struct resbuf* prbResult = acutBuildList(
            RTREAL, volume,
            RTLB,
            RTREAL, centroid[X],
            RTREAL, centroid[Y],
            RTREAL, centroid[Z],
            RTLE,
            RTLB,
            RTREAL, momInertia[X],
            RTREAL, momInertia[Y],
            RTREAL, momInertia[Z],
            RTLE,
            RTLB,
            RTREAL, prodInertia[X],
            RTREAL, prodInertia[Y],
            RTREAL, prodInertia[Z],
            RTLE,
            RTLB,
            RTREAL, prinMoments[X],
            RTREAL, prinMoments[Y],
            RTREAL, prinMoments[Z],
            RTLE,
            RTLB,
            RTLB,
            RTREAL, prinAxes[X][X],
            RTREAL, prinAxes[X][Y],
            RTREAL, prinAxes[X][Z],
            RTLE,
            RTLB,
            RTREAL, prinAxes[Y][X],
            RTREAL, prinAxes[Y][Y],
            RTREAL, prinAxes[Y][Z],
            RTLE,
            RTLB,
            RTREAL, prinAxes[Z][X],
            RTREAL, prinAxes[Z][Y],
            RTREAL, prinAxes[Z][Z],
            RTLE,
            RTLE,
            RTLB,
            RTREAL, radiiGyration[X],
            RTREAL, radiiGyration[Y],
            RTREAL, radiiGyration[Z],
            RTLE,
            RTLB,
            RTLB,
            RTREAL, extents.minPoint()[X],
            RTREAL, extents.minPoint()[Y],
            RTREAL, extents.minPoint()[Z],
            RTLE,
            RTLB,
            RTREAL, extents.maxPoint()[X],
            RTREAL, extents.maxPoint()[Y],
            RTREAL, extents.maxPoint()[Z],
            RTLE,
            RTLE,
            0);

        acedRetList(prbResult);
        acutRelRb(prbResult);
    }
    else
    {
        acedRetNil();
    }
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_isvlide
int CDOSLibApp::ads_dos_isvlide()
{
    CWnd* pWnd = CWnd::FindWindow(L"VisualLisp", 0);
    if (pWnd && pWnd->IsWindowVisible())
        acedRetT();
    else
        acedRetNil();
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_acadmem
int CDOSLibApp::ads_dos_acadmem()
{
    DWORD dwProcessID = GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
    if (hProcess)
    {
        PROCESS_MEMORY_COUNTERS pmc;
        ZeroMemory(&pmc, sizeof(PROCESS_MEMORY_COUNTERS));
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
        {
            CAdsRetList result;
            result.Add((int)pmc.WorkingSetSize);
            result.Add((int)pmc.PeakWorkingSetSize);
            result.Add((int)pmc.PageFaultCount);
            result.Add((int)pmc.PagefileUsage);
            result.Add((int)pmc.PeakPagefileUsage);
            result.Add((int)pmc.QuotaPagedPoolUsage);
            result.Add((int)pmc.QuotaPeakPagedPoolUsage);
            result.Add((int)pmc.QuotaNonPagedPoolUsage);
            result.Add((int)pmc.QuotaPeakNonPagedPoolUsage);
            acedRetList(result);
        }
        else
            acedRetNil();
        CloseHandle(hProcess);
    }
    else
        acedRetNil();

    return RSRSLT;
}

static void ReadXrefs(OdDbDatabase* db)
{
    if (!db)
        return;
    try
    {
        OdDbXRefMan::loadAll(db);
        CAdsRetList result;
        OdDbBlockTablePtr pBlockTable = db->getBlockTableId().safeOpenObject();
        OdDbBlockTableIteratorPtr pBlockTableIterator = pBlockTable->newIterator();
        for (pBlockTableIterator->start(); !pBlockTableIterator->done(); pBlockTableIterator->step())
        {
            OdDbBlockTableRecordPtr pBlock = pBlockTableIterator->getRecord();
            if (pBlock->isFromExternalReference())
            {
                result.Add(pBlock->pathName());
            }
        }
        if (result.IsEmpty())
            acedRetNil();
        else
            acedRetList(result);
    }
    catch (...)
    {
        acedRetNil();
    }
}

////////////////////////////////////////////////////////////////
// dos_xreflist
int CDOSLibApp::ads_dos_xreflist()
{
    CAdsArgs args;

    if (args.IsEmpty())
    {
        if (odapDocManager()->curDocument() == nullptr)
            return RSRSLT;
        ReadXrefs(odapDocManager()->curDocument()->database());
        return RSRSLT;
    }
    CDosPathString strPath;
    if (!args.GetPathString(strPath))
        return RSERR;

    strPath.FullPath();

    if (!curDb())
        return RSERR;
    try
    {
        OdDbDatabasePtr ptr = curDb()->appServices()->createDatabase(false);
        ptr->readFile((const TCHAR*)(const TCHAR*)strPath);
        ReadXrefs(ptr.get());
    }
    catch (OdError& e)
    {
        acutPrintf(_T("\n%ls"), (const TCHAR*)e.description());
    }
    return RSRSLT;
}


static int GetNearestACI(int red, int green, int blue)
{
    int minndx = 0;
    int mindst = 2147483647L;
    int i;
    for (i = 1; i < 255; i++)
    {
        int acirgb = sds_getrgbvalue(i);
        int r = (acirgb & 0xffL);
        int g = (acirgb & 0xff00L) >> 8;
        int b = acirgb >> 16;
        int dst = abs(r - red) + abs(g - green) + abs(b - blue);
        if (dst < mindst)
        {
            minndx = i;
            mindst = dst;
        }
    }
    return minndx;
}

static int GetNearestACI(int rgb)
{
    int r = GetRValue(rgb);
    int g = GetGValue(rgb);
    int b = GetBValue(rgb);
    return GetNearestACI(r, g, b);
}

////////////////////////////////////////////////////////////////
// dos_rgbtogray
int CDOSLibApp::ads_dos_rgbtogray()
{
    CAdsArgs args;

    int r = 0, g = 0, b = 0;

    if (!args.GetInteger(r))
        return RSERR;

    args.Next();
    if (args.IsEmpty())
    {
        if (r < 0 || r > RGB(255, 255, 255))
        {
            acutPrintf(L"DOSLib error: invalid rgb value.\n");
            return RSERR;
        }

        int aci = GetNearestACI(r);
        acedRetInt(aci);
        return RSRSLT;
    }

    if (!args.GetInteger(g))
        return RSERR;

    args.Next();

    if (!args.GetInteger(b))
        return RSERR;

    if (r < 0 || r > 255)
    {
        acutPrintf(L"DOSLib error: invalid red value.\n");
        return RSERR;
    }

    if (g < 0 || g > 255)
    {
        acutPrintf(L"DOSLib error: invalid green value.\n");
        return RSERR;
    }

    if (b < 0 || b > 255)
    {
        acutPrintf(L"DOSLib error: invalid blue value.\n");
        return RSERR;
    }

    COLORREF cr = (COLORREF)((r * 0.30) + (g * 0.59) + (b * 0.11));

    CAdsRetList result;
    result.Add((short)cr);
    result.Add((short)cr);
    result.Add((short)cr);
    acedRetList(result);

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_rgbtoaci
int CDOSLibApp::ads_dos_rgbtoaci()
{
    CAdsArgs args;

    int r = 0, g = 0, b = 0;

    if (!args.GetInteger(r))
        return RSERR;

    args.Next();
    if (args.IsEmpty())
    {
        if (r < 0 || r > RGB(255, 255, 255))
        {
            acutPrintf(L"DOSLib error: invalid rgb value.\n");
            return RSERR;
        }

        int aci = GetNearestACI(r);
        acedRetInt(aci);
        return RSRSLT;
    }

    if (!args.GetInteger(g))
        return RSERR;

    args.Next();

    if (!args.GetInteger(b))
        return RSERR;

    if (r < 0 || r > 255)
    {
        acutPrintf(L"DOSLib error: invalid red value.\n");
        return RSERR;
    }

    if (g < 0 || g > 255)
    {
        acutPrintf(L"DOSLib error: invalid green value.\n");
        return RSERR;
    }

    if (b < 0 || b > 255)
    {
        acutPrintf(L"DOSLib error: invalid blue value.\n");
        return RSERR;
    }

    int aci = GetNearestACI(r, g, b);
    acedRetInt(aci);

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_acitorgb
int CDOSLibApp::ads_dos_acitorgb()
{
    CAdsArgs args;

    int nColor;
    if (!args.GetInteger(nColor))
        return RSERR;

    if (nColor < 1 && nColor > 255)
    {
        acutPrintf(L"DOSLib error: invalid aci value.\n");
        return RSERR;
    }

    COLORREF cr = sds_getrgbvalue(nColor);
    CAdsRetList result;
    result.Add((short)GetRValue(cr));
    result.Add((short)GetGValue(cr));
    result.Add((short)GetBValue(cr));
    acedRetList(result);

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_rgbtohls
int CDOSLibApp::ads_dos_rgbtohls()
{
    CAdsArgs args;

    int r = 0, g = 0, b = 0;

    if (!args.GetInteger(r))
        return RSERR;

    args.Next();
    if (args.IsEmpty())
    {
        if (r < 0 || r > RGB(255, 255, 255))
        {
            acutPrintf(L"DOSLib error: invalid rgb value.\n");
            return RSERR;
        }

        WORD h = 0, l = 0, s = 0;
        ColorRGBToHLS(r, &h, &l, &s);

        CAdsRetList result;
        result.Add((short)h);
        result.Add((short)l);
        result.Add((short)s);
        acedRetList(result);
        return RSRSLT;
    }

    if (!args.GetInteger(g))
        return RSERR;

    args.Next();

    if (!args.GetInteger(b))
        return RSERR;

    if (r < 0 || r > 255)
    {
        acutPrintf(L"DOSLib error: invalid red value.\n");
        return RSERR;
    }

    if (g < 0 || g > 255)
    {
        acutPrintf(L"DOSLib error: invalid green value.\n");
        return RSERR;
    }

    if (b < 0 || b > 255)
    {
        acutPrintf(L"DOSLib error: invalid blue value.\n");
        return RSERR;
    }

    WORD h = 0, l = 0, s = 0;
    ColorRGBToHLS(RGB(r, g, b), &h, &l, &s);

    CAdsRetList result;
    result.Add((short)h);
    result.Add((short)l);
    result.Add((short)s);
    acedRetList(result);

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_hlstorgb
int CDOSLibApp::ads_dos_hlstorgb()
{
    CAdsArgs args;

    int h = 0, l = 0, s = 0;

    if (!args.GetInteger(h))
        return RSERR;

    args.Next();

    if (!args.GetInteger(l))
        return RSERR;

    args.Next();

    if (!args.GetInteger(s))
        return RSERR;

    if (h < 0 || h > 360)
    {
        acutPrintf(L"DOSLib error: invalid hue value.\n");
        return RSERR;
    }

    if (l < 0)
    {
        acutPrintf(L"DOSLib error: invalid luminance value.\n");
        return RSERR;
    }

    if (s < 0)
    {
        acutPrintf(L"DOSLib error: invalid saturation value.\n");
        return RSERR;
    }

    COLORREF cr = ColorHLSToRGB((WORD)h, (WORD)l, (WORD)s);

    CAdsRetList result;
    result.Add((short)GetRValue(cr));
    result.Add((short)GetGValue(cr));
    result.Add((short)GetBValue(cr));
    acedRetList(result);

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_dwgver
int CDOSLibApp::ads_dos_dwgver()
{
    CAdsArgs args;

    CDosPathString strPath;
    if (!args.GetPathString(strPath))
        return RSERR;

    strPath.FullPath();

    OdDbDatabasePtr pDb = OdDbDatabase::createObject();
    try
    {
        pDb->readFile((const TCHAR*)strPath);
    }
    catch (...)
    {
        acutPrintf(L"DOSLib error: cannot read drawing file.\n");
        acedRetNil();
        return RSRSLT;
    }

    // Get the DWG version number.
    auto dwgVer = pDb->originalFileVersion();
    CString str;

    switch (dwgVer)
    {
        case OdDb::kDHL_MC0_0:
            str = L"AutoCAD 1.0";
            break;
        case OdDb::kDHL_AC1_2:
            str = L"AutoCAD 1.2";
            break;
        case OdDb::kDHL_AC1_40:
            str = L"AutoCAD 1.40";
            break;
        case OdDb::kDHL_AC1_50:
            str = L"AutoCAD 1.50";
            break;
        case OdDb::kDHL_AC2_20:
            str = L"AutoCAD 2.20";
            break;
        case OdDb::kDHL_AC2_10:
            str = L"AutoCAD 2.10";
            break;
        case OdDb::kDHL_AC2_21:
            str = L"AutoCAD 2.21";
            break;
        case OdDb::kDHL_AC2_22:
        case OdDb::kDHL_1001:
            str = L"AutoCAD 2.22";
            break;
        case OdDb::kDHL_1002:
            str = L"AutoCAD 2.5";
            break;
        case OdDb::kDHL_1003:
            str = L"AutoCAD 2.6";
            break;
        case OdDb::kDHL_1004:
        case OdDb::kDHL_1005:
            str = L"Release 9";
            break;
        case OdDb::kDHL_1006:
        case OdDb::kDHL_1007:
        case OdDb::kDHL_1008:
            str = L"Release 10";
            break;
        case OdDb::kDHL_1009:
        case OdDb::kDHL_1010:
        case OdDb::kDHL_1011:
            str = L"Release 11/12";
            break;
        case OdDb::kDHL_1012:
            str = L"Release 13";
            break;
        case OdDb::kDHL_1013:
        case OdDb::kDHL_1014:
            str = L"Release 14";
            break;
        case OdDb::kDHL_1500:
        case OdDb::kDHL_1015:
            str = L"AutoCAD 2000";
            break;
        case OdDb::kDHL_1800a:
        case OdDb::kDHL_1800:
            str = L"AutoCAD 2004";
            break;
        case OdDb::kDHL_2100a:
        case OdDb::kDHL_1021:
            str = L"AutoCAD 2007";
            break;
        case OdDb::kDHL_2400a:
        case OdDb::kDHL_1024:
            str = L"AutoCAD 2010";
            break;
        case OdDb::kDHL_2700a:
        case OdDb::kDHL_1027:
            str = L"AutoCAD 2013";
            break;
#if _ARX_VER > 21 || _BRX_VER > 17
        case OdDb::kDHL_3200a:
        case OdDb::kDHL_1032:
            str = L"AutoCAD 2018";
            break;
#endif
        default:
            str = L"Unknown";
    }

    acedRetStr(str);

    return RSRSLT;
}

static bool ListSorterEx(struct resbuf* rb, CAdsRetList& result)
{
    if (0 == rb)
        return false;

    CDosNumberArray numbers;
    CDosStringArray strings;

    while (rb)
    {
        switch (rb->restype)
        {
            case RTSTR:
                strings.Add(CString(rb->resval.rstring));
                break;

            case RTSHORT:
                numbers.Add((double)rb->resval.rint);
                break;

            case RTLONG:
                numbers.Add((double)rb->resval.rlong);
                break;

            case RTREAL:
                numbers.Add(rb->resval.rreal);
                break;

            case RTPOINT:
                numbers.Add(rb->resval.rpoint[0]);
                numbers.Add(rb->resval.rpoint[1]);
                break;

            case RT3DPOINT:
                numbers.Add(rb->resval.rpoint[0]);
                numbers.Add(rb->resval.rpoint[1]);
                numbers.Add(rb->resval.rpoint[2]);
                break;
        }

        rb = rb->rbnext;
    }

    if (0 == numbers.GetCount() && 0 == strings.GetCount())
        return false;

    if (numbers.GetCount() > 1)
        numbers.Sort();

    if (strings.GetCount() > 1)
        strings.Sort();

    int i;

    for (i = 0; i < numbers.GetCount(); i++)
        result.Add(numbers[i]);

    for (i = 0; i < strings.GetCount(); i++)
        result.Add(strings[i]);

    return true;
}

// Obsolete
static struct resbuf* ListSorter(struct resbuf* ab)
{
    int fi1 = 0, fi2 = 0, totcnt = 0;
    struct resbuf* link, * j, * p, * t, * rb, * rbb;

    // Make a copy of the link list (ab)
    for (t = rb = 0, rbb = ab; rbb; rbb = rbb->rbnext)
    {
        if (0 == t)
            t = rb = acutNewRb(rbb->restype);
        else
        {
            rb->rbnext = acutNewRb(rbb->restype);
            rb = rb->rbnext;
        }

        switch (rb->restype)
        {
            case RTSTR:
                if (rbb->resval.rstring == 0)
                    break;
                rb->resval.rstring = new wchar_t[wcslen(rbb->resval.rstring) + 1];
                wcscpy(rb->resval.rstring, rbb->resval.rstring);
                ++fi1;
                break;

            case RTSHORT:
                rb->resval.rint = rbb->resval.rint;
                ++fi1;
                break;

            case RTREAL:
                rb->resval.rreal = rbb->resval.rreal;
                ++fi1;
                break;
        }
    }

    link = t;
    if (link == 0)
        return 0;

    for (rb = link; rb; rbb = rb, rb = rb->rbnext)
    {
        if (rb->restype == RTLB)
        {
            rbb = link;
            link = rb = link->rbnext;
            rbb->rbnext = 0;
            acutRelRb(rbb);
        }
        else if (rb->restype == RTLE)
        {
            rbb->rbnext = 0;
            acutRelRb(rb);
            rb = 0;
            break;
        }
    }

    for (fi1 = -1, totcnt = 0; fi1 < totcnt; ++fi1)
    {
        int type = link->restype;
        for (fi2 = 0, p = j = link; j->rbnext; ++fi2)
        {
            if (j->restype != type)
                return 0;

            switch (j->restype)
            {
                case RTSHORT:
                    if (j->rbnext->restype != RTSHORT)
                    {
                        j = j->rbnext;
                        break;
                    }
                    if (j->resval.rint > j->rbnext->resval.rint)
                    {
                        t = j->rbnext;
                        p->rbnext = j->rbnext;
                        j->rbnext = j->rbnext->rbnext;
                        t->rbnext = j;
                        if (j == link)
                            p = link = t;
                        else p = t;
                    }
                    else
                    {
                        p = j;
                        j = j->rbnext;
                    }
                    break;

                case RTREAL:
                    if (j->rbnext->restype != RTREAL)
                    {
                        j = j->rbnext;
                        break;
                    }
                    if (j->resval.rreal > j->rbnext->resval.rreal)
                    {
                        t = j->rbnext;
                        p->rbnext = j->rbnext;
                        j->rbnext = j->rbnext->rbnext;
                        t->rbnext = j;
                        if (j == link)
                            p = link = t;
                        else
                            p = t;
                    }
                    else
                    {
                        p = j;
                        j = j->rbnext;
                    }
                    break;

                case RTSTR:
                    if (j->rbnext->restype != RTSTR)
                    {
                        j = j->rbnext;
                        break;
                    }
                    if (wcscmp(j->resval.rstring, j->rbnext->resval.rstring) >= 0)
                    {
                        t = j->rbnext;
                        p->rbnext = j->rbnext;
                        j->rbnext = j->rbnext->rbnext;
                        t->rbnext = j;
                        if (j == link)
                            p = link = t;
                        else
                            p = t;
                    }
                    else
                    {
                        p = j;
                        j = j->rbnext;
                    }
                    break;

                default:
                    j = j->rbnext;
                    break;

            }

            if ((totcnt && fi2 >= totcnt - fi1) || j == 0)
                break;
        }

        if (!totcnt)
            totcnt = fi2;
    }

    return link;
}

////////////////////////////////////////////////////////////////
// dos_sortlist
int CDOSLibApp::ads_dos_sortlist()
{
    CAdsArgs args;

    if (args.IsEmpty())
        return RSERR;

    CAdsRetList result;
    bool rc = ListSorterEx(args.Resbuf(), result);
    if (rc)
        acedRetList(result);
    else
        acedRetNil();

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_showobjects
int CDOSLibApp::ads_dos_showobjects()
{
    OdDbObjectIdArray arr;
    if (odapDocManager()->curDocument() == nullptr)
        return RSERR;
    CAdsArgs args;
    if (args.IsEmpty())
    {
        OdDbDatabase* pDb = odapDocManager()->curDocument()->database();
        arr.setPhysicalLength(pDb->approxNumObjects());

        OdDbBlockTableRecordPtr pModel = pDb->getModelSpaceId().openObject();
        for (auto iter = pModel->newIterator(); !iter->done(); iter->step())
        {
            arr.append(iter->objectId());
        }
    }
    else if (args.IsSelectionSet())
    {
        if (!args.GetEntityList(arr))
            return RSERR;
    }
    else if (args.IsEntity())
    {
        OdDbObjectId e;
        if (!args.GetEntity(e))
            return RSERR;
        arr.append(e);
    }
    else
        return RSERR;

    int count = 0;
    for (auto& id : arr)
    {
        OdDbEntityPtr pEnt = id.openObject(OdDb::kForWrite);
        if (!pEnt.isNull())
        {
            if (OdDb::kInvisible == pEnt->visibility())
            {
                pEnt->setVisibility(OdDb::kVisible);
                count++;
            }
        }
    }
    acedRetInt(count);
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_hideobjects
int CDOSLibApp::ads_dos_hideobjects()
{
    CAdsArgs args;

    OdDbObjectIdArray arr;
    if (args.IsSelectionSet())
    {
        if (!args.GetEntityList(arr))
            return RSERR;
    }
    else if (args.IsEntity())
    {
        OdDbObjectId e;
        if (!args.GetEntity(e))
            return RSERR;
        arr.append(e);
    }
    else
        return RSERR;

    int count = 0;
    for (auto& id : arr)
    {
        OdDbEntityPtr pEnt = id.openObject(OdDb::kForWrite);
        if (!pEnt.isNull())
        {
            if (OdDb::kVisible == pEnt->visibility())
            {
                pEnt->setVisibility(OdDb::kVisible);
                count++;
            }
        }
    }
    acedRetInt(count);
    return RSRSLT;
}

enum IncidenceType
{
    kIncidenceToLeft = 0,
    kIncidenceToRight = 1,
    kIncidenceToFront = 2,
    kIncidenceUnknown
};

IncidenceType CurveIncidence(OdDbCurve* pCurve, double param, OdGeVector3d dir, OdGeVector3d normal)
{
    if (!pCurve)
        return kIncidenceUnknown;

    OdGeVector3d deriv1;
    pCurve->getFirstDeriv(param, deriv1);
    if (deriv1.isParallelTo(dir))
    {
        // need second degree analysis
        OdGeVector3d deriv2;
        pCurve->getSecondDeriv(param, deriv2);
        if (deriv2.isZeroLength() || deriv2.isParallelTo(dir))
            return kIncidenceToFront;
        else if (deriv2.crossProduct(dir).dotProduct(normal) < 0)
            return kIncidenceToRight;
        else
            return kIncidenceToLeft;
    }

    if (deriv1.crossProduct(dir).dotProduct(normal) < 0)
        return kIncidenceToLeft;
    else
        return kIncidenceToRight;
}


bool IsInsideCurve(OdDbCurve* pCurve, OdGePoint3d testPt)
{
    if (!pCurve || !pCurve->isClosed())
        return false;


    OdDb2dPolyline* p2dPoly = OdDb2dPolyline::cast(pCurve);
    if (p2dPoly != NULL && p2dPoly->polyType() != OdDb::k2dSimplePoly)
        return false; // Not supported

    OdGePoint3d ptOnCurve;
    auto es = pCurve->getClosestPointTo(testPt, ptOnCurve);
    if (es == eOk && testPt == ptOnCurve)
        return true;

    // check its planar
    OdGePlane plane;
    OdDb::Planarity planarity;
    es = pCurve->getPlane(plane, planarity);
    if (es != eOk || planarity != OdDb::kPlanar)
        return false;

    // make the test ray from the plane
    OdGeVector3d normal = plane.normal();
    OdGeVector3d testVector = normal.perpVector();
    OdDbRay ray;
    ray.setBasePoint(testPt);
    ray.setUnitDir(testVector);

    OdGePoint3dArray IntersectionPoints;
    es = pCurve->intersectWith(&ray, OdDb::kOnBothOperands, IntersectionPoints);
    if (es != eOk)
        return false;

    int numberOfInters = IntersectionPoints.length();
    if (numberOfInters == 0)
        return false; // must be outside

    int i, nGlancingHits = 0;
    double epsilon = 2e-6; // trust me on this...
    for (i = 0; i < numberOfInters; i++)
    {
        // get the first point, and get its parameter
        OdGePoint3d hitPt = IntersectionPoints[i];
        double hitParam = 0.0;
        es = pCurve->getParamAtPoint(hitPt, hitParam);
        if (es != eOk)
            return false;

        double inParam = hitParam - epsilon;
        double outParam = hitParam + epsilon;

        IncidenceType inIncidence = CurveIncidence(pCurve, inParam, testVector, normal);
        IncidenceType outIncidence = CurveIncidence(pCurve, outParam, testVector, normal);
        if ((inIncidence == kIncidenceToRight && outIncidence == kIncidenceToLeft) ||
            (inIncidence == kIncidenceToLeft && outIncidence == kIncidenceToRight))
        {
            nGlancingHits++;
        }
    }

    return ((numberOfInters + nGlancingHits) % 2 == 1);
}


////////////////////////////////////////////////////////////////
// dos_isinsidecurve
int CDOSLibApp::ads_dos_isinsidecurve()
{

    CAdsArgs args;

    OdDbObjectId entId;
    if (!args.GetEntity(entId))
        return RSERR;

    args.Next();

    ads_point pt;
    if (!args.GetPoint(pt))
        return RSERR;

    if (!entId.objectClass()->isDerivedFrom(OdDbCurve::desc()))
    {
        acutPrintf(L"DOSLib error: entity not a curve.\n");
        return RSERR;
    }

    OdDbCurvePtr pCurve = entId.openObject();
    if (pCurve.isNull())
    {
        acutPrintf(L"DOSLib error: entity not opened.\n");
        return RSERR;
    }

    OdGePoint3d testPt(pt[0], pt[1], pt[2]);
    bool rc = IsInsideCurve(pCurve, testPt);
    if (rc)
        acedRetT();
    else
        acedRetNil();

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_plinecentroid
int CDOSLibApp::ads_dos_plinecentroid()
{

    CAdsArgs args;

    OdDbObjectId entId;
    if (!args.GetEntity(entId))
        return RSERR;

    if (!entId.objectClass()->isDerivedFrom(OdDb2dPolyline::desc()) && !entId.objectClass()->isDerivedFrom(OdDb2dPolyline::desc()))
    {
        acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
        return RSERR;
    }

    OdDbEntityPtr pEnt = entId.openObject();
    if (pEnt.isNull())
    {
        acutPrintf(L"DOSLib error: failed to open object.\n");
        return RSERR;
    }

    if (pEnt->isKindOf(OdDb2dPolyline::desc()) && !((OdDb2dPolyline*)pEnt.get())->isClosed())
    {
        acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
        return RSERR;
    }

    if (pEnt->isKindOf(OdDbPolyline::desc()) && !((OdDbPolyline*)pEnt.get())->isClosed())
    {
        acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
        return RSERR;
    }

    OdRxObjectPtrArray curves;
    curves.append(pEnt);

    OdRxObjectPtrArray regions;
    if (eOk != OdDbRegion::createFromCurves(curves, regions))
    {
        acedRetNil();
        return RSRSLT;
    }

    OdDbRegion* pReg = OdDbRegion::cast((OdRxObject*)regions[0]);
    if (0 == pReg)
    {
        acedRetNil();
        return RSRSLT;
    }

    OdGePlane plane;
    OdDb::Planarity planarity;
    pReg->getPlane(plane, planarity);

    OdGePoint3d origin;
    OdGeVector3d xAxis;
    OdGeVector3d yAxis;
    plane.getCoordSystem(origin, xAxis, yAxis);

    double perimeter, area;
    OdGePoint2d centroid;
    double momInertia[2];
    double prodInertia = 0.0;
    double prinMoments[2];
    OdGeVector2d prinAxes[2];
    double radiiGyration[2];
    OdGePoint2d extentsLow, extentsHigh;

    pReg->getAreaProp(
        origin,
        xAxis,
        yAxis,
        perimeter,
        area,
        centroid,
        momInertia,
        prodInertia,
        prinMoments,
        prinAxes,
        radiiGyration,
        extentsLow,
        extentsHigh
    );

    OdGeVector3d normal;
    pReg->getNormal(normal);

    OdGeMatrix3d mat;
    mat.setCoordSystem(origin, xAxis, yAxis, normal);

    OdGePoint3d cpt(centroid[0], centroid[1], 0.0);
    cpt.transformBy(mat);

    ads_point pt;
    pt[0] = cpt.x;
    pt[1] = cpt.y;
    pt[2] = cpt.z;

    acedRetPoint(pt);
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_hatcharea
int CDOSLibApp::ads_dos_hatcharea()
{

    CAdsArgs args;

    OdDbObjectId entId;
    if (!args.GetEntity(entId))
        return RSERR;

    if (!entId.objectClass()->isDerivedFrom(OdDbHatch::desc()))
    {
        acutPrintf(L"DOSLib error: entity not a hatch.\n");
        return RSERR;
    }

    OdDbHatchPtr pHatch = entId.openObject();
    if (pHatch.isNull())
    {
        acutPrintf(L"DOSLib error: cannot open this entity for reading.\n");
        return RSERR;
    }

    try
    {
        double total_area = 0.0;
        int i, loop_count = pHatch->numLoops();
        for (i = 0; i < loop_count; i++)
        {
            double area = 0.0;
            long loopType = 0;

            if (pHatch->loopTypeAt(i) & OdDbHatch::kPolyline)
            {
                OdGePoint2dArray vertices;
                OdGeDoubleArray bulges;
                pHatch->getLoopAt(i, vertices, bulges);

                unsigned int nVertices = vertices.length();

                OdDbPolyline testPoly;
                for (unsigned int vx = 0; vx < nVertices; vx++)
                {
                    double bulge = (bulges.length() < nVertices ? 0.0 : bulges[vx]);
                    testPoly.addVertexAt(vx, vertices[vx], bulge);
                }
                testPoly.getArea(area);
            }
            else
            {
                OdGePoint2dArray vertices;
                OdGeDoubleArray bulges;
                pHatch->getLoopAt(i, vertices, bulges);

                EdgeArray edgePtrs;
                pHatch->getLoopAt(i, edgePtrs);

                OdGeCurve2dPtrArray curves;
                for (auto& edge : edgePtrs)
                {
                    curves.append(OdGeCurve2dPtr(edge));
                }
                OdGeCompositeCurve2d compCurve(curves);
                OdGeInterval interval;
                compCurve.getInterval(interval);
                compCurve.area(interval.lowerBound(), interval.upperBound(), area);
                area = fabs(area);
            }

            if (loop_count > 1 && !(loopType & OdDbHatch::kExternal))
                area = -area;

            total_area += area;
        }
        acedRetReal(total_area);
    }
    catch (OdError& e)
    {
        acutPrintf(L"DOSLib error: %ls\n", (const TCHAR*)e.description());
        return RSERR;
    }
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cmdargs
int CDOSLibApp::ads_dos_cmdargs()
{
    CAdsRetList result;

    int count = 0;
    LPWSTR* pszArgs = CommandLineToArgvW(GetCommandLineW(), &count);
    if (pszArgs)
    {
        int i;
        for (i = 0; i < count; i++)
            result.Add(pszArgs[i]);

        LocalFree(pszArgs);
    }

    if (result.IsEmpty())
        acedRetNil();
    else
        acedRetList(result);

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_getsecret
TCHAR* g_result = 0; // global result
int g_length = 0;    // global result length
int g_cronly = 0;    // global cronly

#if defined(ARX23) || defined(ARX24) || defined(ZRX21)
static bool MessageFilter(MSG* pMsg)
#else
static BOOL MessageFilter(MSG* pMsg)
#endif
{
    if (pMsg && pMsg->message == WM_CHAR)
    {
        int keycode = (int)pMsg->wParam;
        if (keycode != 8 && keycode != 13 && keycode != 27 && ((keycode != 32) || g_cronly))
        {
            // keycode 8 is Backspace   
            // keycode 27 is Escape   
            // keycode 32 is Space (only avoided if cronly is used)   
            pMsg->wParam = 42;     // For Security against subsequent hooks, and display,   
            pMsg->lParam = 589825; // Set these two to reflect '*' keystroke   
            g_result[g_length] = keycode; // keep the real keycode.   
            g_length++;
        }
        if (keycode == 8 && g_length)
            g_length--;
    }
    return FALSE;
}

static int adskGetPassword(int cronly, const TCHAR* prompt, TCHAR* result)
{
    g_result = result;
    g_length = 0;
    g_cronly = cronly;

    //acedRegisterFilterWinMsg(MessageFilter);
    TCHAR temp[135];
    int rc = sds_getstring(cronly, prompt, temp);
    //acedRemoveFilterWinMsg(MessageFilter);

    result[g_length] = '\0';

    g_result = 0;
    g_length = 0;
    g_cronly = 0;

    return rc;
}

int CDOSLibApp::ads_dos_getsecret()
{
    CAdsArgs args;

    CString str;
    if (!args.GetString(str))
        return RSERR;

    args.Next();

    int cronly = 0;
    if (!args.IsEmpty())
        cronly = 1;

    TCHAR result[135];
    if (adskGetPassword(cronly, str, result) == RTNORM)
        acedRetStr(result);
    else
        acedRetNil();

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_cleardbmod

int CDOSLibApp::ads_dos_cleardbmod()
{
    if (sds_popdbmod() == RTNORM)
        acedRetT();
    else
        acedRetNil();
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_summaryinfo

static bool GetSummaryInfo(int item, CString& str)
{
    bool rc = false;
    try
    {
        if (!curDb())
            return false;

        OdDbDatabaseSummaryInfoPtr si = oddbGetSummaryInfo(curDb());
        if (!si.isNull())
        {
            OdString info;
            switch (item)
            {
                case 0:
                    info = si->getTitle();
                    break;
                case 1:
                    info = si->getSubject();
                    break;
                case 2:
                    info = si->getAuthor();
                    break;
                case 3:
                    info = si->getKeywords();
                    break;
                case 4:
                    info = si->getComments();
                    break;
                case 5:
                    info = si->getLastSavedBy();
                    break;
                case 6:
                    info = si->getRevisionNumber();
                    break;
                case 7:
                    info = si->getHyperlinkBase();
                    break;
            }

            if (!info.isEmpty())
            {
                str = (const TCHAR*)info;
                rc = true;
            }
        }
    }
    catch (OdError& e)
    {
        acutPrintf(L"DOSLib error: %ls\n", (const TCHAR*)e.description());
    }
    return rc;
}

static bool SetSummaryInfo(int item, const wchar_t* info)
{
    bool rc = false;
    try
    {
        if (!curDb())
            return false;

        OdDbDatabaseSummaryInfoPtr si = oddbGetSummaryInfo(curDb());
        if (!si.isNull())
        {

            switch (item)
            {
                case 0:
                    si->setTitle(info);
                    break;
                case 1:
                    si->setSubject(info);
                    break;
                case 2:
                    si->setAuthor(info);
                    break;
                case 3:
                    si->setKeywords(info);
                    break;
                case 4:
                    si->setComments(info);
                    break;
                case 5:
                    si->setLastSavedBy(info);
                    break;
                case 6:
                    si->setRevisionNumber(info);
                    break;
                case 7:
                    si->setHyperlinkBase(info);
                    break;
            }
            oddbPutSummaryInfo(si);
            rc = true;
        }
    }
    catch (OdError& e)
    {
        acutPrintf(L"DOSLib error: %ls\n", (const TCHAR*)e.description());
    }
    return rc;
}


int CDOSLibApp::ads_dos_summaryinfo()
{
    CAdsArgs args;

    int item = -1;
    if (args.IsEmpty())
    {
        CAdsRetList result;
        result.AddLB();

        int i;
        for (i = 0; i < 8; i++)
        {
            result.AddLB();
            result.Add(i);
            result.AddDot();
            CString str;
            GetSummaryInfo(i, str);
            result.Add(str);
            result.AddLE();
        }
        result.AddLE();

        if (result.IsEmpty())
            acedRetNil();
        else
            acedRetList(result);

        return RSRSLT;
    }

    else if (args.IsNil())
    {
        int i;
        for (i = 0; i < 8; i++)
            SetSummaryInfo(i, 0);
        acedRetT();
        return RSRSLT;
    }

    else if (!args.GetInteger(item) || item < 0 || item > 7)
        return RSERR;

    args.Next();

    if (args.IsEmpty())
    {
        CString value;
        if (GetSummaryInfo(item, value))
            acedRetStr(value);
        else
            acedRetNil();
    }
    else
    {
        CString value;
        if (args.GetString(value))
        {
            if (SetSummaryInfo(item, value.IsEmpty() ? (const wchar_t*)0 : (const TCHAR*)value))
                acedRetT();
            else
                acedRetNil();
        }
        else if (args.IsNil())
        {
            if (SetSummaryInfo(item, (const wchar_t*)0))
                acedRetT();
            else
                acedRetNil();
        }
    }

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_custominfo

static void ReturnAllCustomSummaryInfo()
{
    CAdsRetList result;

    if (!curDb())
        return;

    OdDbDatabaseSummaryInfoPtr si = oddbGetSummaryInfo(curDb());
    if (!si.isNull())
    {
        int i, count = si->numCustomInfo();
        if (count)
        {
            result.AddLB();
            for (i = 0; i < count; i++)
            {
                OdString key;
                OdString value;
                si->getCustomSummaryInfo(i, key, value);
                if (!key.isEmpty())
                {
                    result.AddLB();
                    result.Add(key);
                    result.AddDot();
                    result.Add(value);
                    result.AddLE();
                }
            }
            result.AddLE();
        }
    }
    if (result.IsEmpty())
        acedRetNil();
    else
        acedRetList(result);
}

static void DeleteAllCustomSummaryInfo()
{
    if (!curDb())
        return;
    OdDbDatabaseSummaryInfoPtr si = oddbGetSummaryInfo(curDb());
    if (!si.isNull())
    {
        int i, count = si->numCustomInfo();
        for (i = 0; i < count; i++)
            si->deleteCustomSummaryInfo(i);
        oddbPutSummaryInfo(si);
    }
}

static bool GetCustomSummaryInfo(const wchar_t* key, CString& str)
{
    bool rc = false;
    if (!curDb())
        return rc;
    OdDbDatabaseSummaryInfoPtr si = oddbGetSummaryInfo(curDb());
    if (!si.isNull())
    {
        OdString value;
        if (si->getCustomSummaryInfo(key, value))
        {
            str = (const TCHAR*)value;
            rc = true;
        }
    }
    return rc;
}

static bool SetCustomSummaryInfo(const wchar_t* key, const wchar_t* value)
{
    bool rc = false;
    try
    {
        if (!curDb())
            return rc;
        OdDbDatabaseSummaryInfoPtr si = oddbGetSummaryInfo(curDb());
        if (!si.isNull())
        {
            try
            {
                OdString str(value);
                if (str.isEmpty())
                {
                    si->deleteCustomSummaryInfo(key);
                }
                else
                {
                    OdString strVal;
                    if (si->getCustomSummaryInfo(key, str))
                    {
                        si->setCustomSummaryInfo(key, value);
                    }
                    else
                    {
                        si->addCustomSummaryInfo(key, value);
                    }
                }
            }
            catch (...)
            {
                return rc;
            }
            oddbPutSummaryInfo(si);
            rc = true;
        }
    }
    catch (...)
    {
        return rc;
    }
    return rc;
}

int CDOSLibApp::ads_dos_custominfo()
{
    CAdsArgs args;

    CString key;
    if (args.IsEmpty())
    {
        ReturnAllCustomSummaryInfo();
        return RSRSLT;
    }

    else if (args.IsNil())
    {
        DeleteAllCustomSummaryInfo();
        acedRetT();
        return RSRSLT;
    }

    else if (!args.GetString(key))
        return RSERR;

    args.Next();

    if (args.IsEmpty())
    {
        CString value;
        if (GetCustomSummaryInfo(key, value))
            acedRetStr(value);
        else
            acedRetNil();
    }
    else
    {
        CString value;
        if (args.GetString(value))
        {
            if (SetCustomSummaryInfo(key, value.IsEmpty() ? (const wchar_t*)0 : (const TCHAR*)value))
                acedRetT();
            else
                acedRetNil();
        }
        else if (args.IsNil())
        {
            if (SetCustomSummaryInfo(key, (const wchar_t*)0))
                acedRetT();
            else
                acedRetNil();
        }
    }

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_olelist

static INT_PTR GetOleLinks(const wchar_t* entry_name, CStringArray& links)
{
    if (curDb() == nullptr)
        return 0;

    OdDbBlockTablePtr pBT = curDb()->getBlockTableId().openObject();
    if (!pBT->has(entry_name))
        return 0;

    OdDbBlockTableRecordPtr pBTR = pBT->getAt(entry_name).openObject();

    INT_PTR link_count = links.GetCount();

    for (auto iter = pBTR->newIterator(); !iter->done(); iter->step())
    {
        const auto& iterId = iter->objectId();
        if (iterId.objectClass()->isDerivedFrom(OdDbOle2Frame::desc()))
        {
            OdDbOle2FramePtr pOle = iterId.openObject();
            if (OT_LINK == pOle->getType())
            {
                links.Add((const TCHAR*)pOle->getLinkPath());
            }
        }
    }
    return links.GetCount() - link_count;
}

int CDOSLibApp::ads_dos_olelist()
{
    CAdsArgs args;

    int mode = 0;

    if (args.IsEmpty())
        mode = 0;
    else
    {
        if (!args.GetInteger(mode))
            return RSERR;
        mode = DOS_CLAMP(mode, 0, 2);
    }

    CStringArray links;
    switch (mode)
    {
        case 1: // model space
            GetOleLinks(_T("*MODEL_SPACE"), links);
            break;
        case 2: // paper space
            GetOleLinks(_T("*PAPER_SPACE"), links);
            break;
        default:
            GetOleLinks(_T("*MODEL_SPACE"), links);
            GetOleLinks(_T("*PAPER_SPACE"), links);
            break;
    }
    if (links.GetCount())
    {
        CAdsRetList result;
        int i;
        for (i = 0; i < (int)links.GetCount(); i++)
            result.Add(links[i]);
        acedRetList(result);
    }
    else
        acedRetNil();

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_imagelist

int CDOSLibApp::ads_dos_imagelist()
{
    CAdsRetList result;

    if (curDb() == nullptr)
        return 0;

    OdDbDictionaryPtr pNad = curDb()->getNamedObjectsDictionaryId().openObject();
    if (!pNad->has(L"ACAD_IMAGE_DICT"))
        return 0;

    OdDbDictionaryPtr pDictImg = pNad->getAt(L"ACAD_IMAGE_DICT").openObject();
    if (pNad.isNull())
        return 0;

    for (auto i = pDictImg->newIterator(); !i->done(); i->next())
    {
        if (i->objectId().objectClass()->isDerivedFrom(OdDbRasterImageDef::desc()))
        {
            OdDbRasterImageDefPtr pDef = i->objectId().openObject();
            result.Add(pDef->activeFileName());
        }
    }

    if (result.IsEmpty())
        acedRetNil();
    else
        acedRetList(result);
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_purgexdata
int CDOSLibApp::ads_dos_purgexdata()
{
    CAdsArgs args;

    OdDbObjectId entId;
    if (!args.GetEntity(entId))
        return RSERR;

    if (!entId.isValid())
        return RSERR;

    OdDbEntityPtr pEnt = entId.openObject(OdDb::kForWrite);
    if (pEnt.isNull())
        return RSERR;

    bool rc = false;
    while (pEnt->xData(0) != nullptr)
    {
        auto xdata = pEnt->xData(0);
        auto xdata_next = xdata->next();
        xdata->setNext(0);
        pEnt->setXData(xdata);
        xdata->setNext(xdata_next);
        xdata.release();
        rc = true;
    }

    if (rc)
        acedRetT();
    else
        acedRetNil();
    return RSRSLT;
}


////////////////////////////////////////////////////////////////
// dos_proxycount
int CDOSLibApp::ads_dos_proxycount()
{
    if (curDb() == nullptr)
        return RSERR;
    int cnt = 0;
    OdDbBlockTablePtr pBT = curDb()->getBlockTableId().openObject();
    for (auto bti = pBT->newIterator(); !bti->done(); bti->step())
    {
        OdDbBlockTableRecordPtr pBTR = bti->getRecordId().openObject();
        for (auto btri = pBTR->newIterator(); !btri->done(); btri->step())
        {
            if (btri->objectId().objectClass()->isDerivedFrom(OdDbProxyEntity::desc()))
                cnt++;
        }
    }
    acedRetInt(cnt);
    return RSRSLT;
}

// It is possible to use ObjectARX to change all proxy entities for something else.
// The attached sample does the following: Iterate through the entire drawing, and
// swaps all proxy entities for anonymous blocks. The blocks contain the equivalent
// geometry of the proxy entities. Note that this will not allow the creating entity
// to recover the information stored in the original entities.

////////////////////////////////////////////////////////////////
// dos_proxyclean
int CDOSLibApp::ads_dos_proxyclean()
{
#ifndef _TXAPP
    OdDbObjectIdArray proxies;
    OdDbBlockTablePtr pBT = curDb()->getBlockTableId().openObject();
    for (auto bti = pBT->newIterator(); !bti->done(); bti->step())
    {
        OdDbBlockTableRecordPtr pBTR = bti->getRecordId().openObject();
        for (auto btri = pBTR->newIterator(); !btri->done(); btri->step())
        {
            if (btri->objectId().objectClass()->isDerivedFrom(OdDbProxyEntity::desc()))
                proxies.append(btri->objectId());
        }
    }

    OdGePoint2dArray proxies;

    OdDbBlockTable* pTable = 0;
    OdDbHostApplicationServices()->workingDatabase()->getBlockTable(pTable, OdDb::kForWrite);
    if (0 == pTable)
        return RSERR;

    OdDbBlockTableIterator* pTableIter = 0;
    for (pTable->newIterator(pTableIter); !pTableIter->done(); pTableIter->step())
    {
        OdDbBlockTableRecord* pRecord = 0;
        pTableIter->getRecord(pRecord, OdDb::kForWrite);
        if (0 == pRecord)
            continue;

        OdDbBlockTableRecordIterator* pRecordIter = 0;
        for (pRecord->newIterator(pRecordIter); !pRecordIter->done(); pRecordIter->step())
        {
            OdDbEntity* pEnt = 0;
            pRecordIter->getEntity(pEnt, OdDb::kForWrite);
            if (pEnt)
            {
                if (pEnt->isKindOf(OdDbProxyEntity::desc()))
                {
                    proxies.append(pEnt->objectId());
                }
                pEnt->close();
            }
        }

        delete pRecordIter;
        pRecord->close();
    }

    delete pTableIter;

    if (Acad::eOk != pTable->upgradeOpen())
    {
        //pTable->close();  
        //return RSERR;
    }

    int i, j, nProxies = proxies.length();
    int num_cleaned = 0;
    for (i = 0; i < nProxies; i++)
    {
        OdDbProxyEntity* pProxy = 0;
        OdDbObject* pObj = 0;
        OdDbOpenOdDbObject(pObj, proxies[i], OdDb::kForWrite);
        pProxy = OdDbProxyEntity::cast(pObj);
        if (0 == pProxy)
        {
            pObj->close();
            continue;
        }

        OdDbVoidPtrArray explodedEnts;
        pProxy->explode(explodedEnts);
        int nExplodedEnts = explodedEnts.length();
        if (nExplodedEnts > 0)
        {
            OdDbBlockTableRecord* pRecord = new OdDbBlockTableRecord();
            pRecord->setName(L"*U");
            OdGePoint2d blockId;
            pTable->add(blockId, pRecord);
            for (j = 0; j < nExplodedEnts; j++)
            {
                OdDbEntity* pEnt = (OdDbEntity*)(explodedEnts[j]);
                pRecord->appendOdDbEntity(pEnt);
                pEnt->setColorIndex(0);
                pEnt->close();
            }
            pRecord->close();

            OdDbBlockTableRecord* pOwningRecord = 0;
            OdDbOpenObject(pOwningRecord, pProxy->ownerId(), OdDb::kForWrite);
            if (pOwningRecord)
            {
                OdDbBlockReference* pRef = new OdDbBlockReference;
                pRef->setBlockTableRecord(blockId);
                pOwningRecord->close();
                pProxy->upgradeOpen();
                pProxy->handOverTo(pRef);
                pRef->setColor(pProxy->color());
                pRef->setLayer(pProxy->layerId());
                pRef->setVisibility(pProxy->visibility());
                delete pProxy;
                pRef->close();

                num_cleaned++;
            }
        }
        else
        {
            pProxy->close();
        }
    }

    pTable->close();

    acedRetInt(num_cleaned);
#endif
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_plinewinding

bool PolylineWindingNumber(OdDbPolyline* pPoly, int& winding)
{
    if (0 == pPoly || false == pPoly->isClosed())
        return false;

    double turn = 0;
    int nSegs = pPoly->numVerts();
    OdGeVector3dArray startTans(nSegs);
    OdGeVector3dArray endTans(nSegs);

    int i;
    for (i = 0; i < nSegs; i++)
    {
        if (pPoly->segType(i) == OdDbPolyline::kArc)
        {
            OdGeCircArc2d arc;
            pPoly->getArcSegAt(i, arc);
            OdGeVector2d startTan;
            OdGeVector2d endTan;
            OdGeVector2dArray startDerivs;
            arc.evalPoint(arc.startAng(), 1, startDerivs);
            startTan = startDerivs[0];
            OdGeVector2dArray endDerivs;
            arc.evalPoint(arc.endAng(), 1, endDerivs);
            endTan = endDerivs[0];
            startTans.append(OdGeVector3d(startTan.x, startTan.y, 0.0));
            endTans.append(OdGeVector3d(endTan.x, endTan.y, 0.0));
            double ang = arc.endAng() - arc.startAng();
            turn += arc.isClockWise() ? -ang : ang;
        }
        else if (pPoly->segType(i) == OdDbPolyline::kLine)
        {
            OdGeLineSeg2d line;
            pPoly->getLineSegAt(i, line);
            OdGeVector2d tan2d = line.endPoint() - line.startPoint();
            OdGeVector3d tan = OdGeVector3d(tan2d.x, tan2d.y, 0.0);
            startTans.append(tan);
            endTans.append(tan);
        }
    }
    nSegs = startTans.length();

    for (i = 0; i < nSegs; i++)
    {
        double angle = endTans[i].angleTo(startTans[(i + 1) % nSegs]);
        OdGeVector3d norm = endTans[i].crossProduct(startTans[(i + 1) % nSegs]);
        angle = norm.z > 0 ? angle : -angle;
        turn += angle;
    }

    turn = turn / DOS_TWOPI;
    double lower = floor(turn);
    double tol = 1e-6;
    if ((turn - lower) < tol)
        winding = (int)lower;
    else if (((lower + 1) - turn) < tol)
        winding = (int)(lower + 1);
    else
        return false;

    return true;
}


int CDOSLibApp::ads_dos_plinewinding()
{
    CAdsArgs args;

    OdDbObjectId entId;
    if (!args.GetEntity(entId))
        return RSERR;

    OdDbEntityPtr pEnt = entId.openObject();
    if (pEnt.isNull())
    {
        acutPrintf(L"DOSLib error: cannot open this entity for reading.\n");
        return RSERR;
    }

    int winding = 0;
    bool rc = false;

    if (entId.objectClass()->isDerivedFrom(OdDbPolyline::desc()))
    {
        auto pPolyEnt = OdDbPolyline::cast(pEnt);
        if (!pPolyEnt.isNull())
        {
            if (true == pPolyEnt->isClosed())
                rc = PolylineWindingNumber(pPolyEnt, winding);
            else
                acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
        }
    }
    else if (entId.objectClass()->isDerivedFrom(OdDb2dPolyline::desc()))
    {
        auto pPoly2dEnt = OdDb2dPolyline::cast(pEnt);
        if (!pPoly2dEnt.isNull())
        {

            if (true == pPoly2dEnt->isClosed())
            {
                auto pPolyEnt = OdDbPolyline::createObject();
                if (!pPolyEnt.isNull())
                {
                    if (eOk == pPolyEnt->convertFrom(pPoly2dEnt, false))
                        rc = PolylineWindingNumber(pPolyEnt, winding);;
                }
            }
            else
                acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
        }
    }
    else
    {
        acutPrintf(L"DOSLib error: entity not a closed polyline.\n");
    }

    if (rc == true)
        acedRetInt(winding);
    else
        acedRetNil();

    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_acadname
int CDOSLibApp::ads_dos_acadname()
{
    if (odapDocManager()->curDocument() == nullptr)
        return RSERR;
    OdDbDatabase* pDb = odapDocManager()->curDocument()->database();
    OdString appVer;
    appVer.format(_T("%ls %ls"), (const TCHAR*)pDb->appServices()->product(), (const TCHAR*)pDb->appServices()->versionString());
    acedRetStr(appVer);
    return RSRSLT;
}

////////////////////////////////////////////////////////////////
// dos_slblist

static const char SLIDEHEADER[32] = "AutoCAD Slide Library 1.0\015\012\032";

typedef struct tagSlideEntry
{
    char m_name[32];
    DWORD m_offset;
} SLIDEENTRY, * LPSLIDEENTRY;

int CDOSLibApp::ads_dos_slblist()
{
    CAdsArgs args;

    CDosPathString path;
    if (!args.GetPathString(path) || !path.FileExists())
        return RSERR;

    bool bSort = false;
    bool bAscending = true;

    args.Next();

    if (!args.IsEmpty())
    {
        bSort = true;
        if (args.IsNil())
            bAscending = false;
    }

    CFileException ex;
    CFile file;
    if (!file.Open(path, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan, &ex))
    {
        wchar_t szError[1024];
        ex.GetErrorMessage(szError, 1024);
        DOS_PrintError(szError);
        acedRetNil();
        return RSRSLT;
    }

    char header[32];
    memset(&header, 0, 32);
    if (32 != file.Read(header, 32) || 0 != memcmp(header, SLIDEHEADER, 32))
    {
        file.Close();
        acedRetNil();
        return RSRSLT;
    }

    CDosStringArray names;

    SLIDEENTRY entry;
    size_t length = 0;
    do
    {
        memset(&entry, 0, sizeof(SLIDEENTRY));
        if (sizeof(SLIDEENTRY) == file.Read(&entry, sizeof(SLIDEENTRY)))
        {
            if (entry.m_name && entry.m_name[0])
                names.Add(CString(entry.m_name));
            else
                break;
        }
        else
            break;
    } while (true);

    file.Close();

    if (bSort)
        names.Sort(false, bAscending);

    CAdsRetList result;
    for (int i = 0; i < (int)names.GetCount(); i++)
        result.Add(names[i]);

    if (result.IsEmpty())
        acedRetNil();
    else
        acedRetList(result);

    return RSRSLT;
}