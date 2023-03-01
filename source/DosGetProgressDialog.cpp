/////////////////////////////////////////////////////////////////////////////
// DosGetProgressDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosGetProgressDialog.h"

CDosGetProgressDialog::CDosGetProgressDialog(CWnd* pParent)
    : CDLDialogEx(CDosGetProgressDialog::IDD, pParent)
{
}

void CDosGetProgressDialog::DoDataExchange(CDataExchange* pDX)
{
    CDLDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDCANCEL, m_Cancel);
    DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}

BEGIN_MESSAGE_MAP(CDosGetProgressDialog, CDLDialogEx)
    ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
    ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDosGetProgressDialog::OnInitDialog()
{
    SetDialogName(L"DOSLib:GetProgress");

    CDLDialogEx::OnInitDialog();

    LockDialogHeight();
    StretchControlX(IDC_PROMPT, 100);
    StretchControlX(IDC_PROGRESS, 100);
    MoveControlX(IDCANCEL, 50);

    SetWindowText(m_Title);
    SetDlgItemText(IDC_PROMPT, m_Prompt);

    m_Progress.SetShowText(TRUE);
    m_Progress.SetRange(0, m_nUpper);
    m_Progress.SetPos(0);

    // cancel button
    //m_Cancel.EnableWindow(m_bCancel);
    if (m_bCancel == FALSE)
        m_Cancel.ShowWindow(SW_HIDE);

    CenterWindow();

    return TRUE;
}

void CDosGetProgressDialog::KillDialog()
{
    OnCancel();
}

void CDosGetProgressDialog::OnCancel()
{
    DestroyWindow();
}

LRESULT CDosGetProgressDialog::OnAcadKeepFocus(WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

void CDosGetProgressDialog::PostNcDestroy()
{
    CDLDialogEx::PostNcDestroy();
    delete this;
}

LRESULT CDosGetProgressDialog::OnNcHitTest(CPoint point)
{
    LRESULT hit = CDLDialogEx::OnNcHitTest(point);
    if (hit == HTCLIENT)
        return HTCAPTION;
    return hit;
}
