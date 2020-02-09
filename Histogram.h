#pragma once
#include "resource.h"

// Histogram 대화 상자입니다.

class Histogram : public CDialog
{
	DECLARE_DYNAMIC(Histogram)

public:
	Histogram(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~Histogram();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_HISTOGRAM };
private:
	BOOL m_bDragging1;
	BOOL m_bDragging2;
	UINT m_uWidth;
	UINT m_uHeight;
	CPoint m_margin;
	bool m_bObj;
	bool m_bBKG;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedCheckObj();
	afx_msg void OnBnClickedCheckBkg();
};
