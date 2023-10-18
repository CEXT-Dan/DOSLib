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
    static CString ResBufToString(/*const*/ resbuf* src);
    static CString buildFailString(const TCHAR* functionName, /*const*/ resbuf* expected,/* const*/ resbuf* result);
    static bool ResBufEqual(/*const*/ resbuf* expected,/* const*/ resbuf* result);
    static bool testNull(const TCHAR* functionName, resbuf* result);
    static bool testEqual(const TCHAR* functionName,/*const*/ resbuf* expected,/* const*/ resbuf* result);
    static bool testResBufFail();
    static bool testResBufEqual();
    static bool test_abs();
    static bool test_absolutepath();
    static bool test_acitorgb();
    static bool test_acadmem();
    static bool test_acadname();
    static bool test_acos();
    static bool test_acosh();
};

#endif