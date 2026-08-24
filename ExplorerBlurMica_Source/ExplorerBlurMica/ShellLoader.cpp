/*
* BHO���������
*
* Author: Maple
* date: 2022-1-31 Create
* Copyright winmoes.com
*/

#include "ShellLoader.h"

long g_cDllRef = 0;

/*������޸��˴��� ��ʹ��VS��GUID���������µ�GUID��*/
const std::wstring CLSID_SHELL_BHO_STR = L"{B44BD3C8-E597-4E08-AE43-246CE24698E7}";
const CLSID CLSID_SHELL_BHO = { 0xb44bd3c8, 0xe597, 0x4e08, { 0xae, 0x43, 0x24, 0x6c, 0xe2, 0x46, 0x98, 0xe7 } };

// config-editor-wpf.exe looks for these two CLSIDs, not Maplespe's BHO id.
static const std::wstring CLSID_EDITOR_A_STR = L"{BAE3934B-8A6A-4BFB-81BD-3FC599A1BAF3}";
static const CLSID CLSID_EDITOR_A = { 0xbae3934b, 0x8a6a, 0x4bfb, { 0x81, 0xbd, 0x3f, 0xc5, 0x99, 0xa1, 0xba, 0xf3 } };
static const std::wstring CLSID_EDITOR_B_STR = L"{887D3A6A-502E-4AF5-9CE6-D515E12AFE89}";
static const CLSID CLSID_EDITOR_B = { 0x887d3a6a, 0x502e, 0x4af5, { 0x9c, 0xe6, 0xd5, 0x15, 0xe1, 0x2a, 0xfe, 0x89 } };

#pragma region IDispatch

CIDispatch::~CIDispatch()
{
}

STDMETHODIMP CIDispatch::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown || riid == DIID_DWebBrowserEvents2)
		*ppv = static_cast<CIDispatch*>(this);
	else if (riid == IID_IDispatch)
		*ppv = static_cast<IDispatch*>(this);
	else
		return E_NOINTERFACE;
	AddRef();
	return S_OK;
}

ULONG __stdcall CIDispatch::AddRef()
{
	InterlockedIncrement(&g_cDllRef);
	return InterlockedIncrement(&m_ref);
}

ULONG __stdcall CIDispatch::Release()
{
	int tmp = InterlockedDecrement(&m_ref);
	if (tmp == 0)
		delete this;
	InterlockedDecrement(&g_cDllRef);
	return tmp;
}

STDMETHODIMP CIDispatch::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if (!pDispParams)
		return E_INVALIDARG;

	switch (dispIdMember)
	{
	case DISPID_BEFORENAVIGATE2:
		break;
	case DISPID_NAVIGATECOMPLETE2:
		break;
	case DISPID_DOCUMENTCOMPLETE:
	{
		std::wstring path = (wchar_t*)pDispParams->rgvarg->pvarVal->bstrVal;
		if (path != m_lastpath)
		{
			m_lastpath = path;
			OnDocComplete(path, GetCurrentThreadId());
		}
	}
	break;
	case DISPID_DOWNLOADBEGIN:
		break;
	case DISPID_DOWNLOADCOMPLETE:
		break;
	case DISPID_NEWWINDOW2:
		break;
	case DISPID_WINDOWREGISTERED:
		break;
	case DISPID_ONQUIT:
		break;
	default:
		break;
	}

	return S_OK;
}

#pragma endregion

#pragma region CObjectWithSite

CObjectWithSite::CObjectWithSite()
{
}

CObjectWithSite::~CObjectWithSite()
{
	ReleaseRes();
}

STDMETHODIMP CObjectWithSite::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown)
		*ppv = static_cast<CObjectWithSite*>(this);
	else if (riid == IID_IObjectWithSite)
		*ppv = static_cast<IObjectWithSite*>(this);
	else
		return E_NOINTERFACE;
	AddRef();
	return S_OK;
}

ULONG __stdcall CObjectWithSite::AddRef()
{
	InterlockedIncrement(&g_cDllRef);
	return InterlockedIncrement(&m_ref);
}

ULONG __stdcall CObjectWithSite::Release()
{
	int tmp = InterlockedDecrement(&m_ref);
	if (tmp == 0)
		delete this;
	InterlockedDecrement(&g_cDllRef);
	return tmp;
}


STDMETHODIMP CObjectWithSite::SetSite(IUnknown* pUnkSite)
{
	ReleaseRes();
	HRESULT hr = pUnkSite->QueryInterface(IID_IWebBrowser2, (void**)&m_web);
	if (FAILED(hr))
		ReleaseRes();

	IConnectionPointContainer* cpoint = nullptr;

	hr = m_web->QueryInterface(IID_IConnectionPointContainer, (void**)&cpoint);
	if (FAILED(hr)) return E_FAIL;

	hr = cpoint->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_cpoint);
	if (FAILED(hr))
	{
		cpoint->Release();
		return E_FAIL;
	}

	m_cpoint->Advise((IUnknown*)new CIDispatch(), &m_cookie);

	// First CabinetWClass is still constructing here. Hook after a delay.
	ScheduleOnWindowLoad();

	return hr;
}

