/////////////////////////////////////////////////////////////////////////////
// DosLayerListDialog.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DosLayerListDialog.h"

CDosLayerListDialog::CDosLayerListDialog(CWnd* pParent)
    : CDLDialogEx(CDosLayerListDialog::IDD, pParent)
{
    m_title = L"";
    m_prompt = L"";
    m_layer = L"";
    m_layers.RemoveAll();

    m_hide_on_layers = false;
    m_hide_off_layers = false;

    m_hide_unlocked_layers = false;
    m_hide_locked_layers = false;

    m_hide_thawed_layers = false;
    m_hide_frozen_layers = false;

    m_hide_std_layers = false;
    m_hide_xref_layers = false;

    m_multiple_selection = false;

    m_hide_plot_layers = false;
    m_hide_noplot_layers = false;

    m_hide_used_layers = false;
    m_hide_unused_layers = false;

    m_used_layer_names.RemoveAll();
    m_unused_layer_names.RemoveAll();
}

void CDosLayerListDialog::DoDataExchange(CDataExchange* pDX)
{
    CDLDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_listbox);
    DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CDosLayerListDialog, CDLDialogEx)
    ON_LBN_DBLCLK(IDC_LIST, OnDblClkList)
    ON_LBN_SELCHANGE(IDC_LIST, OnSelChangeList)
END_MESSAGE_MAP()

void CDosLayerListDialog::SetTitle(LPCTSTR lp)
{
    m_title = lp;
}

void CDosLayerListDialog::SetPrompt(LPCTSTR lp)
{
    m_prompt = lp;
}

bool CDosLayerListDialog::MultipleSelection()
{
    return m_multiple_selection;
}

void CDosLayerListDialog::SetMultipleSelection(bool b)
{
    m_multiple_selection = b;
}

void CDosLayerListDialog::HideOnLayers(bool b)
{
    m_hide_on_layers = b;
}

void CDosLayerListDialog::HideOffLayers(bool b)
{
    m_hide_off_layers = b;
}

void CDosLayerListDialog::HideUnlockedLayers(bool b)
{
    m_hide_unlocked_layers = b;
}

void CDosLayerListDialog::HideLockedLayers(bool b)
{
    m_hide_locked_layers = b;
}

void CDosLayerListDialog::HideThawedLayers(bool b)
{
    m_hide_thawed_layers = b;
}

void CDosLayerListDialog::HideFrozenLayers(bool b)
{
    m_hide_frozen_layers = b;
}

void CDosLayerListDialog::HideStdLayers(bool b)
{
    m_hide_std_layers = b;
}

void CDosLayerListDialog::HideXrefLayers(bool b)
{
    m_hide_xref_layers = b;
}

void CDosLayerListDialog::HidePlotLayers(bool b)
{
    m_hide_plot_layers = b;
}

void CDosLayerListDialog::HideNoPlotLayers(bool b)
{
    m_hide_noplot_layers = b;
}

void CDosLayerListDialog::HideUsedLayers(bool b)
{
    m_hide_used_layers = b;
}

void CDosLayerListDialog::HideUnusedLayers(bool b)
{
    m_hide_unused_layers = b;
}

