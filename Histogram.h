#pragma once
#include "resource.h"

// Histogram ��ȭ �����Դϴ�.

class Histogram : public CDialog
{
	DECLARE_DYNAMIC(Histogram)

public:
	Histogram(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Histogram();

// ��ȭ ���� �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
