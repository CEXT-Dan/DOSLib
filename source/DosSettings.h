#pragma once

typedef int32_t CTRLID;

class CDLDialogExMap : public CObject
{
public:
    CDLDialogExMap() = default;
    virtual ~CDLDialogExMap() override = default;
    virtual void Serialize(CArchive& ar) override;

    bool load();
    bool store();
    bool clear();

    static CString storePath();
    static CDLDialogExMap& instance();

public:
    std::map<CString, std::map<CTRLID, CRect>> crectMap;

protected:
    DECLARE_SERIAL(CDLDialogExMap)

protected:
    static const int VER = 1;
    int readVer = 0;
    bool isOk = false;
};
