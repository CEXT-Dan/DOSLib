#include "stdafx.h"
#include "UnitTests.h"

#ifdef _UNITTEST
void CommandTest_dosUnitTest::execute(OdEdCommandContext* pCmdCtx)
{
    m_pCtx = pCmdCtx;
    m_pDb = m_pCtx->database();
    try
    {
        UnitTests::testResBufFail();
        UnitTests::testResBufEqual();
        UnitTests::test_abs();
        UnitTests::test_acitorgb();
        UnitTests::test_acadmem();
        UnitTests::test_acadname();
        UnitTests::test_acos();
        UnitTests::test_acosh();
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

CString UnitTests::ResBufToString( /*const*/ resbuf* src)
{
    CString fmt = _T("NULL");
    if (src == nullptr)
        return fmt;
    std::vector<CString> strings;
    for (resbuf* rbTail = src; rbTail != nullptr; rbTail = rbTail->rbnext)
    {
        switch (rbTail->restype)
        {
            case RTLB:
            {
                strings.push_back(_T("("));
                break;
            }
            case RTLE:
            {
                strings.push_back(_T(")"));
                break;
            }
            case RTDOTE:
            {
                strings.push_back(_T("."));
                break;
            }
            case RTNIL:
            {
                strings.push_back(_T("NIL"));
                break;
            }
            case RTT:
            {
                strings.push_back(_T("T"));
                break;
            }
            case RTSHORT:
            {
                fmt.Format(_T("%d"), rbTail->resval.rint);
                strings.push_back(fmt);
                break;
            }
            case RTLONG:
            {
                fmt.Format(_T("%ld"), rbTail->resval.rlong);
                strings.push_back(fmt);
                break;
            }
            case RTANG:
            case RTREAL:
            {
                fmt.Format(_T("%lf"), rbTail->resval.rreal);
                strings.push_back(fmt);
                break;
            }
            case RTLONG_PTR:
            case RTINT64:
            {
                fmt.Format(_T("%lld"), rbTail->resval.mnInt64);
                strings.push_back(fmt);
                break;
            }
            case RTPICKS:
            case RTENAME:
            {
                fmt.Format(_T("%lld"), rbTail->resval.rlname[1]);
                strings.push_back(fmt);
                break;
            }
            case RTSTR:
            {
                strings.push_back(rbTail->resval.rstring);
                break;
            }
            case RTPOINT:
            {
                fmt.Format(_T("(%lf,%lf)"), rbTail->resval.rpoint[0], rbTail->resval.rpoint[1]);
                strings.push_back(fmt);
                break;
            }
            case RTORINT:
            case RT3DPOINT:
            {
                fmt.Format(_T("(%lf,%lf,%lf)"), rbTail->resval.rpoint[0], rbTail->resval.rpoint[1], rbTail->resval.rpoint[2]);
                strings.push_back(fmt);
                break;
            }
        }
    }

    fmt.Empty();
    for (const auto& item : strings)
    {
        fmt.Append(item);
        fmt.Append(_T(" "));
    }
    fmt.Trim();
    return fmt;
}

CString UnitTests::buildFailString(const TCHAR* functionName, /*const*/ resbuf* expected, /*const*/ resbuf* result)
{
    CString fmt;
    fmt.Format(_T("\nFail %ls, expected %ls, result %ls: "),
        functionName, (const TCHAR*)ResBufToString(expected), (const TCHAR*)ResBufToString(result));
    return fmt;
}

bool UnitTests::ResBufEqual(/*const*/ resbuf* expected,/* const*/ resbuf* result)
{
    size_t rdlSize = 0;
    for (resbuf* rblTail = expected; rblTail != nullptr; rblTail = rblTail->rbnext)
        rdlSize++;
    size_t rdrSize = 0;
    for (resbuf* rbrTail = result; rbrTail != nullptr; rbrTail = rbrTail->rbnext)
        rdrSize++;
    if (rdlSize != rdrSize)
        return false;
    {
        resbuf* rblTail = expected;
        resbuf* rbrTail = result;
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

bool UnitTests::testNull(const TCHAR* functionName, resbuf* result)
{
    bool flag = result != nullptr;
    if (flag)
        sds_printf(_T("\nPass %ls: "), functionName);
    else
        sds_printf(_T("\nFail %ls, result was NULL: "), functionName);
    return flag;
}

bool UnitTests::testEqual(const TCHAR* functionName,/*const*/ resbuf* result, /*const*/ resbuf* expected)
{
    bool flag = ResBufEqual(result, expected);
    if (flag)
        sds_printf(_T("\nPass %ls: "), functionName);
    else
        sds_printf(buildFailString(functionName, expected, result));
    return flag;
}

bool UnitTests::testResBufFail()
{
    static constexpr ads_point testPoint = { 0,0,0 };
    AcResBufPtr expected(sds_buildlist(RTLB, RT3DPOINT, testPoint, RTSTR, _T("This is a test"), RTLE, RTNONE));
    AcResBufPtr result(sds_buildlist(RTLB, RT3DPOINT, testPoint, RTSTR, _T("This is not a test"), RTLE, RTNONE));

    bool flag = ResBufEqual(expected.get(), result.get());
    if (flag)
        sds_printf(_T("\nPass %ls: "), __FUNCTIONW__);
    else
        sds_printf(buildFailString(__FUNCTIONW__, expected.get(), result.get()));
    return flag;
}

bool UnitTests::testResBufEqual()
{
    static constexpr ads_point testPoint = { 0,0,0 };
    AcResBufPtr expected(sds_buildlist(RTLB, RT3DPOINT, testPoint, RTSTR, _T("This is a test"), RTLE, RTNONE));
    AcResBufPtr result(sds_buildlist(RTLB, RT3DPOINT, testPoint, RTSTR, _T("This is a test"), RTLE, RTNONE));

    bool flag = ResBufEqual(expected.get(), result.get());
    if (flag)
        sds_printf(_T("\nPass %ls: "), __FUNCTIONW__);
    else
        sds_printf(buildFailString(__FUNCTIONW__, expected.get(), result.get()));
    return flag;
}

bool UnitTests::test_abs()
{
    AcResBufPtr parg(sds_buildlist(RTSTR, _T("dos_abs"), RTREAL, -3.14, RTNONE));
    AcResBufPtr result = safeInvoke(parg.get());
    AcResBufPtr expected(sds_buildlist(RTREAL, 3.14, RTNONE));
    return testEqual(__FUNCTIONW__, result.get(), expected.get());
}

bool UnitTests::test_absolutepath()
{
    sds_printf(_T("\nFunction, %ls, Not implemented yet"), __FUNCTIONW__);
    return false;
}

bool UnitTests::test_acitorgb()
{
    AcResBufPtr parg(sds_buildlist(RTSTR, _T("dos_acitorgb"), RTSHORT, 128, RTNONE));
    AcResBufPtr result = safeInvoke(parg.get());
    AcResBufPtr expected(sds_buildlist(RTSHORT, 0, RTSHORT, 76, RTSHORT, 57, RTNONE));
    return testEqual(__FUNCTIONW__, result.get(), expected.get());
}

bool UnitTests::test_acadmem()
{
    AcResBufPtr parg(sds_buildlist(RTSTR, _T("dos_acadmem"), RTNONE));
    AcResBufPtr result = safeInvoke(parg.get());
    return UnitTests::testNull(__FUNCTIONW__, result.get());
}

bool UnitTests::test_acadname()
{
    AcResBufPtr parg(sds_buildlist(RTSTR, _T("dos_acadname"), RTNONE));
    AcResBufPtr result = safeInvoke(parg.get());
    return UnitTests::testNull(__FUNCTIONW__, result.get());
}

bool UnitTests::test_acos()
{
    AcResBufPtr parg(sds_buildlist(RTSTR, _T("dos_acos"), RTREAL, 0.0, RTNONE));
    AcResBufPtr result = safeInvoke(parg.get());
    AcResBufPtr expected(sds_buildlist(RTREAL, 1.5707963267948966, RTNONE));
    return testEqual(__FUNCTIONW__, result.get(), expected.get());
}

bool UnitTests::test_acosh()
{
    AcResBufPtr parg(sds_buildlist(RTSTR, _T("dos_acosh"), RTSHORT, 10, RTNONE));
    AcResBufPtr result = safeInvoke(parg.get());
    AcResBufPtr expected(sds_buildlist(RTREAL, 2.9932228461263808, RTNONE));
    return testEqual(__FUNCTIONW__, result.get(), expected.get());
}
#endif