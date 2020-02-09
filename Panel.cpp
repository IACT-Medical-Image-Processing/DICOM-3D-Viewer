#include "StdAfx.h"
#include "Panel.h"
#include "Histogram.h"
#include "Resource.h"
#include "MainFrm.h"
#include "DICOM ViewerDoc.h"

BEGIN_MESSAGE_MAP(CPanel, CDockablePane)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()

	ON_BN_CLICKED(IDC_SEG, &CPanel::OnBnClickedSeg)
	ON_BN_CLICKED(IDC_RESET, &CPanel::OnBnClickedReset)
END_MESSAGE_MAP()

CPanel::CPanel(void)
{
	m_pannel_mode = PANEL_NONE;
}


CPanel::~CPanel(void)
{
}

void CPanel::OnPaint()
{
	CPaintDC dc(this);

	m_HistoDlg.OnPaint();
	m_SpectDlg.OnPaint();
}


void CPanel::OnSize(UINT nType, int cx, int cy)
{
	//CDockablePane::OnSize(nType, cx, cy);

	CRect rc1;
	CRect rc2;
	CRect rc_sum;

	m_HistoDlg.GetClientRect(&rc1);
	m_SpectDlg.GetClientRect(&rc2);
	rc_sum.left = min(rc1.left, rc2.left);
	rc_sum.right = max(rc1.right,rc2.right);
	rc_sum.top = min(rc1.top, rc2.top);
	rc_sum.bottom = max(rc1.bottom,rc2.bottom);

	CDockablePane::OnSize(nType, rc_sum.right - rc_sum.left, rc_sum.bottom - rc_sum.top);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//AdjustWindowRect(&rc_sum,WS_OVERLAPPED,FALSE);

	m_HistoDlg.MoveWindow(0,0,300,400);
	m_SpectDlg.MoveWindow(0,400,300,300);
}


BOOL CPanel::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CBrush backBrush(RGB(255, 255, 255)); // <- ���Į����. 
	CBrush* pOldBrush = pDC->SelectObject(&backBrush); 

	CRect rect; 
    GetClientRect(&rect);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush); 
	
	return CDockablePane::OnEraseBkgnd(pDC);
}


void CPanel::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_HistoDlg.OnMouseMove(nFlags, point);
	m_SpectDlg.OnMouseMove(nFlags, point);

	CDockablePane::OnMouseMove(nFlags, point);
}


void CPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_HistoDlg.OnLButtonDown(nFlags, point);
	m_SpectDlg.OnLButtonDown(nFlags, point);

	CDockablePane::OnLButtonDown(nFlags, point);
}


void CPanel::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_HistoDlg.OnLButtonUp(nFlags,point);
	m_SpectDlg.OnLButtonUp(nFlags, point);

	CDockablePane::OnLButtonUp(nFlags, point);
}


int CPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	BOOL bRet = m_HistoDlg.Create(IDD_HISTOGRAM, this);
	ASSERT( bRet );
	
	bRet = m_SpectDlg.Create(IDD_SPECTRUM, this);
	ASSERT( bRet );

	btn_seg.Create(L"seg",BS_DEFPUSHBUTTON,CRect(0,0,100,30),this,100);
	btn_reset.Create(L"reset",BS_DEFPUSHBUTTON,CRect(0,50,100,30),this,101);

	return 0;
}


void CPanel::OnDestroy()
{
	CDockablePane::OnDestroy();
}

void CPanel::OnThreshold()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;
	
	pDoc->m_SegMode = THRESHOLD;

	m_HistoDlg.ShowWindow(SW_SHOW);
	m_SpectDlg.ShowWindow(SW_SHOW);

}


void CPanel::OnAThreshold()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	pDoc->m_SegMode = ATHRESHOLD;

	m_HistoDlg.ShowWindow(SW_SHOW);
}


void CPanel::OnGraphCut()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	pDoc->m_SegMode = GRAPHCUT;

	m_HistoDlg.ShowWindow(SW_SHOW);
}

void CPanel::OnDynamicRegionGrowing()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	pDoc->m_SegMode = DYNAMICREGIONGROWING;

	m_HistoDlg.ShowWindow(SW_HIDE);
}

void CPanel::OnLevelSet2D()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	pDoc->m_SegMode = LEVELSET_2D;

	m_HistoDlg.ShowWindow(SW_HIDE);
}

void CPanel::OnLevelSet3D()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	pDoc->m_SegMode = LEVELSET_3D;

	m_HistoDlg.ShowWindow(SW_HIDE);
}

void CPanel::OnBnClickedSeg()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	((CMainFrame *)AfxGetMainWnd())->OnButtonSeg();
}


void CPanel::OnBnClickedReset()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
