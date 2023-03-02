/////////////////////////////////////////////////////////////////////////////
// DOSLibApp.cpp
//
// Copyright (c) 1992-2023, Robert McNeel & Associates. All rights reserved.
// DOSLib is a trademark of Robert McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

#include "StdAfx.h"
#include "DOSLibApp.h"
#include "DosAboutDialog.h"

#define szRDS _RXST("RMA_")

EXTERN_C __declspec(dllexport) AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
    switch (msg)
    {
        case  AcRx::kInitAppMsg:
        {
            auto res = ::CoInitialize(0);
        }
        break;
        case  AcRx::kUnloadAppMsg:
        {
            auto app = CDOSLibApp::getInstance();
            if (app != nullptr)
                app->unloadAdsFuncs();
            ::CoUninitialize();
        }
        break;
        case AcRx::kLoadDwgMsg:
        {
            auto app = CDOSLibApp::getInstance();
            if (app != nullptr)
            {
                app->onLoadDwg();
                app->loadAdsFuncs();
            }
        }
        break;
        case AcRx::kUnloadDwgMsg:
        {
            auto app = CDOSLibApp::getInstance();
            if (app != nullptr)
                app->unloadAdsFuncs();
        }
        break;
        case AcRx::kInvkSubrMsg:
        {
            auto app = CDOSLibApp::getInstance();
            if (app != nullptr)
                app->execAdsFunc();
        }
    }
    return AcRx::kRetOK;
}

ODRX_DEFINE_DYNAMIC_MODULE(CDOSLibApp);

CDOSLibApp::CDOSLibApp()
{
    sModuleInstance = this;
    m_strVersion.Format(L"%d.%d.%d", m_nMajorVersion, m_nMinorVersion, m_nServiceRelease);
    srand((unsigned)time(nullptr));
}

CDOSLibApp::~CDOSLibApp()
{
    sModuleInstance = nullptr;
}

void CDOSLibApp::initApp()
{
    try
    {
#ifdef _UNITTEST
        odedRegCmds()->addCommand(&m_CommandTest_dosUnitTest);
#endif
        CDLDialogExMap::instance().load();
    }
    catch (...) {}
}

void CDOSLibApp::uninitApp()
{
    try
    {
#ifdef _UNITTEST
        odedRegCmds()->removeCmd(&m_CommandTest_dosUnitTest);
#endif
        CDLDialogExMap::instance().store();
    }
    catch (...) {}
}

void CDOSLibApp::onLoadDwg()
{
    if (!m_onLoadDwgOnce)
    {
        acutPrintf(L"\n%ls %ls Loaded.\n", AppName(), (const TCHAR*)m_strVersion);
        m_onLoadDwgOnce = true;
    }
}

const wchar_t* CDOSLibApp::AppName() const
{
    return L"DOSLib";
}

const wchar_t* CDOSLibApp::AppDescription() const
{
    return L"LISP Library for BricsCAD";
}

const wchar_t* CDOSLibApp::AppVersion() const
{
    return m_strVersion;
}

const wchar_t* CDOSLibApp::AppCopyright() const
{
    return L"Copyright (c) 1992-2023 Robert McNeel & Associates.";
}

int CDOSLibApp::MajorVersion() const
{
    return m_nMajorVersion;
}

int CDOSLibApp::MinorVersion() const
{
    return m_nMinorVersion;
}

int CDOSLibApp::ServiceRelease() const
{
    return m_nServiceRelease;
}

const wchar_t* CDOSLibApp::AppInternet() const
{
    return L"https://github.com/dalefugier/DOSLib";
}

int CDOSLibApp::execAdsFunc()
{
    int fcode = sds_getfuncode();
    if (fcode < sAdsFuncs.size())
        return sAdsFuncs[fcode].adsFunc();
    return RTERROR;
}

void CDOSLibApp::loadAdsFuncs()
{
    for (int i = 0; i < sAdsFuncs.size(); i++)
    {
        sds_defun(sAdsFuncs[i].function_name, i);
        sds_regfunc(sAdsFuncs[i].adsFunc, i);
    }
}

void CDOSLibApp::unloadAdsFuncs()
{
    for (int i = 0; i < sAdsFuncs.size(); i++)
    {
        sds_undef(sAdsFuncs[i].function_name, i);
    }
}

