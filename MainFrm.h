
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#include "Panel.h"
#include "segmentation.h"

class CFormView1;
class CFormView2;
class CFormView3;
class CFormView4;
class CSegmentation;

class CMainFrame : public CFrameWndEx
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:
	inline CView* GetView1(){ return (CView*)m_pView1; }
	inline CView* GetView2(){ return (CView*)m_pView2; }
	inline CView* GetView3(){ return (CView*)m_pView3; }
	inline CView* GetView4(){ return (CView*)m_pView4; }
	inline CPanel* GetPanel(){ return &m_Panel; }
private:
	CSplitterWnd    m_wndSplitter;
	CFormView1*		m_pView1;
	CFormView2*		m_pView2;
	CFormView3*		m_pView3;
	CFormView4*		m_pView4;
	CPanel			m_Panel;
//	CSegmentation	test1;
// �۾��Դϴ�.
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
	void Destroy();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	int maxwindow_type;
// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnButtonSeg();
	afx_msg void OnButtonVol();
	afx_msg void OnButtonModel();
	
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnMaxView1();
	afx_msg void OnMaxView2();
	afx_msg void OnMaxView3();
	afx_msg void OnMaxView4();

	afx_msg void OnDefaultView();
	afx_msg void OnThreshold();
	afx_msg void OnAThreshold();
	afx_msg void OnGraphCut();
	afx_msg void OnLevelSet2D();
	afx_msg void OnLevelSet3D();
	afx_msg void OnVolRender();
	afx_msg void OnGrowing();
};


