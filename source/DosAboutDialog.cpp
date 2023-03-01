/////////////////////////////////////////////////////////////////////////////
// DosAboutDialog.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosAboutDialog.h"
#include "DOSLibApp.h"

CDosAboutDialog::CDosAboutDialog(CWnd* pParent)
    : CDLDialogEx(CDosAboutDialog::IDD, pParent)
{
}

void CDosAboutDialog::DoDataExchange(CDataExchange* pDX)
{
    CDLDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DOSLIBICON, m_icon);
    DDX_Control(pDX, IDC_COPYRIGHT, m_copyright);
    DDX_Control(pDX, IDC_PRODUCT, m_product);
    DDX_Control(pDX, IDC_WEB, m_web);
}

BEGIN_MESSAGE_MAP(CDosAboutDialog, CDLDialogEx)
    ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosAboutDialog::OnInitDialog()
{
    SetDialogName(L"DOSLib:About");
    CDLDialogEx::OnInitDialog();
    LockDialogWidth();
    LockDialogHeight();

    CString str;
    str.Format(L"%s Version %s", CDOSLibApp::getInstance()->AppName(), CDOSLibApp::getInstance()->AppVersion());
    m_product.SetWindowText(str);
    m_copyright.SetWindowText(CDOSLibApp::getInstance()->AppCopyright());
    m_web.SetWindowText(CDOSLibApp::getInstance()->AppInternet());
    m_icon.m_link = CDOSLibApp::getInstance()->AppInternet();
    m_web.m_link = CDOSLibApp::getInstance()->AppInternet();
    return TRUE;
}

LRESULT CDosAboutDialog::OnNcHitTest(CPoint point)
{
    LRESULT hit = CDLDialogEx::OnNcHitTest(point);
    if (hit == HTCLIENT)
        return HTCAPTION;
    return hit;
}
