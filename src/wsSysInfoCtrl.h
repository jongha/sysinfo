#pragma once

// wsSysInfoCtrl.h : CwsSysInfoCtrl ActiveX 컨트롤 클래스의 선언입니다.


// CwsSysInfoCtrl : 구현을 보려면 wsSysInfoCtrl.cpp을(를) 참조하십시오.

class CwsSysInfoCtrl : public COleControl
{
	DECLARE_DYNCREATE(CwsSysInfoCtrl)

// 생성자입니다.
public:
	CwsSysInfoCtrl();

// 재정의
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// 구현
protected:
	~CwsSysInfoCtrl();

	DECLARE_OLECREATE_EX(CwsSysInfoCtrl)    // 클래스 팩터리와 GUID입니다.
	DECLARE_OLETYPELIB(CwsSysInfoCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CwsSysInfoCtrl)     // 속성 페이지 ID입니다.
	DECLARE_OLECTLTYPE(CwsSysInfoCtrl)		// 형식 이름과 기타 상태입니다.

// 메시지 맵입니다.
	DECLARE_MESSAGE_MAP()

// 디스패치 맵입니다.
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// 이벤트 맵입니다.
	DECLARE_EVENT_MAP()

// 디스패치와 이벤트 ID입니다.
public:
	enum {
		dispidOS = 1L
	};
protected:
	BSTR get_OS(void);
};

