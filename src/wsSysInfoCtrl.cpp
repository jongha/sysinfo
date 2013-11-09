// wsSysInfoCtrl.cpp : CwsSysInfoCtrl ActiveX 컨트롤 클래스의 구현입니다.

#include "stdafx.h"
#include "wsSysInfo.h"
#include "wsSysInfoCtrl.h"
#include "wsSysInfoPropPage.h"
#include "wsSysInfoUtil.h"
#include ".\wssysinfoctrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CwsSysInfoCtrl, COleControl)



// 메시지 맵입니다.

BEGIN_MESSAGE_MAP(CwsSysInfoCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// 디스패치 맵입니다.

BEGIN_DISPATCH_MAP(CwsSysInfoCtrl, COleControl)
	DISP_FUNCTION_ID(CwsSysInfoCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CwsSysInfoCtrl, "OS", dispidOS, get_OS, VT_BSTR, VTS_NONE)
END_DISPATCH_MAP()



// 이벤트 맵입니다.

BEGIN_EVENT_MAP(CwsSysInfoCtrl, COleControl)
END_EVENT_MAP()



// 속성 페이지입니다.

// TODO: 필요에 따라 속성 페이지를 추가합니다. 카운트도 이에 따라 증가해야 합니다.
BEGIN_PROPPAGEIDS(CwsSysInfoCtrl, 1)
	PROPPAGEID(CwsSysInfoPropPage::guid)
END_PROPPAGEIDS(CwsSysInfoCtrl)



// 클래스 팩터리와 GUID를 초기화합니다.

IMPLEMENT_OLECREATE_EX(CwsSysInfoCtrl, "WSSYSINFO.wsSysInfoCtrl.1",
	0x304edcbf, 0x45ed, 0x447d, 0x8f, 0x20, 0xe0, 0x39, 0x1f, 0x22, 0xe5, 0x5c)



// 형식 라이브러리 ID와 버전입니다.

IMPLEMENT_OLETYPELIB(CwsSysInfoCtrl, _tlid, _wVerMajor, _wVerMinor)



// 인터페이스 ID입니다.

const IID BASED_CODE IID_DwsSysInfo =
		{ 0x83318352, 0xC83D, 0x4F2E, { 0x83, 0x7E, 0x4B, 0xE0, 0xDD, 0xB4, 0x28, 0xC0 } };
const IID BASED_CODE IID_DwsSysInfoEvents =
		{ 0x25A7B8C, 0xE5A4, 0x4CAF, { 0x98, 0xBC, 0xE6, 0x1A, 0x69, 0x5B, 0x8B, 0x64 } };



// 컨트롤 형식 정보입니다.

static const DWORD BASED_CODE _dwwsSysInfoOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CwsSysInfoCtrl, IDS_WSSYSINFO, _dwwsSysInfoOleMisc)



// CwsSysInfoCtrl::CwsSysInfoCtrlFactory::UpdateRegistry -
// CwsSysInfoCtrl에서 시스템 레지스트리 항목을 추가하거나 제거합니다.

BOOL CwsSysInfoCtrl::CwsSysInfoCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: 컨트롤이 아파트 모델 스레딩 규칙을 준수하는지
	// 확인합니다. 자세한 내용은 MFC Technical Note 64를
	// 참조하십시오. 컨트롤이 아파트 모델 규칙에
	// 맞지 않는 경우 다음에서 여섯 번째 매개 변수를 변경합니다.
	// afxRegApartmentThreading을 0으로 설정합니다.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_WSSYSINFO,
			IDB_WSSYSINFO,
			afxRegApartmentThreading,
			_dwwsSysInfoOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CwsSysInfoCtrl::CwsSysInfoCtrl - 생성자입니다.

CwsSysInfoCtrl::CwsSysInfoCtrl()
{
	InitializeIIDs(&IID_DwsSysInfo, &IID_DwsSysInfoEvents);
	// TODO: 여기에서 컨트롤의 인스턴스 데이터를 초기화합니다.
}



// CwsSysInfoCtrl::~CwsSysInfoCtrl - 소멸자입니다.

CwsSysInfoCtrl::~CwsSysInfoCtrl()
{
	// TODO: 여기에서 컨트롤의 인스턴스 데이터를 정리합니다.
}



// CwsSysInfoCtrl::OnDraw - 그리기 함수입니다.

void CwsSysInfoCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: 다음 코드를 사용자가 직접 작성한 그리기 코드로 바꾸십시오.
//	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
//	pdc->Ellipse(rcBounds);
}



// CwsSysInfoCtrl::DoPropExchange - 지속성 지원입니다.

void CwsSysInfoCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: 지속적인 사용자 지정 속성 모두에 대해 PX_ functions를 호출합니다.
}



// CwsSysInfoCtrl::OnResetState - 컨트롤을 기본 상태로 다시 설정합니다.

void CwsSysInfoCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange에 들어 있는 기본값을 다시 설정합니다.

	// TODO: 여기에서 다른 모든 컨트롤의 상태를 다시 설정합니다.
}



// CwsSysInfoCtrl::AboutBox - "정보" 대화 상자를 사용자에게 보여 줍니다.

void CwsSysInfoCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_WSSYSINFO);
	dlgAbout.DoModal();
}



// CwsSysInfoCtrl 메시지 처리기입니다.

BSTR CwsSysInfoCtrl::get_OS(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// TODO: 여기에 디스패치 처리기를 추가합니다.

	return strResult.AllocSysString();
}
