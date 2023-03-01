#pragma once
#include <afxdialogex.h>

class CDLDialogEx : public CDialogEx
{
public:
    DECLARE_DYNAMIC(CDLDialogEx)

    // Construction
public:
    CDLDialogEx();
    CDLDialogEx(UINT nIDTemplate, CWnd* pParent = NULL);
    CDLDialogEx(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
    virtual ~CDLDialogEx() override;

    virtual BOOL OnInitDialog() override;

    void SetDialogName(const CString& name);
    void LockDialogWidth();
    void LockDialogHeight();

    BOOL StretchControlX(UINT id, LONG lStretchPct);
    BOOL StretchControlY(UINT id, LONG lStretchPct);
    BOOL StretchControlXY(UINT id, LONG lStretchXPct, LONG lStretchYPct);
    BOOL MoveControlX(UINT id, LONG lMovePct);
    BOOL MoveControlY(UINT id, LONG lMovePct);
    BOOL MoveControlXY(UINT id, LONG lMoveXPct, LONG lMoveYPct);

    afx_msg void OnDestroy();

protected:
    void loadIcon();
    void setTheme();
    void cleanSysMenu();
    void resetDynamicLayout();
    void saveToSettings();
    void restoreFromSettings();

protected:
    DECLARE_MESSAGE_MAP()

protected:
    CString m_dialogName;
};

