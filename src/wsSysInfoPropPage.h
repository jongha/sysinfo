#pragma once

// wsSysInfoPropPage.h : CwsSysInfoPropPage 속성 페이지 클래스의 선언입니다.


// CwsSysInfoPropPage : 구현을 보려면 wsSysInfoPropPage.cpp을 참조하십시오.

class CwsSysInfoPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CwsSysInfoPropPage)
	DECLARE_OLECREATE_EX(CwsSysInfoPropPage)

// 생성자입니다.
public:
	CwsSysInfoPropPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPPAGE_WSSYSINFO };

// 구현
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 메시지 맵입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