AdsFunctionDefs CDOSLibApp::createAdsFuncs()
{
    return AdsFunctionDefs
    {
     AdsFunctionDef{ _T("dos_chkdsk"),               ads_dos_chkdsk},
     AdsFunctionDef{ _T("dos_drive"),                ads_dos_drive},
     AdsFunctionDef{ _T("dos_drivep"),               ads_dos_drivep},
     AdsFunctionDef{ _T("dos_drives"),               ads_dos_drives},
     AdsFunctionDef{ _T("dos_drivetype"),            ads_dos_drivetype},
     AdsFunctionDef{ _T("dos_filesys"),              ads_dos_filesys},
     AdsFunctionDef{ _T("dos_format"),               ads_dos_format},
     AdsFunctionDef{ _T("dos_label"),                ads_dos_label},
     AdsFunctionDef{ _T("dos_serialno"),             ads_dos_serialno},
     AdsFunctionDef{ _T("dos_hdserialno"),           ads_dos_hdserialno},

     AdsFunctionDef{ _T("dos_absolutepath"),         ads_dos_absolutepath},
     AdsFunctionDef{ _T("dos_compactpath"),          ads_dos_compactpath},
     AdsFunctionDef{ _T("dos_fullpath"),             ads_dos_fullpath},
     AdsFunctionDef{ _T("dos_ispathrelative"),       ads_dos_ispathrelative},
     AdsFunctionDef{ _T("dos_ispathroot"),           ads_dos_ispathroot},
     AdsFunctionDef{ _T("dos_ispathsameroot"),       ads_dos_ispathsameroot},
     AdsFunctionDef{ _T("dos_ispathunc"),            ads_dos_ispathunc},
     AdsFunctionDef{ _T("dos_ispathurl"),            ads_dos_ispathurl},
     AdsFunctionDef{ _T("dos_longpath"),             ads_dos_longpath},
     AdsFunctionDef{ _T("dos_makepath"),             ads_dos_makepath},
     AdsFunctionDef{ _T("dos_path"),                 ads_dos_path},
     AdsFunctionDef{ _T("dos_pathbackslash"),        ads_dos_pathbackslash},
     AdsFunctionDef{ _T("dos_pathextension"),        ads_dos_pathextension},
     AdsFunctionDef{ _T("dos_pathquotes"),           ads_dos_pathquotes},
     AdsFunctionDef{ _T("dos_relativepath"),         ads_dos_relativepath},
     AdsFunctionDef{ _T("dos_shortpath"),            ads_dos_shortpath},
     AdsFunctionDef{ _T("dos_splitpath"),            ads_dos_splitpath},
     AdsFunctionDef{ _T("dos_uncpath"),              ads_dos_uncpath},
     AdsFunctionDef{ _T("dos_ispathnetwork"),        ads_dos_ispathnetwork},
     AdsFunctionDef{ _T("dos_ispathslow"),           ads_dos_ispathslow},
     AdsFunctionDef{ _T("dos_localpath"),            ads_dos_localpath},
     AdsFunctionDef{ _T("dos_expandenv"),            ads_dos_expandenv},
     AdsFunctionDef{ _T("dos_unexpandenv"),          ads_dos_unexpandenv},

     AdsFunctionDef{ _T("dos_chdir"),                ads_dos_chdir},
     AdsFunctionDef{ _T("dos_deltree"),              ads_dos_deltree},
     AdsFunctionDef{ _T("dos_dirattrib"),            ads_dos_dirattrib},
     AdsFunctionDef{ _T("dos_dirp"),                 ads_dos_dirp},
     AdsFunctionDef{ _T("dos_dirtree"),              ads_dos_dirtree},
     AdsFunctionDef{ _T("dos_getdir"),               ads_dos_getdir},
     AdsFunctionDef{ _T("dos_mkdir"),                ads_dos_mkdir},
     AdsFunctionDef{ _T("dos_pwdir"),                ads_dos_pwdir},
     AdsFunctionDef{ _T("dos_rendir"),               ads_dos_rendir},
     AdsFunctionDef{ _T("dos_rmdir"),                ads_dos_rmdir},
     AdsFunctionDef{ _T("dos_specialdir"),           ads_dos_specialdir},
     AdsFunctionDef{ _T("dos_subdir"),               ads_dos_subdir},
     AdsFunctionDef{ _T("dos_sysdir"),               ads_dos_sysdir},
     AdsFunctionDef{ _T("dos_tempdir"),              ads_dos_tempdir},
     AdsFunctionDef{ _T("dos_windir"),               ads_dos_windir},
     AdsFunctionDef{ _T("dos_dirsize"),              ads_dos_dirsize},

     AdsFunctionDef{ _T("dos_attrib"),               ads_dos_attrib},
     AdsFunctionDef{ _T("dos_copy"),                 ads_dos_copy},
     AdsFunctionDef{ _T("dos_delete"),               ads_dos_delete},
     AdsFunctionDef{ _T("dos_dir"),                  ads_dos_dir},
     AdsFunctionDef{ _T("dos_dos2unix"),             ads_dos_dos2unix},
     AdsFunctionDef{ _T("dos_encrypt"),              ads_dos_encrypt},
     AdsFunctionDef{ _T("dos_file"),                 ads_dos_file},
     AdsFunctionDef{ _T("dos_fileex"),               ads_dos_fileex},
     AdsFunctionDef{ _T("dos_filedate"),             ads_dos_filedate},
     AdsFunctionDef{ _T("dos_filep"),                ads_dos_filep},
     AdsFunctionDef{ _T("dos_filesize"),             ads_dos_filesize},
     AdsFunctionDef{ _T("dos_find"),                 ads_dos_find},
     AdsFunctionDef{ _T("dos_getfiled"),             ads_dos_getfiled},
     AdsFunctionDef{ _T("dos_getfilem"),             ads_dos_getfilem},
     AdsFunctionDef{ _T("dos_getfilenav"),           ads_dos_getfilenav},
     AdsFunctionDef{ _T("dos_mergefiles"),           ads_dos_mergefiles},
     AdsFunctionDef{ _T("dos_move"),                 ads_dos_move},
     AdsFunctionDef{ _T("dos_openp"),                ads_dos_openp},
     AdsFunctionDef{ _T("dos_recent"),               ads_dos_recent},
     AdsFunctionDef{ _T("dos_recycle"),              ads_dos_recycle},
     AdsFunctionDef{ _T("dos_rename"),               ads_dos_rename},
     AdsFunctionDef{ _T("dos_search"),               ads_dos_search},
     AdsFunctionDef{ _T("dos_tempfile"),             ads_dos_tempfile},
     AdsFunctionDef{ _T("dos_touch"),                ads_dos_touch},
     AdsFunctionDef{ _T("dos_unix2dos"),             ads_dos_unix2dos},
     AdsFunctionDef{ _T("dos_fileinfo"),             ads_dos_fileinfo},
     AdsFunctionDef{ _T("dos_emptyrecycle"),         ads_dos_emptyrecycle},
     AdsFunctionDef{ _T("dos_filecrc"),              ads_dos_filecrc},
     AdsFunctionDef{ _T("dos_fileowner"),            ads_dos_fileowner},
     AdsFunctionDef{ _T("dos_readtextfile"),         ads_dos_readtextfile},
     AdsFunctionDef{ _T("dos_writetextfile"),        ads_dos_writetextfile},
     AdsFunctionDef{ _T("dos_readdelimitedfile"),    ads_dos_readdelimitedfile},
     AdsFunctionDef{ _T("dos_isfilename"),           ads_dos_isfilename},
     AdsFunctionDef{ _T("dos_getprn"),               ads_dos_getprn},
     AdsFunctionDef{ _T("dos_printers"),             ads_dos_printers},
     AdsFunctionDef{ _T("dos_setprn"),               ads_dos_setprn},
     AdsFunctionDef{ _T("dos_spool"),                ads_dos_spool},

     AdsFunctionDef{ _T("dos_getini"),               ads_dos_getini},
     AdsFunctionDef{ _T("dos_regaddkey"),            ads_dos_regaddkey},
     AdsFunctionDef{ _T("dos_regdelkey"),            ads_dos_regdelkey},
     AdsFunctionDef{ _T("dos_regdelval"),            ads_dos_regdelval},
     AdsFunctionDef{ _T("dos_regenumkeys"),          ads_dos_regenumkeys},
     AdsFunctionDef{ _T("dos_regenumnames"),         ads_dos_regenumnames},
     AdsFunctionDef{ _T("dos_reggetint"),            ads_dos_reggetint},
     AdsFunctionDef{ _T("dos_reggetstr"),            ads_dos_reggetstr},
     AdsFunctionDef{ _T("dos_reggettype"),           ads_dos_reggettype},
     AdsFunctionDef{ _T("dos_regsetint"),            ads_dos_regsetint},
     AdsFunctionDef{ _T("dos_regsetstr"),            ads_dos_regsetstr},
     AdsFunctionDef{ _T("dos_setini"),               ads_dos_setini},

     AdsFunctionDef{ _T("dos_regdel"),               ads_dos_regdel},
     AdsFunctionDef{ _T("dos_regget"),               ads_dos_regget},
     AdsFunctionDef{ _T("dos_regkey"),               ads_dos_regkey},
     AdsFunctionDef{ _T("dos_regset"),               ads_dos_regset},

     AdsFunctionDef{ _T("dos_command"),              ads_dos_command},
     AdsFunctionDef{ _T("dos_execute"),              ads_dos_execute},
     AdsFunctionDef{ _T("dos_exewait"),              ads_dos_exewait},
     AdsFunctionDef{ _T("dos_processes"),            ads_dos_processes},
     AdsFunctionDef{ _T("dos_shellexe"),             ads_dos_shellexe},

     AdsFunctionDef{ _T("dos_checklist"),            ads_dos_checklist},
     AdsFunctionDef{ _T("dos_combolist"),            ads_dos_combolist},
     AdsFunctionDef{ _T("dos_editbox"),              ads_dos_editbox},
     AdsFunctionDef{ _T("dos_editlist"),             ads_dos_editlist},
     AdsFunctionDef{ _T("dos_getcancel"),            ads_dos_getcancel},
     AdsFunctionDef{ _T("dos_getcolor"),             ads_dos_getcolor},
     AdsFunctionDef{ _T("dos_getint"),               ads_dos_getint},
     AdsFunctionDef{ _T("dos_getpassword"),          ads_dos_getpassword},
     AdsFunctionDef{ _T("dos_getprogress"),          ads_dos_getprogress},
     AdsFunctionDef{ _T("dos_getreal"),              ads_dos_getreal},
     AdsFunctionDef{ _T("dos_getstring"),            ads_dos_getstring},
     AdsFunctionDef{ _T("dos_htmlbox"),              ads_dos_htmlbox},
     AdsFunctionDef{ _T("dos_htmlboxex"),            ads_dos_htmlboxex},
     AdsFunctionDef{ _T("dos_listbox"),              ads_dos_listbox},
     AdsFunctionDef{ _T("dos_msgbox"),               ads_dos_msgbox},
     AdsFunctionDef{ _T("dos_msgboxex"),             ads_dos_msgboxex},
     AdsFunctionDef{ _T("dos_multilist"),            ads_dos_multilist},
     AdsFunctionDef{ _T("dos_popupmenu"),            ads_dos_popupmenu},
     AdsFunctionDef{ _T("dos_proplist"),             ads_dos_proplist},
     AdsFunctionDef{ _T("dos_splash"),               ads_dos_splash},
     AdsFunctionDef{ _T("dos_waitcursor"),           ads_dos_waitcursor},
     AdsFunctionDef{ _T("dos_linetypebox"),          ads_dos_linetypebox},
     AdsFunctionDef{ _T("dos_traywnd"),              ads_dos_traywnd},
     AdsFunctionDef{ _T("dos_msgboxchk"),            ads_dos_msgboxchk},
     AdsFunctionDef{ _T("dos_getdate"),              ads_dos_getdate},
     AdsFunctionDef{ _T("dos_orderlist"),            ads_dos_orderlist},
     AdsFunctionDef{ _T("dos_duallist"),             ads_dos_duallist},
     AdsFunctionDef{ _T("dos_htmldialog"),           ads_dos_htmldialog},

     AdsFunctionDef{ _T("dos_strcase"),              ads_dos_strcase},
     AdsFunctionDef{ _T("dos_strcat"),               ads_dos_strcat},
     AdsFunctionDef{ _T("dos_strchar"),              ads_dos_strchar},
     AdsFunctionDef{ _T("dos_strcompare"),           ads_dos_strcompare},
     AdsFunctionDef{ _T("dos_strdelete"),            ads_dos_strdelete},
     AdsFunctionDef{ _T("dos_strexcluding"),         ads_dos_strexcluding},
     AdsFunctionDef{ _T("dos_strfind"),              ads_dos_strfind},
     AdsFunctionDef{ _T("dos_strfindoneof"),         ads_dos_strfindoneof},
     AdsFunctionDef{ _T("dos_strincluding"),         ads_dos_strincluding},
     AdsFunctionDef{ _T("dos_strinsert"),            ads_dos_strinsert},
     AdsFunctionDef{ _T("dos_strischar"),            ads_dos_strischar},
     AdsFunctionDef{ _T("dos_strleft"),              ads_dos_strleft},
     AdsFunctionDef{ _T("dos_strlength"),            ads_dos_strlength},
     AdsFunctionDef{ _T("dos_strmatch"),             ads_dos_strmatch},
     AdsFunctionDef{ _T("dos_strmid"),               ads_dos_strmid},
     AdsFunctionDef{ _T("dos_strremove"),            ads_dos_strremove},
     AdsFunctionDef{ _T("dos_strreplace"),           ads_dos_strreplace},
     AdsFunctionDef{ _T("dos_strreverse"),           ads_dos_strreverse},
     AdsFunctionDef{ _T("dos_strreversefind"),       ads_dos_strreversefind},
     AdsFunctionDef{ _T("dos_strright"),             ads_dos_strright},
     AdsFunctionDef{ _T("dos_strtokens"),            ads_dos_strtokens},
     AdsFunctionDef{ _T("dos_strtrim"),              ads_dos_strtrim},
     AdsFunctionDef{ _T("dos_strtrimleft"),          ads_dos_strtrimleft},
     AdsFunctionDef{ _T("dos_strtrimright"),         ads_dos_strtrimright},
     AdsFunctionDef{ _T("dos_strformat"),            ads_dos_strformat},
     AdsFunctionDef{ _T("dos_strformatnumber"),      ads_dos_strformatnumber},
     AdsFunctionDef{ _T("dos_strformatcurrency"),    ads_dos_strformatcurrency},
     AdsFunctionDef{ _T("dos_strformatdate"),        ads_dos_strformatdate},
     AdsFunctionDef{ _T("dos_strsort"),              ads_dos_strsort},
     AdsFunctionDef{ _T("dos_strcmplogical"),        ads_dos_strcmplogical},
     AdsFunctionDef{ _T("dos_strcull"),              ads_dos_strcull},
     AdsFunctionDef{ _T("dos_strregexp"),            ads_dos_strregexp},

     AdsFunctionDef{ _T("dos_acadmem"),              ads_dos_acadmem},
     AdsFunctionDef{ _T("dos_acitorgb"),             ads_dos_acitorgb},
     AdsFunctionDef{ _T("dos_arxlist"),              ads_dos_arxlist},
     AdsFunctionDef{ _T("dos_closeall"),             ads_dos_closeall},
     AdsFunctionDef{ _T("dos_cmdline"),              ads_dos_cmdline},
     AdsFunctionDef{ _T("dos_curvearea"),            ads_dos_curvearea},
     AdsFunctionDef{ _T("dos_dwgpreview"),           ads_dos_dwgpreview},
     AdsFunctionDef{ _T("dos_dwgver"),               ads_dos_dwgver},
     AdsFunctionDef{ _T("dos_extractpreview"),       ads_dos_extractpreview},
     AdsFunctionDef{ _T("dos_isbreak"),              ads_dos_isbreak},
     AdsFunctionDef{ _T("dos_istextscr"),            ads_dos_istextscr},
     AdsFunctionDef{ _T("dos_isvlide"),              ads_dos_isvlide},
     AdsFunctionDef{ _T("dos_layerlistbox"),         ads_dos_layerlistbox},
     AdsFunctionDef{ _T("dos_lisplist"),             ads_dos_lisplist},
     AdsFunctionDef{ _T("dos_massprops"),            ads_dos_massprops},
     AdsFunctionDef{ _T("dos_progbar"),              ads_dos_progbar},
     AdsFunctionDef{ _T("dos_rgbtoaci"),             ads_dos_rgbtoaci},
     AdsFunctionDef{ _T("dos_saveall"),              ads_dos_saveall},
     AdsFunctionDef{ _T("dos_show"),                 ads_dos_show},
     AdsFunctionDef{ _T("dos_xreflist"),             ads_dos_xreflist},
     AdsFunctionDef{ _T("dos_hideobjects"),          ads_dos_hideobjects},
     AdsFunctionDef{ _T("dos_showobjects"),          ads_dos_showobjects},
     AdsFunctionDef{ _T("dos_rgbtogray"),            ads_dos_rgbtogray},
     AdsFunctionDef{ _T("dos_rgbtohls"),             ads_dos_rgbtohls},
     AdsFunctionDef{ _T("dos_hlstorgb"),             ads_dos_hlstorgb},
     AdsFunctionDef{ _T("dos_isinsidecurve"),        ads_dos_isinsidecurve},
     AdsFunctionDef{ _T("dos_plinecentroid"),        ads_dos_plinecentroid},
     AdsFunctionDef{ _T("dos_hatcharea"),            ads_dos_hatcharea},
     AdsFunctionDef{ _T("dos_cmdargs"),              ads_dos_cmdargs},
     AdsFunctionDef{ _T("dos_getsecret"),            ads_dos_getsecret},
     AdsFunctionDef{ _T("dos_cleardbmod"),           ads_dos_cleardbmod},
     AdsFunctionDef{ _T("dos_summaryinfo"),          ads_dos_summaryinfo},
     AdsFunctionDef{ _T("dos_custominfo"),           ads_dos_custominfo},
     AdsFunctionDef{ _T("dos_olelist"),              ads_dos_olelist},
     AdsFunctionDef{ _T("dos_imagelist"),            ads_dos_imagelist},
     AdsFunctionDef{ _T("dos_purgexdata"),           ads_dos_purgexdata},
     AdsFunctionDef{ _T("dos_proxycount"),           ads_dos_proxycount},
     AdsFunctionDef{ _T("dos_proxyclean"),           ads_dos_proxyclean},
     AdsFunctionDef{ _T("dos_plinewinding"),         ads_dos_plinewinding},
     AdsFunctionDef{ _T("dos_acadname"),             ads_dos_acadname},
     AdsFunctionDef{ _T("dos_slblist"),              ads_dos_slblist},

     AdsFunctionDef{ _T("dos_about"),                ads_dos_about},
     AdsFunctionDef{ _T("dos_version"),              ads_dos_version},
#ifndef _TXAPP
     AdsFunctionDef{ _T("dos_demandload"),           ads_dos_demandload},
     AdsFunctionDef{ _T("dos_help"),                 ads_dos_help},
#endif

     AdsFunctionDef{ _T("dos_adminp"),               ads_dos_adminp},
     AdsFunctionDef{ _T("dos_beep"),                 ads_dos_beep},
     AdsFunctionDef{ _T("dos_capslock"),             ads_dos_capslock},
     AdsFunctionDef{ _T("dos_cdate"),                ads_dos_cdate},
     AdsFunctionDef{ _T("dos_clipboard"),            ads_dos_clipboard},
     AdsFunctionDef{ _T("dos_computer"),             ads_dos_computer},
     AdsFunctionDef{ _T("dos_date"),                 ads_dos_date},
     AdsFunctionDef{ _T("dos_guidgen"),              ads_dos_guidgen},
     AdsFunctionDef{ _T("dos_hostname"),             ads_dos_hostname},
     AdsFunctionDef{ _T("dos_htmlhelp"),             ads_dos_htmlhelp},
     AdsFunctionDef{ _T("dos_ispoweruser"),          ads_dos_ispoweruser},
     AdsFunctionDef{ _T("dos_ipaddress"),            ads_dos_ipaddress},
     AdsFunctionDef{ _T("dos_macaddress"),           ads_dos_macaddress},
     AdsFunctionDef{ _T("dos_mem"),                  ads_dos_mem},
     AdsFunctionDef{ _T("dos_numlock"),              ads_dos_numlock},
     AdsFunctionDef{ _T("dos_pause"),                ads_dos_pause},
     AdsFunctionDef{ _T("dos_random"),               ads_dos_random},
     AdsFunctionDef{ _T("dos_scrolllock"),           ads_dos_scrolllock},
     AdsFunctionDef{ _T("dos_sortlist"),             ads_dos_sortlist},
     AdsFunctionDef{ _T("dos_speaker"),              ads_dos_speaker},
     AdsFunctionDef{ _T("dos_time"),                 ads_dos_time},
     AdsFunctionDef{ _T("dos_username"),             ads_dos_username},
     AdsFunctionDef{ _T("dos_ver"),                  ads_dos_ver},
     AdsFunctionDef{ _T("dos_wav"),                  ads_dos_wav},
     AdsFunctionDef{ _T("dos_winhelp"),              ads_dos_winhelp},
     AdsFunctionDef{ _T("dos_iswin64"),              ads_dos_iswin64},
     AdsFunctionDef{ _T("dos_isacad64"),             ads_dos_isacad64},
     AdsFunctionDef{ _T("dos_tickcount"),            ads_dos_tickcount},
     AdsFunctionDef{ _T("dos_nicinfo"),              ads_dos_nicinfo},
     AdsFunctionDef{ _T("dos_printscrn"),            ads_dos_printscrn},
     AdsFunctionDef{ _T("dos_asynckeystate"),        ads_dos_asynckeystate},
     AdsFunctionDef{ _T("dos_systemmetrics"),        ads_dos_systemmetrics},

     AdsFunctionDef{ _T("dos_abs"),                  ads_dos_abs},
     AdsFunctionDef{ _T("dos_acos"),                 ads_dos_acos},
     AdsFunctionDef{ _T("dos_acosh"),                ads_dos_acosh},
     AdsFunctionDef{ _T("dos_asin"),                 ads_dos_asin},
     AdsFunctionDef{ _T("dos_asinh"),                ads_dos_asinh},
     AdsFunctionDef{ _T("dos_atan"),                 ads_dos_atan},
     AdsFunctionDef{ _T("dos_atan2"),                ads_dos_atan2},
     AdsFunctionDef{ _T("dos_atanh"),                ads_dos_atanh},
     AdsFunctionDef{ _T("dos_ceil"),                 ads_dos_ceil},
     AdsFunctionDef{ _T("dos_clamp"),                ads_dos_clamp},
     AdsFunctionDef{ _T("dos_cos"),                  ads_dos_cos},
     AdsFunctionDef{ _T("dos_cosh"),                 ads_dos_cosh},
     AdsFunctionDef{ _T("dos_div"),                  ads_dos_div},
     AdsFunctionDef{ _T("dos_dtr"),                  ads_dos_dtr},
     AdsFunctionDef{ _T("dos_e"),                    ads_dos_e},
     AdsFunctionDef{ _T("dos_exp"),                  ads_dos_exp},
     AdsFunctionDef{ _T("dos_floor"),                ads_dos_floor},
     AdsFunctionDef{ _T("dos_fmod"),                 ads_dos_fmod},
     AdsFunctionDef{ _T("dos_hypot"),                ads_dos_hypot},
     AdsFunctionDef{ _T("dos_interp"),               ads_dos_interp},
     AdsFunctionDef{ _T("dos_log"),                  ads_dos_log},
     AdsFunctionDef{ _T("dos_log10"),                ads_dos_log10},
     AdsFunctionDef{ _T("dos_log2"),                 ads_dos_log2},
     AdsFunctionDef{ _T("dos_max"),                  ads_dos_max},
     AdsFunctionDef{ _T("dos_mean"),                 ads_dos_mean},
     AdsFunctionDef{ _T("dos_median"),               ads_dos_median},
     AdsFunctionDef{ _T("dos_min"),                  ads_dos_min},
     AdsFunctionDef{ _T("dos_mode"),                 ads_dos_mode},
     AdsFunctionDef{ _T("dos_modf"),                 ads_dos_modf},
     AdsFunctionDef{ _T("dos_moment"),               ads_dos_moment},
     AdsFunctionDef{ _T("dos_normalize"),            ads_dos_normalize},
     AdsFunctionDef{ _T("dos_parameterize"),         ads_dos_parameterize},
     AdsFunctionDef{ _T("dos_pi"),                   ads_dos_pi},
     AdsFunctionDef{ _T("dos_pow"),                  ads_dos_pow},
     AdsFunctionDef{ _T("dos_range"),                ads_dos_range},
     AdsFunctionDef{ _T("dos_round"),                ads_dos_round},
     AdsFunctionDef{ _T("dos_rtd"),                  ads_dos_rtd},
     AdsFunctionDef{ _T("dos_sin"),                  ads_dos_sin},
     AdsFunctionDef{ _T("dos_sinh"),                 ads_dos_sinh},
     AdsFunctionDef{ _T("dos_sqrt"),                 ads_dos_sqrt},
     AdsFunctionDef{ _T("dos_sum"),                  ads_dos_sum},
     AdsFunctionDef{ _T("dos_difference"),           ads_dos_difference},
     AdsFunctionDef{ _T("dos_product"),              ads_dos_product},
     AdsFunctionDef{ _T("dos_quotient"),             ads_dos_quotient},
     AdsFunctionDef{ _T("dos_tan"),                  ads_dos_tan},
     AdsFunctionDef{ _T("dos_tanh"),                 ads_dos_tanh},
     AdsFunctionDef{ _T("dos_trunc"),                ads_dos_trunc},
     AdsFunctionDef{ _T("dos_gcd"),                  ads_dos_gcd},
     AdsFunctionDef{ _T("dos_lcm"),                  ads_dos_lcm},
     AdsFunctionDef{ _T("dos_fact"),                 ads_dos_fact},
     AdsFunctionDef{ _T("dos_equal"),                ads_dos_equal},
     AdsFunctionDef{ _T("dos_sign"),                 ads_dos_sign},
     AdsFunctionDef{ _T("dos_chgsign"),              ads_dos_chgsign},
     AdsFunctionDef{ _T("dos_copysign"),             ads_dos_copysign},
     AdsFunctionDef{ _T("dos_sortnumbers"),          ads_dos_sortnumbers},
     AdsFunctionDef{ _T("dos_cullnumbers"),          ads_dos_cullnumbers},
     AdsFunctionDef{ _T("dos_permute"),              ads_dos_permute},
     AdsFunctionDef{ _T("dos_vector"),               ads_dos_vector},
     AdsFunctionDef{ _T("dos_unitize"),              ads_dos_unitize},
     AdsFunctionDef{ _T("dos_length"),               ads_dos_length},
     AdsFunctionDef{ _T("dos_negate"),               ads_dos_negate},
     AdsFunctionDef{ _T("dos_crossproduct"),         ads_dos_crossproduct},
     AdsFunctionDef{ _T("dos_dotproduct"),           ads_dos_dotproduct},
     AdsFunctionDef{ _T("dos_wedgeproduct"),         ads_dos_wedgeproduct},
     AdsFunctionDef{ _T("dos_tripleproduct"),        ads_dos_tripleproduct},
     AdsFunctionDef{ _T("dos_perpendicularto"),      ads_dos_perpendicularto},
     AdsFunctionDef{ _T("dos_zerop"),                ads_dos_zerop},
     AdsFunctionDef{ _T("dos_tinyp"),                ads_dos_tinyp},
     AdsFunctionDef{ _T("dos_unitp"),                ads_dos_unitp},
     AdsFunctionDef{ _T("dos_parallelp"),            ads_dos_parallelp},
     AdsFunctionDef{ _T("dos_perpendicularp"),       ads_dos_perpendicularp},
     AdsFunctionDef{ _T("dos_orthogonalp"),          ads_dos_orthogonalp},
     AdsFunctionDef{ _T("dos_orthonormalp"),         ads_dos_orthonormalp},
     AdsFunctionDef{ _T("dos_righthandp"),           ads_dos_righthandp},
     AdsFunctionDef{ _T("dos_anglebetween"),         ads_dos_anglebetween},
     AdsFunctionDef{ _T("dos_scale"),                ads_dos_scale},
     AdsFunctionDef{ _T("dos_quadratic"),            ads_dos_quadratic},
     AdsFunctionDef{ _T("dos_phi"),                  ads_dos_phi},
     AdsFunctionDef{ _T("dos_cbrt"),                 ads_dos_cbrt},
     AdsFunctionDef{ _T("dos_isprime"),              ads_dos_isprime},
    };
}

CDOSLibApp* CDOSLibApp::getInstance()
{
    return sModuleInstance;
}
//
int CDOSLibApp::ads_dos_about()
{
    CAdsArgs args;
    CAcModuleResourceOverride myResources;
    CDosAboutDialog dlg(CWnd::FromHandle(adsw_acadMainWnd()));
    dlg.DoModal();
    acedRetNil();
    return RSRSLT;
}

int CDOSLibApp::ads_dos_version()
{
    CAdsRetList result;
    result.Add(getInstance()->MajorVersion());
    result.Add(getInstance()->MinorVersion());
    result.Add(getInstance()->ServiceRelease());
    acedRetList(result);
    return RSRSLT;
}