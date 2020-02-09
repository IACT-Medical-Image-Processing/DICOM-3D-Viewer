// Histogram.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DICOM Viewer.h"
#include "Histogram.h"
#include "afxdialogex.h"
#include "DICOM ViewerDoc.h"
#include "MainFrm.h"
#include "FormView1.h"
#include "FormView2.h"
#include "FormView3.h"
#include "FormView4.h"

// Histogram 대화 상자입니다.

IMPLEMENT_DYNAMIC(Histogram, CDialog)

Histogram::Histogram(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HISTOGRAM,this)
{
	m_uWidth = 256;
	m_uHeight = 256;
	m_bDragging1 = FALSE;
	m_bDragging2 = FALSE;
	m_margin.x = 10;
	m_margin.y = 30;
	m_bObj = false;
	m_bBKG = false;
}

Histogram::~Histogram()
{
}

void Histogram::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Histogram, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHECK_OBJ, &Histogram::OnBnClickedCheckObj)
	ON_BN_CLICKED(IDC_CHECK_BKG, &Histogram::OnBnClickedCheckBkg)
END_MESSAGE_MAP()


// Histogram 메시지 처리기입니다.


void Histogram::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc*) ((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
		
	if(pDoc == NULL)
		return;

	if(!pDoc->IsDataLoaded())
		return;

	CDC memDC;
	CBitmap bitmap;

	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc,m_uWidth,m_uHeight);

    BITMAP bmpInfo;
    bitmap.GetBitmap(&bmpInfo);
 
    CBitmap *pOldBitmap = (CBitmap *) memDC.SelectObject(&bitmap);
 
	for (int i=1; i <256; i++)
	{
		if(pDoc->m_fMaxHisto < pDoc->m_pfHistogram[i])
		{
			pDoc->m_fMaxHisto = pDoc->m_pfHistogram[i];
		}
	}
	
	CPen green_pen;
	green_pen.CreatePen(PS_SOLID, 1, RGB(0,255,0));

	CPen* pOldPen;
	pOldPen = memDC.SelectObject(&green_pen);

	for (int i=1; i <256; i++)
	{
		float percent = ((float)(pDoc->m_pfHistogram[i])/(float)(pDoc->m_fMaxHisto))*(float)(m_uHeight);

		memDC.MoveTo(CPoint(i,m_uHeight));
		memDC.LineTo(CPoint(i,m_uHeight - (int)(percent)*0.5));

		//memDC.SetPixel(CPoint(i,512 - (int)percent),RGB(0,255,0));
	}

	CPen red_pen;
	red_pen.CreatePen(PS_SOLID, 2, RGB(255,0,0));
	memDC.SelectObject(&red_pen);

	memDC.MoveTo(CPoint(pDoc->m_startHisto,m_uHeight));
	memDC.LineTo(CPoint(pDoc->m_startHisto,0));
	
	//CRect rtStart;
	//rtStart.left = pDoc->m_startHisto - 10;
	//rtStart.right = pDoc->m_startHisto + 10;
	//rtStart.top = m_uHeight-20;
	//rtStart.bottom = m_uHeight;
	//
	//memDC.Rectangle(&rtStart);

	memDC.MoveTo(CPoint(pDoc->m_endHisto,m_uHeight));
	memDC.LineTo(CPoint(pDoc->m_endHisto,0));

	//CRect rtEnd;
	//rtEnd.left = pDoc->m_endHisto - 10;
	//rtEnd.right = pDoc->m_endHisto + 10;
	//rtEnd.top = m_uHeight-20;
	//rtEnd.bottom = m_uHeight;
	//
	//memDC.Rectangle(&rtEnd);

	CRect rect;
    GetClientRect(&rect);
	dc.StretchBlt(m_margin.x, m_margin.y, 
		m_uWidth, m_uHeight, &memDC,
		0,0, m_uWidth, m_uHeight, SRCCOPY);
 
	memDC.SelectObject(pOldPen);
	memDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	memDC.DeleteDC();
}


