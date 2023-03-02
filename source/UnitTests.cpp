#include "stdafx.h"
#include "UnitTests.h"

#ifdef _UNITTEST
void CommandTest_dosUnitTest::execute(OdEdCommandContext* pCmdCtx)
{
    m_pCtx = pCmdCtx;
    m_pDb = m_pCtx->database();
    try
    {
        UnitTests::testRestBufEqual();
        UnitTests::test_abs();
    }
    catch (const OdError& e)
    {
        m_pCtx->dbUserIO()->putString(e.description());
    }
}

AcResBufPtr safeInvoke(const resbuf* args)
{
    resbuf* res = nullptr;
    sds_invoke(args, &res);
    return AcResBufPtr(res);
}

bool UnitTests::RestBufEqual(/*const*/ resbuf* left,/* const*/ resbuf* right)
{
    size_t rdlSize = 0;
    for (resbuf* rblTail = left; rblTail != nullptr; rblTail = rblTail->rbnext)
        rdlSize++;
    size_t rdrSize = 0;
    for (resbuf* rbrTail = right; rbrTail != nullptr; rbrTail = rbrTail->rbnext)
        rdrSize++;
    if (rdlSize != rdrSize)
        return false;
    {
        resbuf* rblTail = left;
        resbuf* rbrTail = right;
        for (; rblTail != nullptr && rbrTail != nullptr; rblTail = rblTail->rbnext, rbrTail = rbrTail->rbnext)
        {
            if (rblTail->restype != rbrTail->restype)
                return false;
            switch (rblTail->restype)
            {
                case RTSTR:
                {
                    if (_wcsicmp(rblTail->resval.rstring, rbrTail->resval.rstring) != 0)
                        return false;
                }
                break;
                default:
                {
                    static constexpr size_t sz = sizeof(resbuf::resval);
                    if (memcmp(&rblTail->resval, &rbrTail->resval, sizeof(sz)) != 0)
                        return false;
                }
                break;
            }
        }
    }
    return true;
}

bool UnitTests::testRestBufEqual()
{
    static constexpr ads_point testPoint = { 0,0,0 };
    AcResBufPtr pLeft(sds_buildlist(RTLB, RT3DPOINT, testPoint, RTSTR, _T("This is a test"), RTLE));
    AcResBufPtr pRight(sds_buildlist(RTLB, RT3DPOINT, testPoint, RTSTR, _T("This is a test"), RTLE));

    bool flag = RestBufEqual(pLeft.get(), pRight.get());
    if (flag)
        sds_printf(_T("\nFunction, %ls, Pass"), __FUNCTIONW__);
    else
        sds_printf(_T("\nFunction, %ls, Fail"), __FUNCTIONW__);
    return flag;
}

bool UnitTests::test_abs()
{
    AcResBufPtr parg(sds_buildlist(RTSTR, _T("dos_abs"), RTREAL, -3.14));
    AcResBufPtr result = safeInvoke(parg.get());
    AcResBufPtr expected(sds_buildlist(RTREAL, 3.14));

    bool flag = RestBufEqual(result.get(), expected.get());
    if (flag)
        sds_printf(_T("\nFunction, %ls, Pass"), __FUNCTIONW__);
    else
        sds_printf(_T("\nFunction, %ls, Fail"), __FUNCTIONW__);
    return flag;

}

bool UnitTests::test_absolutepath()
{
    sds_printf(_T("\nFunction, %ls, Not implemented yet"), __FUNCTIONW__);
    return false;
}
#endif