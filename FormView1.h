
// DICOM ViewerView.h : CDicomView Ŭ������ �������̽�
//

#pragma once
#include "DICOM ViewerDoc.h"
#include "MainFrm.h"

class CFormView1 : public CScrollView
{
public: // serialization������ ��������ϴ�.
	CFormView1();
	DECLARE_DYNCREATE(CFormView1)

// Ư���Դϴ�.
public:
	CDICOMViewerDoc* GetDocument() const;
	VOID SetCurrentSize(UINT _uSize);
// �۾��Դϴ�.
private:
	int m_uPage1;
	float m_fScale1;
	BOOL m_bStartToMove1;
	CPoint m_StartPoint1;
	CPoint m_MovePoint1;
	CPoint m_DownPoint1;
	BOOL m_bStartToMove1_r;
	CPoint m_StartPoint1_r;
	CPoint m_MovePoint1_r;
	CPoint m_DownPoint1_r;
	CPoint m_margin;
	float m_view_size;

	//CDC memDC;
	//CBitmap bitmap;

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CFormView1();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void MarkObject(CPoint pt, int z);
	int GetPageNo(){	return m_uPage1;	}

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

// 
private :
	float ContrastFunction(float lux_d);
};

#ifndef _DEBUG  // DICOM ViewerView.cpp�� ����� ����
inline CDICOMViewerDoc* CFormView1::GetDocument() const
   { return reinterpret_cast<CDICOMViewerDoc*>(m_pDocument); }
#endif

