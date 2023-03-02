#pragma once

#ifdef _UNITTEST
class CommandTest_dosUnitTest : public OdStaticRxObject < OdEdCommand >
{
    OdDbCommandContextPtr m_pCtx;
    OdDbDatabase* m_pDb;
public:
    const OdString localName() const { return globalName(); }
    const OdString groupName() const { return  OD_T("TxText"); }
    const OdString globalName() const { return OD_T("dos_unittest"); }
    virtual void execute(OdEdCommandContext* pCmdCtx);
};

class UnitTests
{
public:
    static bool RestBufEqual(/*const*/ resbuf* left, /*const*/ resbuf* right);
    static bool testRestBufEqual();
    static bool test_abs();
    static bool test_absolutepath();
};

#endif