#pragma once

class CAcExtensionModule {
public:
    CAcExtensionModule();
    ~CAcExtensionModule();

protected:
    bool    m_bAttached;
    HINSTANCE m_hDefaultResource;
    HINSTANCE m_hModuleResource;
    AFX_EXTENSION_MODULE m_module;

public:
    bool    Attached() const;
    HINSTANCE DefaultResourceInstance() const;
    HINSTANCE ModuleResourceInstance() const;
    bool    AttachInstance(HINSTANCE hExtInst, HINSTANCE hResInst);
    bool    AttachInstance(HINSTANCE hExtInst)
    {
        return this->AttachInstance(hExtInst, hExtInst);
    }
    void    DetachInstance();
};

inline CAcExtensionModule::CAcExtensionModule() :
    m_bAttached(false),
    m_hDefaultResource(NULL),
    m_hModuleResource(NULL)
{
    m_module.bInitialized = FALSE;
    m_module.hModule = NULL;
    m_module.hResource = NULL;
    m_module.pFirstSharedClass = NULL;
    m_module.pFirstSharedFactory = NULL;
}

inline CAcExtensionModule::~CAcExtensionModule()
{
}

inline bool CAcExtensionModule::Attached() const
{
    return m_bAttached;
}

inline bool CAcExtensionModule::AttachInstance(HINSTANCE hExtInst, HINSTANCE hResInst)
{
    if (m_bAttached)
        return false;
    m_bAttached = AfxInitExtensionModule(m_module, hExtInst) != 0;
    if (m_bAttached) {
        this->m_hDefaultResource = ::AfxGetResourceHandle();
        this->m_hModuleResource = hResInst;
        this->m_module.hResource = hResInst;
        new CDynLinkLibrary(m_module);
    }
    return m_bAttached;
}

inline HINSTANCE CAcExtensionModule::DefaultResourceInstance() const
{
    return m_hDefaultResource;
}

inline void CAcExtensionModule::DetachInstance()
{
    if (m_bAttached) {
        AfxTermExtensionModule(m_module);
        this->m_hDefaultResource = nullptr;
        this->m_hModuleResource = nullptr;
        m_bAttached = FALSE;
    }
}

inline HINSTANCE CAcExtensionModule::ModuleResourceInstance() const
{
    return m_hModuleResource;
}

class CAcModuleResourceOverride {
public:
    CAcModuleResourceOverride();
    CAcModuleResourceOverride(HINSTANCE hInst);
    ~CAcModuleResourceOverride();
    static HINSTANCE ResourceInstance() { return m_extensionModule.ModuleResourceInstance(); }
private:
    static  CAcExtensionModule& m_extensionModule;
    HINSTANCE m_previousResourceInstance;
};

inline CAcModuleResourceOverride::CAcModuleResourceOverride()
    : CAcModuleResourceOverride(NULL)
{
}

inline CAcModuleResourceOverride::CAcModuleResourceOverride(HINSTANCE hInst)
{
    m_previousResourceInstance = AfxGetResourceHandle();
    HINSTANCE hInstanceToSet = m_extensionModule.ModuleResourceInstance();
    if (hInst)
        hInstanceToSet = hInst;
    AfxSetResourceHandle(hInstanceToSet);
}

inline CAcModuleResourceOverride::~CAcModuleResourceOverride()
{
    ASSERT(m_previousResourceInstance);
    AfxSetResourceHandle(m_previousResourceInstance);
    m_previousResourceInstance = NULL;
}

#define AC_DECLARE_EXTENSION_MODULE(exm)        \
    extern CAcExtensionModule exm;

#define AC_IMPLEMENT_EXTENSION_MODULE(exm)      \
    CAcExtensionModule exm;                     \
    CAcExtensionModule& CAcModuleResourceOverride::m_extensionModule = exm;