void Histogram::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDICOMViewerDoc* pDoc = NULL;
	pDoc = (CDICOMViewerDoc*) ((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	if(pDoc == NULL)
		return;

	if(m_bDragging1)
	{
		pDoc->m_startHisto = (UINT)point.x - m_margin.x;

		if(pDoc->m_startHisto > (pDoc->m_endHisto -10))
		{
			pDoc->m_startHisto = pDoc->m_endHisto -10;
		}

		if(pDoc->m_startHisto < 0)
		{
			pDoc->m_startHisto = 0;
		}

		if(pDoc->m_startHisto > (int)m_uWidth)
		{
			pDoc->m_startHisto = m_uWidth;
		}
	}
	
	else if(m_bDragging2)
	{
		pDoc->m_endHisto = (UINT)point.x - m_margin.x;

		if(pDoc->m_endHisto < (int)(pDoc->m_startHisto +10))
		{
			pDoc->m_endHisto = pDoc->m_startHisto +10;
		}

		if(pDoc->m_endHisto < 0)
		{
			pDoc->m_endHisto = 0;
		}

		if(pDoc->m_endHisto > (int)m_uWidth)
		{
			pDoc->m_endHisto = m_uWidth;
		}
	}
	
	Invalidate(FALSE);

	//CDialog::OnMouseMove(nFlags, point);
}


void Histogram::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDICOMViewerDoc* pDoc = NULL;
	pDoc = (CDICOMViewerDoc*) ((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	if(pDoc == NULL)
		return;
	
	CRect rect;
    GetClientRect(&rect);

	CRect rtStart;
	rtStart.left = pDoc->m_startHisto - 10 + m_margin.x;
	rtStart.right = pDoc->m_startHisto + 10 + m_margin.x;
	rtStart.top = 0;
	rtStart.bottom = 300;

	m_bDragging1 = rtStart.PtInRect(point);

	if(m_bDragging1 == FALSE)
	{
		CRect rtEnd;
		rtEnd.left = pDoc->m_endHisto - 10 + m_margin.x;
		rtEnd.right = pDoc->m_endHisto + 10 + m_margin.x;
		rtEnd.top = 0;
		rtEnd.bottom = 300;

		m_bDragging2 = rtEnd.PtInRect(point);
	}

	CFormView1* pView1 = (CFormView1*) ((CMainFrame*)AfxGetMainWnd())->GetView1();
	if(!pView1) 
		return;

	CFormView2* pView2 = (CFormView2*) ((CMainFrame*)AfxGetMainWnd())->GetView2();
	if(!pView2) 
		return;
	CFormView3* pView3 = (CFormView3*) ((CMainFrame*)AfxGetMainWnd())->GetView3();
	if(!pView3) 
		return;

	pView1->Invalidate(FALSE);
	pView2->Invalidate(FALSE);
	pView3->Invalidate(FALSE);

	//CDialog::OnLButtonDown(nFlags, point);
}


void Histogram::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bDragging1 = FALSE;
	m_bDragging2 = FALSE;

	((CMainFrame*)AfxGetMainWnd())->OnButtonVol();

	//CFormView1* pView1 = (CFormView1*) ((CMainFrame*)AfxGetMainWnd())->GetView1();
	//CFormView2* pView2 = (CFormView2*) ((CMainFrame*)AfxGetMainWnd())->GetView2();
	//CFormView3* pView3 = (CFormView3*) ((CMainFrame*)AfxGetMainWnd())->GetView3();
	//CFormView4* pView4 = (CFormView4*) ((CMainFrame*)AfxGetMainWnd())->GetView4();

	//pView1->Invalidate(FALSE);
	//pView2->Invalidate(FALSE);
	//pView3->Invalidate(FALSE);
	//pView4->Invalidate(FALSE);

	////CDialog::OnLButtonUp(nFlags, point);
}


int Histogram::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	return 0;
}

void Histogram::OnBnClickedCheckObj()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CheckDlgButton(IDC_CHECK_BKG,FALSE);
	m_bBKG = false;

	if(m_bObj == false)
	{
		CheckDlgButton(IDC_CHECK_OBJ,TRUE);
		m_bObj = true;
	}
	else
	{
		CheckDlgButton(IDC_CHECK_OBJ,FALSE);
		m_bObj = false;
	}

}


void Histogram::OnBnClickedCheckBkg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CheckDlgButton(IDC_CHECK_OBJ,FALSE);
	m_bObj = false;

	if(m_bBKG == false)
	{
		CheckDlgButton(IDC_CHECK_BKG,TRUE);
		m_bBKG = true;
	}
	else
	{
		CheckDlgButton(IDC_CHECK_BKG,FALSE);
		m_bBKG = false;
	}

}
