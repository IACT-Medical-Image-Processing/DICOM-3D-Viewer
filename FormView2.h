
// DICOM ViewerView.h : CDicomView 클래스의 인터페이스
//

#pragma once
#include "DICOM ViewerDoc.h"

class CFormView2 : public CScrollView
{
public: // serialization에서만 만들어집니다.
	CFormView2();
	DECLARE_DYNCREATE(CFormView2)

// 특성입니다.
public:
	CDICOMViewerDoc* GetDocument() const;
// 작업입니다.
public:
	VOID SetCurrentSize(UINT _uSize);
private:
	int m_uPage2;
	float m_fScale2;
	CPoint m_oldPoint2;
	BOOL m_bStartToMove2;
	CPoint m_StartPoint2;
	CPoint m_MovePoint2;
	CPoint m_DownPoint2;
	BOOL m_bStartToMove2_r;
	CPoint m_StartPoint2_r;
	CPoint m_MovePoint2_r;
	CPoint m_DownPoint2_r;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CFormView2();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

// 
private :
	float ContrastFunction(float lux_d);
};

#ifndef _DEBUG  // DICOM ViewerView.cpp의 디버그 버전
inline CDICOMViewerDoc* CFormView2::GetDocument() const
   { return reinterpret_cast<CDICOMViewerDoc*>(m_pDocument); }
#endif

