
// DICOM ViewerView.h : CDicomView 클래스의 인터페이스
//

#pragma once

#include "DICOM ViewerDoc.h"
#include "Camera.h"
#include "RendererHelper.h"
#include "RendererSeg.h"

class CFormView4 : public CScrollView
{
public: // serialization에서만 만들어집니다.
	CFormView4();
private:
	DECLARE_DYNCREATE(CFormView4)

	//CCamera Camera;
	
// 특성입니다.
public:
	CDICOMViewerDoc* GetDocument() const;

// 작업입니다.
public:
	void Init();

	HGLRC m_hRC;
	HDC m_hDC;

private:
	void Destroy();

	void DemoLight(void);
	void GetModuleDirectory();
	void createCube(float _x, float _y, float _z);
	void polygon(float _x, float _y, float _z, int a, int b, int c, int d);
	CRendererHelper m_RendererHelp;
	RendererSeg m_RendererSeg;

	CString ModuleDirectory;
	CString ErrorLog;
		
	GLfloat m_mean_vector[3];
// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
// 구현입니다.
public:
	virtual ~CFormView4();
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();

	void GetMeanVector3();

	//void GLResize(int cx, int cy);
	//void GLRenderScene(void);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // DICOM ViewerView.cpp의 디버그 버전
inline CDICOMViewerDoc* CFormView4::GetDocument() const
   { return reinterpret_cast<CDICOMViewerDoc*>(m_pDocument); }
#endif
