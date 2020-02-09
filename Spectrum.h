#pragma once

#include "resource.h"
#include "afxwin.h"
#include "SpectrumPoint.h"

// Spectrum ��ȭ �����Դϴ�.

class Spectrum : public CDialogEx
{
	DECLARE_DYNAMIC(Spectrum)

public:
	Spectrum(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Spectrum();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SPECTRUM };

	void SetKeyColor(CPoint _pos, COLORREF _color);

	SpectrumPoint m_key_point[256];
	CPoint m_point;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	inline float lerp(float t, float a, float b){return (1-t)*a + t*b;}
	int GetStartEndPos(int _pos_x, int* _pStart,int* _pEnd);
	//int GetStartEndColor(int key_pos_x,int& pre_i, int& next_i);
	void UpdateColorAll();
	int select_key_point(int x, int y);

	bool m_DragDown;
	bool m_bGradient;
	int m_drag_point_index;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedGradient();
};
