/////////////////////////////////////////////////////////////////////////////
// DosHtmlDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "stdafx.h"
#include "DosHtmlDialog.h"

CDosHtmlDialog::CDosHtmlDialog(CWnd* pParent)
    : CDLDialogEx(CDosHtmlDialog::IDD, pParent)
{
    m_sTitle = L"";
    m_sAddress = L"";
    m_bSize = FALSE;
    m_dx = 0;
    m_dy = 0;
}

void CDosHtmlDialog::DoDataExchange(CDataExchange* pDX)
{
    CDLDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EXPLORER, m_webBrowser);
}

BEGIN_MESSAGE_MAP(CDosHtmlDialog, CDLDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_SIZE()
    ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosHtmlDialog::OnInitDialog()
{
    SetDialogName(L"DOSLib:HtmlBox");

    CDLDialogEx::OnInitDialog();

    StretchControlXY(IDC_EXPLORER, 100, 100);

    // Set the initial address
    SetWindowText(m_sTitle);
    SetWebAddress(m_sAddress);

    if (m_bSize)
    {
        SetWindowPos(CWnd::FromHandle(adsw_acadMainWnd()), 0, 0, m_dx, m_dy, SWP_NOMOVE);
        CenterWindow();
    }

    return TRUE;
}

void CDosHtmlDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
    CDLDialogEx::OnSysCommand(nID, lParam);
}

void CDosHtmlDialog::SetWebAddress(LPCTSTR lpAddress)
{
    COleVariant vtEmpty;
    m_webBrowser.Navigate(lpAddress, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
    ResizeWindow();
}

void CDosHtmlDialog::ResizeWindow()
{
    if (::IsWindow(m_webBrowser.m_hWnd))
    {
        CRect rect;
        GetClientRect(rect);
        m_webBrowser.MoveWindow(rect);
    }
}

void CDosHtmlDialog::OnSize(UINT nType, int cx, int cy)
{
    CDLDialogEx::OnSize(nType, cx, cy);
    ResizeWindow();
}

BEGIN_EVENTSINK_MAP(CDosHtmlDialog, CDLDialogEx)
    ON_EVENT(CDosHtmlDialog, IDC_EXPLORER, 113 /* TitleChange */, OnTitleChangeExplorer, VTS_BSTR)
END_EVENTSINK_MAP()

void CDosHtmlDialog::OnTitleChangeExplorer(LPCTSTR Text)
{
}

void CDosHtmlDialog::OnCancel()
{
    if (!m_bSize)
        CDLDialogEx::OnOK();
    else
        CDLDialogEx::OnCancel();
}

LRESULT CDosHtmlDialog::OnNcHitTest(CPoint point)
{
    LRESULT hit = CDLDialogEx::OnNcHitTest(point);
    if (hit == HTCLIENT)
        return HTCAPTION;
    return hit;
}


////////////////////////////////////////////////////////////////

CDosModelessHtmlDialog::CDosModelessHtmlDialog(CWnd* pParent)
    : CDLDialogEx(CDosModelessHtmlDialog::IDD, pParent)
{
    m_sTitle = L"";
    m_sAddress = L"";
    m_bSize = FALSE;
    m_dx = 0;
    m_dy = 0;
}

void CDosModelessHtmlDialog::DoDataExchange(CDataExchange* pDX)
{
    CDLDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EXPLORER, m_webBrowser);
}

BEGIN_MESSAGE_MAP(CDosModelessHtmlDialog, CDLDialogEx)
    ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
    ON_WM_SYSCOMMAND()
    ON_WM_SIZE()
    ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosModelessHtmlDialog::OnInitDialog()
{
    SetDialogName(L"DOSLib:HtmlBox2");

    CDLDialogEx::OnInitDialog();

    StretchControlXY(IDC_EXPLORER, 100, 100);

    // Set the initial address
    SetWindowText(m_sTitle);
    SetWebAddress(m_sAddress);

    if (m_bSize)
    {
        SetWindowPos(CWnd::FromHandle(adsw_acadMainWnd()), 0, 0, m_dx, m_dy, SWP_NOMOVE);
        CenterWindow();
    }

    return TRUE;
}

void CDosModelessHtmlDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
    CDLDialogEx::OnSysCommand(nID, lParam);
}

void CDosModelessHtmlDialog::SetWebAddress(LPCTSTR lpAddress)
{
    COleVariant vtEmpty;
    m_webBrowser.Navigate(lpAddress, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
    ResizeWindow();
}

void CDosModelessHtmlDialog::ResizeWindow()
{
    if (::IsWindow(m_webBrowser.m_hWnd))
    {
        CRect rect;
        GetClientRect(rect);
        m_webBrowser.MoveWindow(rect);
    }
}

void CDosModelessHtmlDialog::OnSize(UINT nType, int cx, int cy)
{
    CDLDialogEx::OnSize(nType, cx, cy);
    ResizeWindow();
}

BEGIN_EVENTSINK_MAP(CDosModelessHtmlDialog, CDLDialogEx)
    ON_EVENT(CDosModelessHtmlDialog, IDC_EXPLORER, 113 /* TitleChange */, OnTitleChangeExplorer, VTS_BSTR)
END_EVENTSINK_MAP()

void CDosModelessHtmlDialog::OnTitleChangeExplorer(LPCTSTR Text)
{
}

void CDosModelessHtmlDialog::OnCancel()
{
    DestroyWindow();
}

LRESULT CDosModelessHtmlDialog::OnNcHitTest(CPoint point)
{
    LRESULT hit = CDLDialogEx::OnNcHitTest(point);
    if (hit == HTCLIENT)
        return HTCAPTION;
    return hit;
}

void CDosModelessHtmlDialog::KillDialog()
{
    OnCancel();
}

LRESULT CDosModelessHtmlDialog::OnAcadKeepFocus(WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

void CDosModelessHtmlDialog::PostNcDestroy()
{
    CDLDialogEx::PostNcDestroy();
    delete this;
}

void CDosModelessHtmlDialog::Refresh()
{
    if (!m_sTitle.IsEmpty())
        SetWindowText(m_sTitle);

    if (!m_sAddress.IsEmpty())
        SetWebAddress(m_sAddress);
}
