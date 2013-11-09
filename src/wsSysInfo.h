#pragma once

// wsSysInfo.h : wsSysInfo.DLL의 기본 헤더 파일입니다.

#if !defined( __AFXCTL_H__ )
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // 주 기호입니다.


// CwsSysInfoApp : 구현을 보려면 wsSysInfo.cpp를 참조하십시오.

class CwsSysInfoApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

