#include "stdafx.h"
#include "DosSettings.h"

IMPLEMENT_SERIAL(CDLDialogExMap, CObject, 1)

void CDLDialogExMap::Serialize(CArchive& ar)
{
    try
    {
        CObject::Serialize(ar);
        if (ar.IsStoring())
        {
            ar << VER;
            ar << crectMap.size();
            for (const auto& item : crectMap)
            {
                ar << item.first;//dialog name
                ar << item.second.size();
                for (const auto& subitem : item.second)
                {
                    ar << subitem.first; //control id
                    ar << subitem.second; //crect
                }
            }
        }
        else
        {
            ar >> readVer;
            if (readVer != VER)
                return;
            size_t mapSize = 0;
            ar >> mapSize;
            for (size_t i = 0; i < mapSize; i++)
            {
                CString dialogName;
                size_t subMapSize = 0;
                ar >> dialogName;
                ar >> subMapSize;

                std::map<CTRLID, CRect> subMap;
                for (size_t si = 0; si < subMapSize; si++)
                {
                    int id = 0;
                    CRect rc;
                    ar >> id;
                    ar >> rc;
                    subMap.emplace(id, rc);
                }
                crectMap.emplace(dialogName, std::move(subMap));
            }
        }
    }
    catch (...)
    {
        acutPrintf(_T("\nDosLib::load CDLDialogExMap error:"));
        isOk = false;
    }
    isOk = true;
}

CDLDialogExMap& CDLDialogExMap::instance()
{
    static CDLDialogExMap mthis;
    return mthis;
}

bool CDLDialogExMap::load()
{
    if (!std::filesystem::exists((const TCHAR*)storePath()))
        return false;
    CFile fileL;
    CFileException e;
    const BOOL isOpen = fileL.Open(storePath(), CFile::modeRead, &e);
    if (isOpen != TRUE)
    {
        TCHAR szError[256];
        e.GetErrorMessage(szError, 256);
        acutPrintf(_T("\nDosLib::load error %ls:"), szError);
        return false;
    }
    CArchive arLoad(&fileL, CArchive::load);
    this->Serialize(arLoad);
    arLoad.Close();
    fileL.Close();
    return true;
}

bool CDLDialogExMap::store()
{
    CFile fileS;
    CFileException e;
    const BOOL isOpen = fileS.Open(storePath(), CFile::modeWrite | CFile::modeCreate, &e);
    if (isOpen != TRUE)
    {
        TCHAR szError[256];
        e.GetErrorMessage(szError, 256);
        acutPrintf(_T("\nDosLib::store error %ls:"), szError);
        return false;
    }
    CArchive arStore(&fileS, CArchive::store);
    this->Serialize(arStore);
    arStore.Close();
    fileS.Close();
    return true;
}

bool CDLDialogExMap::clear()
{
    crectMap.clear();
    return true;
}

CString CDLDialogExMap::storePath()
{
    CString sVal;
    try
    {
        const std::wstring _appdata = _wgetenv(_T("APPDATA"));
        auto path = std::filesystem::path{ _appdata } / _T("DosLib");
        if (!std::filesystem::exists(path))
            std::filesystem::create_directory(path);
        path /= _T("DialogData.dat");
        sVal = path.c_str();
    }
    catch (...)
    {
        acutPrintf(_T("\nCould not write to appdata"));
    }
    return sVal;
}