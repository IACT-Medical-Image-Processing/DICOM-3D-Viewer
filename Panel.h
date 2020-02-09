#pragma once

#include "afxdockablepane.h"
#include "Histogram.h"
#include "Spectrum.h"

enum PANEL_MODE
{
	PANEL_NONE,
	PANEL_THRES,
	PANEL_A_THRES,
	PANEL_GRAPH_CUT,
	PANEL_LEVEL_SET
};

class CPanel : public CDockablePane
{
public:
	afx_msg void OnThreshold();
	afx_msg void OnAThreshold();
	afx_msg void OnGraphCut();
	afx_msg void OnLevelSet2D();
	afx_msg void OnLevelSet3D();
	afx_msg void OnDynamicRegionGrowing();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	inline void SetMode(PANEL_MODE _panel_mode){ m_pannel_mode = _panel_mode; }
	
	Histogram m_HistoDlg;
	Spectrum m_SpectDlg;
	CButton btn_seg;
	CButton btn_reset;

private:
	PANEL_MODE m_pannel_mode;
public:
	CPanel(void);
	~CPanel(void);
	
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedSeg();
	afx_msg void OnBnClickedReset();
};