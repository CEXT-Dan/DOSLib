#include "StdAfx.h"
#include "CDLDialogEx.h"


//for theme
#include "dwmapi.h"
#pragma comment( lib, "dwmapi.lib")

//
IMPLEMENT_DYNAMIC(CDLDialogEx, CDialogEx)
//
BEGIN_MESSAGE_MAP(CDLDialogEx, CDialogEx)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

CDLDialogEx::CDLDialogEx()
    : CDialogEx()
{
    resetDynamicLayout();
}

CDLDialogEx::CDLDialogEx(UINT nIDTemplate, CWnd* pParent /*= NULL*/)
    : CDialogEx(nIDTemplate, pParent /*= NULL*/)
{
    resetDynamicLayout();
}

CDLDialogEx::CDLDialogEx(LPCTSTR lpszTemplateName, CWnd* pParentWnd /*= NULL*/)
    : CDialogEx(lpszTemplateName, pParentWnd /*= NULL*/)
{
    resetDynamicLayout();
}

CDLDialogEx::~CDLDialogEx()
{
    EnableDynamicLayout(FALSE);
}

BOOL CDLDialogEx::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    loadIcon();
    cleanSysMenu();
    setTheme();
    restoreFromSettings();
    return TRUE;
}

void CDLDialogEx::SetDialogName(const CString& name)
{
    m_dialogName = name;
}

void CDLDialogEx::LockDialogWidth()
{
    //TODO:
}

void CDLDialogEx::LockDialogHeight()
{
    //TODO:
}

BOOL CDLDialogEx::StretchControlX(UINT id, LONG lStretchPct)
{
    CMFCDynamicLayout* dynamicLayout = GetDynamicLayout();
    if (dynamicLayout == nullptr)
        return FALSE;
    return dynamicLayout->AddItem(id, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontal(lStretchPct));
}

BOOL CDLDialogEx::StretchControlY(UINT id, LONG lStretchPct)
{
    CMFCDynamicLayout* dynamicLayout = GetDynamicLayout();
    if (dynamicLayout == nullptr)
        return FALSE;
    return dynamicLayout->AddItem(id, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeVertical(lStretchPct));
}

BOOL CDLDialogEx::StretchControlXY(UINT id, LONG lStretchXPct, LONG lStretchYPct)
{
    CMFCDynamicLayout* dynamicLayout = GetDynamicLayout();
    if (dynamicLayout == nullptr)
        return FALSE;
    return dynamicLayout->AddItem(id, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(lStretchXPct, lStretchYPct));
}

BOOL CDLDialogEx::MoveControlX(UINT id, LONG lMovePct)
{
    CMFCDynamicLayout* dynamicLayout = GetDynamicLayout();
    if (dynamicLayout == nullptr)
        return FALSE;
    return dynamicLayout->AddItem(id, CMFCDynamicLayout::MoveHorizontal(lMovePct), CMFCDynamicLayout::SizeNone());
}

BOOL CDLDialogEx::MoveControlY(UINT id, LONG lMovePct)
{
    CMFCDynamicLayout* dynamicLayout = GetDynamicLayout();
    if (dynamicLayout == nullptr)
        return FALSE;
    return dynamicLayout->AddItem(id, CMFCDynamicLayout::MoveVertical(lMovePct), CMFCDynamicLayout::SizeNone());
}

BOOL CDLDialogEx::MoveControlXY(UINT id, LONG lMoveXPct, LONG lMoveYPct)
{
    CMFCDynamicLayout* dynamicLayout = GetDynamicLayout();
    if (dynamicLayout == nullptr)
        return FALSE;
    return dynamicLayout->AddItem(id, CMFCDynamicLayout::MoveHorizontalAndVertical(lMoveXPct, lMoveYPct), CMFCDynamicLayout::SizeNone());
}

void CDLDialogEx::OnDestroy()
{
    saveToSettings();
}

void CDLDialogEx::loadIcon()
{
    ModifyStyle(0, WS_POPUPWINDOW);
    HICON hIcon = LoadIcon(_hdllInstance, MAKEINTRESOURCE(IDI_DOSLIB));
    SetIcon(hIcon, FALSE);
}

void CDLDialogEx::setTheme()
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

void CDLDialogEx::cleanSysMenu()
{
    CMenu* pSystemMenu = GetSystemMenu(FALSE);
    pSystemMenu->EnableMenuItem(SC_MINIMIZE, MF_DISABLED);
    pSystemMenu->DeleteMenu(SC_MINIMIZE, MF_BYCOMMAND);
    pSystemMenu->EnableMenuItem(SC_MAXIMIZE, MF_DISABLED);
    pSystemMenu->DeleteMenu(SC_MAXIMIZE, MF_BYCOMMAND);
    pSystemMenu->EnableMenuItem(SC_RESTORE, MF_DISABLED);
    pSystemMenu->DeleteMenu(SC_RESTORE, MF_BYCOMMAND);
}

void CDLDialogEx::resetDynamicLayout()
{
    EnableDynamicLayout(FALSE);
    EnableDynamicLayout(TRUE);
}

void CDLDialogEx::saveToSettings()
{
    CRect rect;
    GetWindowRect(&rect);

    std::map<int, CRect> subMap;
    subMap.emplace(this->GetDlgCtrlID(), rect);

    CWnd* pChild = GetWindow(GW_CHILD);
    while (pChild != nullptr)
    {
        CRect childRect;
        pChild->GetWindowRect(childRect);
        subMap.emplace(pChild->GetDlgCtrlID(), childRect);
        pChild = pChild->GetNextWindow();
    }
    CDLDialogExMap::instance().crectMap[m_dialogName] = subMap;
}

void CDLDialogEx::restoreFromSettings()
{
    if (CDLDialogExMap::instance().crectMap.contains(m_dialogName))
    {
        const auto& subMap = CDLDialogExMap::instance().crectMap.at(m_dialogName);

        if (subMap.contains(this->GetDlgCtrlID()))
            this->MoveWindow(subMap.at(this->GetDlgCtrlID()));

        CWnd* pChild = GetWindow(GW_CHILD);
        while (pChild != nullptr)
        {
            if (subMap.contains(pChild->GetDlgCtrlID()))
            {
                CRect childRect = subMap.at(pChild->GetDlgCtrlID());
                ScreenToClient(&childRect);
                pChild->MoveWindow(childRect);
            }
            pChild = pChild->GetNextWindow();
        }
    }
}