BOOL CDosLayerListDialog::RecreateListBox(CListBox* pList, LPVOID lpParam/*=NULL*/)
{
    if (pList == NULL)
        return FALSE;
    if (pList->GetSafeHwnd() == NULL)
        return FALSE;

    CWnd* pParent = pList->GetParent();
    if (pParent == NULL)
        return FALSE;

    // Get current attributes
    DWORD dwStyle = pList->GetStyle();
    DWORD dwStyleEx = pList->GetExStyle();

    CRect rc;
    pList->GetWindowRect(&rc);
    pParent->ScreenToClient(&rc);

    UINT nID = pList->GetDlgCtrlID();
    CFont* pFont = pList->GetFont();
    CWnd* pWndAfter = pList->GetNextWindow(GW_HWNDPREV);

    // Create the new list box and copy the old list box items 
    // into a new listbox along with each item's data, and selection state
    CListBox listNew;
    if (!listNew.CreateEx(dwStyleEx, L"LISTBOX", L"", dwStyle, rc, pParent, nID, lpParam))
        return FALSE;

    listNew.SetFont(pFont);
    int nNumItems = pList->GetCount();
    BOOL bMultiSel = (dwStyle & LBS_MULTIPLESEL || dwStyle & LBS_EXTENDEDSEL);

    for (int n = 0; n < nNumItems; n++)
    {
        CString sText;
        pList->GetText(n, sText);
        int nNewIndex = listNew.AddString(sText);
        listNew.SetItemData(nNewIndex, pList->GetItemData(n));
        if (bMultiSel && pList->GetSel(n))
            listNew.SetSel(nNewIndex);
    }

    if (!bMultiSel)
    {
        int nCurSel = pList->GetCurSel();
        if (nCurSel != -1)
        {
            CString sSelText;
            pList->GetText(nCurSel, sSelText);
            listNew.SetCurSel(listNew.FindStringExact(-1, sSelText));
        }
    }

    pList->DestroyWindow();
    HWND hwnd = listNew.Detach();
    pList->Attach(hwnd);
    pList->SetWindowPos(pWndAfter == NULL ? &CWnd::wndBottom : pWndAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    return TRUE;
}

BOOL CDosLayerListDialog::OnInitDialog()
{
    SetDialogName(L"DOSLib:LayerListBox");

    CDLDialogEx::OnInitDialog();

    if (m_multiple_selection)
    {
        m_listbox.ModifyStyle(0, LBS_MULTIPLESEL | LBS_EXTENDEDSEL | WS_VSCROLL);
        RecreateListBox(&m_listbox);
    }

    StretchControlX(IDC_PROMPT, 100);
    StretchControlXY(IDC_LIST, 100, 100);
    MoveControlX(IDOK, 100);
    MoveControlX(IDCANCEL, 100);

    SetWindowText(m_title);
    SetDlgItemText(IDC_PROMPT, m_prompt);

    if (m_hide_used_layers || m_hide_unused_layers)
        ProcessLayers();

    FillListBox();
    OnSelChangeList();

    return TRUE;
}

void CDosLayerListDialog::FillListBox()
{
    OdApDocument* pDoc = odapDocManager()->curDocument();
    if (pDoc == nullptr)
        return;

    OdDbLayerTablePtr pTable = pDoc->database()->getLayerTableId().safeOpenObject();
    auto pIterator = pTable->newIterator();

    while (!pIterator->done())
    {
        OdDbLayerTableRecordPtr pRecord = pIterator->getRecord();
        if (!pRecord.isNull())
        {
            OdString pName = pRecord->getName();
            auto color = pRecord->color();

            bool bDisplayLayer = true;

            // If the layer is on
            if (m_hide_on_layers && !pRecord->isOff())
                bDisplayLayer = false;

            // If the layer is off
            if (m_hide_off_layers && pRecord->isOff())
                bDisplayLayer = false;

            // If the layer is thawed
            if (m_hide_thawed_layers && !pRecord->isFrozen())
                bDisplayLayer = false;

            // If the layer is frozen
            if (m_hide_frozen_layers && pRecord->isFrozen())
                bDisplayLayer = false;

            // If the layer is unlocked
            if (m_hide_unlocked_layers && !pRecord->isLocked())
                bDisplayLayer = false;

            // If the layer is locked
            if (m_hide_locked_layers && pRecord->isLocked())
                bDisplayLayer = false;

            // If the layer is standard
            if (m_hide_std_layers && !pRecord->isDependent())
                bDisplayLayer = false;

            // If the layer is xref
            if (m_hide_xref_layers && pRecord->isDependent())
                bDisplayLayer = false;

            // If the layer is plotable
            if (m_hide_plot_layers && pRecord->isPlottable())
                bDisplayLayer = false;

            // If the layer is not plotable
            if (m_hide_noplot_layers && !pRecord->isPlottable())
                bDisplayLayer = false;

            // If the layer is used
            if (m_hide_used_layers && IsUsedLayer(pName))
                bDisplayLayer = false;

            // If the layer is unused
            if (m_hide_unused_layers && IsUnusedLayer(pName))
                bDisplayLayer = false;

            if (bDisplayLayer)
                m_listbox.AddString(pName, sds_getrgbvalue(color.colorIndex()));
        }
        pIterator->step();
    }
}

void CDosLayerListDialog::OnDblClkList()
{
    if (!m_multiple_selection)
        OnOK();
}

void CDosLayerListDialog::OnOK()
{
    if (m_multiple_selection)
    {
        m_layers.RemoveAll();
        int	count = m_listbox.GetSelCount();
        if (count > 0)
        {
            CArray<int, int> layer_list;
            layer_list.SetSize(count);
            m_listbox.GetSelItems(count, layer_list.GetData());
            for (int i = 0; i < layer_list.GetCount(); i++)
            {
                CString str;
                m_listbox.GetText(layer_list[i], str);
                m_layers.Add(str);
            }
        }
    }
    else
    {
        int index = m_listbox.GetCurSel();
        m_listbox.GetText(index, m_layer);
    }

    CDLDialogEx::OnOK();
}

bool CDosLayerListDialog::GetLayerName(const OdDbObjectId& id, CString& strName)
{
    bool rc = false;

    OdDbLayerTableRecordPtr layerTableRecord = id.safeOpenObject();

    if (layerTableRecord.isNull())
        return rc;

    OdString name = layerTableRecord->getName();
    strName = (const TCHAR*)name;
    return (strName.IsEmpty()) ? false : true;
}

bool CDosLayerListDialog::ProcessLayers()
{
    OdApDocument* pDoc = odapDocManager()->curDocument();
    if (pDoc == nullptr)
        return false;

    OdDbLayerTablePtr pTable = pDoc->database()->getLayerTableId().safeOpenObject();
    auto layerTableIterator = pTable->newIterator();

    OdDbObjectIdArray used_layers;
    OdDbObjectIdArray unused_layers;
    OdDbObjectId id;

    for (; !layerTableIterator->done(); layerTableIterator->step())
    {
        auto id = layerTableIterator->getRecordId();
        used_layers.append(id);
        unused_layers.append(id);
    }

    pDoc->database()->purge(unused_layers);

    unsigned int i = 0;
    for (i = 0; i < unused_layers.length(); i++)
        used_layers.remove(unused_layers[i]);

    for (i = 0; i < used_layers.length(); i++)
    {
        CString strName;
        if (GetLayerName(used_layers[i], strName))
            m_used_layer_names.Add(strName);
    }
    for (i = 0; i < unused_layers.length(); i++)
    {
        CString strName;
        if (GetLayerName(unused_layers[i], strName))
            m_unused_layer_names.Add(strName);
    }
    return true;
}

bool CDosLayerListDialog::IsUsedLayer(const wchar_t* name)
{
    int i;
    for (i = 0; i < m_used_layer_names.GetCount(); i++)
    {
        CString strName = m_used_layer_names[i];
        if (strName.CompareNoCase(name) == 0)
            return true;
    }
    return false;
}

bool CDosLayerListDialog::IsUnusedLayer(const wchar_t* name)
{
    int i;
    for (i = 0; i < m_unused_layer_names.GetCount(); i++)
    {
        CString strName = m_unused_layer_names[i];
        if (strName.CompareNoCase(name) == 0)
            return true;
    }
    return false;
}

void CDosLayerListDialog::OnSelChangeList()
{
    if (m_multiple_selection)
    {
        int	count = m_listbox.GetSelCount();
        m_ok.EnableWindow(count > 0 ? TRUE : FALSE);
    }
    else
    {
        int index = m_listbox.GetCurSel();
        m_ok.EnableWindow(index >= 0 ? TRUE : FALSE);
    }
}