
// DICOM ViewerView.h : CDicomView Ŭ������ �������̽�
//

#pragma once
class CDICOMViewerDoc;

class CDicomView : public CView
{
protected: // serialization������ ��������ϴ�.
	CDicomView();
	DECLARE_DYNCREATE(CDicomView)

// Ư���Դϴ�.
public:
	CDICOMViewerDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CDicomView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // DICOM ViewerView.cpp�� ����� ����
inline CDICOMViewerDoc* CDicomView::GetDocument() const
   { return reinterpret_cast<CDICOMViewerDoc*>(m_pDocument); }
#endif

