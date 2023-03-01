/////////////////////////////////////////////////////////////////////////////
// DosDwgPreviewDialog.h
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#pragma once

class CDosDwgPreviewCtrl : public CStatic
{
public:
    CDosDwgPreviewCtrl() {}
    virtual ~CDosDwgPreviewCtrl() {}

    CString m_strFilename;
    unsigned long m_nColor = 0;

    bool modified = true;
    std::vector<uint8_t> bmpBytes;


protected:
    void readDb();
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CDosDwgPreviewDialog : public CDLDialogEx
{
public:
    CDosDwgPreviewDialog(CWnd* pParent);

    enum { IDD = IDD_DWGPREVIEW };
    CListBox m_List;
    CDosDwgPreviewCtrl m_DwgPreview;

    CString m_Title;
    CString m_Prompt;
    uint32_t m_Color = 0;
    CStringArray m_StrArray;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    void FillListBox();

protected:
    afx_msg void OnBrowse();
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnSelchangeList();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDblclkList();
    afx_msg LRESULT OnNcHitTest(CPoint point);
    DECLARE_MESSAGE_MAP()
};
