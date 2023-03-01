/////////////////////////////////////////////////////////////////////////////
// DosDwgPreviewDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosDwgPreviewDialog.h"
#include "DosFolderDialog.h"

BEGIN_MESSAGE_MAP(CDosDwgPreviewCtrl, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CDosDwgPreviewCtrl::readDb()
{
    if (!curDb())
        return;
    try
    {
        if (modified)
        {
            bmpBytes.clear();
            OdDbDatabasePtr pDb = curDb()->appServices()->createDatabase(false);
            pDb->readFile((const TCHAR*)m_strFilename, true);

            OdUInt32 len = 0;
            const void* buf = pDb->thumbnailBitmap(len);
            if (len == 0)
                return;
            bmpBytes = std::vector<uint8_t>(len);
            memcpy_s(&bmpBytes[0], bmpBytes.size(), buf, len);
            modified = false;
        }
    }
    catch (OdError& e)
    {
        acutPrintf(_T("\n%ls"), (const TCHAR*)e.description());
    }
}

void CDosDwgPreviewCtrl::OnPaint()
{
    CRect rc, rcframe;
    GetWindowRect(rcframe);
    GetClientRect(rc);
    ClientToScreen(rc);
    int cx = rcframe.Width() - rc.Width();
    int cy = rcframe.Height() - rc.Height();
    ScreenToClient(rc);
    rc.DeflateRect(cx / 2, cy / 2);
    auto brush = CBrush(GetSysColor(COLOR_BTNFACE));
    GetDC()->FillRect(rc, &brush);
    CStatic::OnPaint();
    readDb();
    //
    if (bmpBytes.size())
    {
        LPBITMAPINFO info = (LPBITMAPINFO)bmpBytes.data();
        std::unique_ptr<Gdiplus::Bitmap> image(Gdiplus::Bitmap::FromBITMAPINFO(info, &bmpBytes[31]));
        if (image->GetLastStatus() != 0)
            return;
        Gdiplus::Graphics g(GetDC()->GetSafeHdc());
        g.DrawImage(image.get(), Gdiplus::Rect(0, 0, rc.right, rc.bottom));
    }
}


CDosDwgPreviewDialog::CDosDwgPreviewDialog(CWnd* pParent)
    : CDLDialogEx(CDosDwgPreviewDialog::IDD, pParent)
{
    m_Title = L"";
    m_Prompt = L"";
}

void CDosDwgPreviewDialog::DoDataExchange(CDataExchange* pDX)
{
    CDLDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_List);
    DDX_Control(pDX, IDC_STATIC_FRAME, m_DwgPreview);
}

BEGIN_MESSAGE_MAP(CDosDwgPreviewDialog, CDLDialogEx)
    ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
    ON_WM_PAINT()
    ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
    ON_WM_SIZE()
    ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
    ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CDosDwgPreviewDialog::OnBrowse()
{
    CDosFolderDialog dlg(adsw_acadMainWnd());
    dlg.SetDialogTitle(L"Select a folder containing drawing files");
    dlg.SetDialogFolder(m_Prompt);
    if (dlg.DoModal() == IDOK)
    {
        m_Prompt = dlg.GetFolderName();
        SetDlgItemText(IDC_PROMPT, m_Prompt);
        FillListBox();
    }
}

void CDosDwgPreviewDialog::OnPaint()
{
    CPaintDC dc(this);
}

BOOL CDosDwgPreviewDialog::OnInitDialog()
{
    SetDialogName(L"DOSLib:DwgPreview");

    CDLDialogEx::OnInitDialog();

    StretchControlY(IDC_LIST, 100);
    StretchControlXY(IDC_STATIC_FRAME, 100, 100);
    MoveControlY(IDOK, 100);
    MoveControlY(IDCANCEL, 100);
    MoveControlY(IDC_BROWSE, 100);

    SetWindowText(m_Title);
    SetDlgItemText(IDC_PROMPT, m_Prompt);

    FillListBox();

    return TRUE;
}

void CDosDwgPreviewDialog::FillListBox()
{
    m_List.ResetContent();

    if (m_StrArray.GetCount() > 0)
    {
        int i;
        for (i = 0; i < m_StrArray.GetCount(); i++)
            m_List.AddString(m_StrArray[i]);
        m_List.SetCurSel(0);
        OnSelchangeList();
        GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
        return;
    }

    CString str = m_Prompt;
    str += L"\\*.dwg";

    CFileFind finder;
    BOOL bWorking = finder.FindFile(str);
    if (!bWorking)
        return;

    while (bWorking)
    {
        bWorking = finder.FindNextFile();
        if (finder.IsDots())
            continue;
        if (finder.IsDirectory())
            continue;
        m_List.AddString(finder.GetFileName());
    }

    m_List.SetCurSel(0);
    OnSelchangeList();
}

void CDosDwgPreviewDialog::OnOK()
{
    m_Prompt = m_DwgPreview.m_strFilename;
    CDLDialogEx::OnOK();
}

void CDosDwgPreviewDialog::OnSelchangeList()
{
    CString cFile;
    int nSel = m_List.GetCurSel();
    m_List.GetText(nSel, cFile);

    if (m_StrArray.GetCount() > 0)
        m_DwgPreview.m_strFilename = cFile;
    else
        m_DwgPreview.m_strFilename.Format(L"%s\\%s", m_Prompt, cFile);

    m_DwgPreview.m_nColor = m_Color;
    m_DwgPreview.modified = true;
    m_DwgPreview.Invalidate();
}

void CDosDwgPreviewDialog::OnSize(UINT nType, int cx, int cy)
{
    CDLDialogEx::OnSize(nType, cx, cy);
    m_DwgPreview.Invalidate();
}

void CDosDwgPreviewDialog::OnDblclkList()
{
    OnOK();
}

LRESULT CDosDwgPreviewDialog::OnNcHitTest(CPoint point)
{
    LRESULT hit = CDLDialogEx::OnNcHitTest(point);
    if (hit == HTCLIENT)
        return HTCAPTION;
    return hit;
}
