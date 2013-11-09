// wsSysInfoPropPage.cpp : CwsSysInfoPropPage 속성 페이지 클래스의 구현입니다.

#include "stdafx.h"
#include "wsSysInfo.h"
#include "wsSysInfoPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CwsSysInfoPropPage, COlePropertyPage)



// 메시지 맵입니다.

BEGIN_MESSAGE_MAP(CwsSysInfoPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// 클래스 팩터리와 GUID를 초기화합니다.

IMPLEMENT_OLECREATE_EX(CwsSysInfoPropPage, "WSSYSINFO.wsSysInfoPropPage.1",
	0x4f1866b2, 0x6db3, 0x41f6, 0x98, 0x7e, 0xff, 0xaf, 0x18, 0xe7, 0x3e, 0x75)



// CwsSysInfoPropPage::CwsSysInfoPropPageFactory::UpdateRegistry -
// CwsSysInfoPropPage에서 시스템 레지스트리 항목을 추가하거나 제거합니다.

BOOL CwsSysInfoPropPage::CwsSysInfoPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_WSSYSINFO_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CwsSysInfoPropPage::CwsSysInfoPropPage - 생성자입니다.

CwsSysInfoPropPage::CwsSysInfoPropPage() :
	COlePropertyPage(IDD, IDS_WSSYSINFO_PPG_CAPTION)
{
}



// CwsSysInfoPropPage::DoDataExchange - 페이지와 속성 사이에서 데이터를 이동시킵니다.

void CwsSysInfoPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CwsSysInfoPropPage 메시지 처리기입니다.
