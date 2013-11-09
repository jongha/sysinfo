// wsSysInfo.cpp : CwsSysInfoApp 및 DLL 등록의 구현입니다.

#include "stdafx.h"
#include "wsSysInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CwsSysInfoApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x928BD067, 0x19FC, 0x482B, { 0xB0, 0x4C, 0x29, 0xC2, 0x6C, 0xAE, 0xAC, 0x1D } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CwsSysInfoApp::InitInstance - DLL 초기화입니다.

BOOL CwsSysInfoApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: 여기에 직접 작성한 모듈 초기화 코드를 추가합니다.
	}

	return bInit;
}



// CwsSysInfoApp::ExitInstance - DLL 종료입니다.

int CwsSysInfoApp::ExitInstance()
{
	// TODO: 여기에 직접 작성한 모듈 종료 코드를 추가합니다.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - 시스템 레지스트리에 항목을 추가합니다.

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - 시스템 레지스트리에서 항목을 제거합니다.

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