void CObjectWithSite::ReleaseRes()
{
	if (m_web) m_web->Release();
	if (m_cpoint)
	{
		m_cpoint->Unadvise(m_cookie);
		m_cpoint->Release();
	}
	m_web = nullptr;
	m_cpoint = nullptr;
}

#pragma endregion

#pragma region ClassFactory

STDMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown || riid == IID_IClassFactory)
	{
		*ppv = this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG __stdcall ClassFactory::AddRef()
{
	InterlockedIncrement(&g_cDllRef);
	return InterlockedIncrement(&g_cDllRef);
}

ULONG __stdcall ClassFactory::Release()
{
	int tmp = InterlockedDecrement(&m_ref);
	if (tmp == 0)
		delete this;
	InterlockedDecrement(&g_cDllRef);
	return tmp;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		InterlockedIncrement(&g_cDllRef);
	else
		InterlockedDecrement(&g_cDllRef);
	return S_OK;
}

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, LPVOID* ppvObj)
{
	*ppvObj = NULL;
	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;
	CObjectWithSite* bho = new CObjectWithSite();
	bho->AddRef();
	HRESULT hr = bho->QueryInterface(riid, ppvObj);
	bho->Release();
	return hr;
}

#pragma endregion

#pragma region Reg

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppvOut)
{
	static ClassFactory factory;
	*ppvOut = NULL;
	if (rclsid == CLSID_SHELL_BHO || rclsid == CLSID_EDITOR_A || rclsid == CLSID_EDITOR_B) {
		return factory.QueryInterface(riid, ppvOut);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow(void)
{
	return (g_cDllRef > 0) ? S_FALSE : S_OK;
}

static HRESULT RegisterInProcServer(const std::wstring& clsidStr, const wchar_t* dllpath)
{
	HKEY hkey = 0;
	std::wstring regpath = L"CLSID\\" + clsidStr;
	if (RegCreateKeyExW(HKEY_CLASSES_ROOT, regpath.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return SELFREG_E_CLASS;
	RegSetValueExW(hkey, NULL, 0, REG_SZ, (const BYTE*)L"ExplorerBlurMica BHO", 24 * sizeof(TCHAR));
	RegCloseKey(hkey);

	if (RegCreateKeyExW(HKEY_CLASSES_ROOT, (regpath + L"\\InProcServer32").c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return SELFREG_E_CLASS;
	RegSetValueExW(hkey, NULL, 0, REG_SZ, (const BYTE*)dllpath, DWORD(wcslen(dllpath) + 1) * sizeof(wchar_t));
	RegSetValueExW(hkey, L"ThreadingModel", 0, REG_SZ, (const BYTE*)L"Apartment", 10 * sizeof(wchar_t));
	RegCloseKey(hkey);
	return S_OK;
}

static HRESULT RegisterBho(const std::wstring& clsidStr)
{
	HKEY hkey = 0;
	if (RegCreateKeyExW(HKEY_LOCAL_MACHINE,
		(LR"(Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\)" + clsidStr).c_str(),
		0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return SELFREG_E_CLASS;
	DWORD value = 1;
	RegSetValueExW(hkey, L"NoInternetExplorer", 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));
	RegCloseKey(hkey);
	return S_OK;
}

static void UnregisterClsid(const std::wstring& clsidStr)
{
	RegDeleteKeyW(HKEY_LOCAL_MACHINE, (LR"(Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\)" + clsidStr).c_str());
	RegDeleteKeyW(HKEY_CLASSES_ROOT, (L"CLSID\\" + clsidStr + L"\\InProcServer32").c_str());
	RegDeleteKeyW(HKEY_CLASSES_ROOT, (L"CLSID\\" + clsidStr).c_str());
}

STDAPI DllRegisterServer()
{
	TCHAR dllpath[MAX_PATH];
	GetModuleFileNameW(g_hModule, dllpath, MAX_PATH);

	if (FAILED(RegisterInProcServer(CLSID_SHELL_BHO_STR, dllpath)))
		return SELFREG_E_CLASS;
	if (FAILED(RegisterInProcServer(CLSID_EDITOR_A_STR, dllpath)))
		return SELFREG_E_CLASS;
	if (FAILED(RegisterInProcServer(CLSID_EDITOR_B_STR, dllpath)))
		return SELFREG_E_CLASS;

	if (FAILED(RegisterBho(CLSID_SHELL_BHO_STR)))
		return SELFREG_E_CLASS;
	if (FAILED(RegisterBho(CLSID_EDITOR_A_STR)))
		return SELFREG_E_CLASS;

	return S_OK;
}

STDAPI DllUnregisterServer()
{
	UnregisterClsid(CLSID_SHELL_BHO_STR);
	UnregisterClsid(CLSID_EDITOR_A_STR);
	UnregisterClsid(CLSID_EDITOR_B_STR);
	return S_OK;
}

#pragma endregion
