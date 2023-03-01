/////////////////////////////////////////////////////////////////////////////
// DosColorDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosColorDialog.h"

IMPLEMENT_DYNAMIC(CDosColorDialog, CColorDialog)

CDosColorDialog::CDosColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd)
    : CColorDialog(clrInit, dwFlags, pParentWnd)
{
    m_Title = L"Color";
}

BEGIN_MESSAGE_MAP(CDosColorDialog, CColorDialog)
END_MESSAGE_MAP()

BOOL CDosColorDialog::OnInitDialog()
{
    CColorDialog::OnInitDialog();
    loadIcon();
    setTheme();
    SetWindowText(m_Title);
    return TRUE;
}

void CDosColorDialog::loadIcon()
{
    ModifyStyle(0, WS_POPUPWINDOW);
    HICON hIcon = LoadIcon(_hdllInstance, MAKEINTRESOURCE(IDI_DOSLIB));
    SetIcon(hIcon, FALSE);
}

void CDosColorDialog::setTheme()
{
    resbuf rb;
    sds_getvar(_T("COLORTHEME"), &rb);
    if (rb.resval.rint == 1)
        return;

    auto hwnd = this->GetSafeHwnd();
    constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_I20 = 20;
    BOOL USE_DARK_MODE = true;
    BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
        hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE_I20, &USE_DARK_MODE, sizeof(USE_DARK_MODE)));
    const auto style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, 0);
    SetWindowLong(hwnd, GWL_STYLE, style);
}
