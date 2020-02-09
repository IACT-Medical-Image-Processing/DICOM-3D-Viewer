// Spectrum.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DICOM Viewer.h"
#include "Spectrum.h"
#include "afxdialogex.h"
#include "DICOM ViewerDoc.h"
#include "MainFrm.h"
#include "FormView1.h"
#include "FormView2.h"
#include "FormView3.h"
#include "FormView4.h"
// Spectrum 대화 상자입니다.

IMPLEMENT_DYNAMIC(Spectrum, CDialogEx)

Spectrum::Spectrum(CWnd* pParent /*=NULL*/)
	: CDialogEx(Spectrum::IDD, pParent)
{
	m_DragDown = false;

	for(int i = 0; i < 256; i++)
	{
		RECT rt;
		rt.left = i-5;
		rt.right = i+5;
		rt.top = 40-5;
		rt.bottom = 40+5;
		m_key_point[i].m_rt = rt;

	//	m_key_point[i].m_color = RGB(i,i,i);
	//	m_key_point[i].m_bExist = false;
	}
	
	//m_key_point[0].m_color = RGB(0,0,0);
	//m_key_point[0].m_bExist = true;
	m_key_point[0].SetFixed(true);
	SetKeyColor(0,RGB(255,255,255));
	
	//m_key_point[255].m_color = RGB(255,255,255);
	//m_key_point[255].m_bExist = true;
	m_key_point[255].SetFixed(true);
	SetKeyColor(255,RGB(255,255,255));

	SetKeyColor(80,RGB(0,255,0));
	SetKeyColor(100,RGB(0,0,255));

	m_bGradient = false;
	m_drag_point_index = -1;
}

Spectrum::~Spectrum()
{
}

void Spectrum::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Spectrum, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_GRADIENT, &Spectrum::OnBnClickedGradient)
END_MESSAGE_MAP()


// Spectrum 메시지 처리기입니다.
void Spectrum::SetKeyColor(CPoint _pos, COLORREF _color)
{
	COLORREF start_color = RGB(0,0,0);
	COLORREF end_color = RGB(255,255,255);

	if(m_bGradient == true)
	{
		int start_color_pos = 0;
		int end_color_pos = 255;

		GetStartEndPos(_pos.x, &start_color_pos, &end_color_pos);

		start_color = m_key_point[start_color_pos].m_color;
		end_color = m_key_point[end_color_pos].m_color;

		for(int i = start_color_pos; i < end_color_pos; i++)
		{
			float total_period = end_color_pos - start_color_pos;

			int r = lerp((i-start_color_pos)/total_period, GetRValue(start_color),GetRValue(end_color));
			int g = lerp((i-start_color_pos)/total_period, GetGValue(start_color),GetGValue(end_color));
			int b = lerp((i-start_color_pos)/total_period, GetBValue(start_color),GetBValue(end_color));

			m_key_point[i].m_color = RGB(r,g,b);
		}
	}
	else // pure color
	{
		int start_pos = 0;
		int end_pos = 255;

		GetStartEndPos(_pos.x, &start_pos, &end_pos);

		for(int i = _pos.x; i < end_pos; i++)
		{
			m_key_point[i].m_color = _color;
		}
	}
	
	m_key_point[_pos.x].m_bExist = true;
}

//int Spectrum::GetStartEndColor(int key_pos_x,int& pre_i, int& next_i)
//{
//	bool once = true;
//
//	COLORREF start_color = m_key_point[key_pos_x].m_color;
//
//	for(int i = key_pos_x; i > 0; i--)
//	{
//		if(start_color != m_key_point[i].m_color)
//		{
//			pre_i = i;
//			break;
//		}
//
//		if(i == 0)
//		{
//			pre_i = 0;
//		}
//	}
//
//	for(int i = key_pos_x+1; i < 256; i++)
//	{
//		if(start_color != m_key_point[i].m_color)
//		{
//			next_i = i;
//			break;
//		}
//
//		if(i == 255)
//		{
//			next_i = 255;
//		}
//	}
//	return 0;
//}

int Spectrum::GetStartEndPos(int _pos_x, int* _pPre,int* _pNext)
{
	for(int i = _pos_x; i > 0; i--)
	{		
		if(m_key_point[i].m_bExist == true)
		{
			*_pPre = i;
			break;
		}
	}

	for(int i = _pos_x+1; i < 256; i++)
	{		
		if(m_key_point[i].m_bExist == true)
		{
			*_pNext = i;
			return 0;
		}
	}

	return -1;
}

void Spectrum::UpdateColorAll( )
{
	int start_pos = 0;
	int end_pos = 255;

	int start_color_pos = 0;
	int end_color_pos = 255;

	COLORREF start_color = RGB(0,0,0);
	COLORREF end_color = RGB(255,255,255);

	if(m_bGradient == true)
	{
		for(int i = 0; i < 255; i++) // 전체 탐색
		{
			if(m_key_point[i].m_bExist == true) // 키포인트 존재하면
			{
				GetStartEndPos(i, &start_color_pos,&end_color_pos);

				start_color = m_key_point[start_color_pos].m_color;
				end_color = m_key_point[end_color_pos].m_color;

				int k = 0;
				for(k = start_color_pos; k < end_color_pos; k++)
				{
					float total_period = end_color_pos - start_color_pos;

					int r = lerp((k-start_color_pos+1)/total_period, GetRValue(start_color),GetRValue(end_color));
					int g = lerp((k-start_color_pos+1)/total_period, GetGValue(start_color),GetGValue(end_color));
					int b = lerp((k-start_color_pos+1)/total_period, GetBValue(start_color),GetBValue(end_color));

					m_key_point[k].m_color = RGB(r,g,b);
				}

				i = end_color_pos -1;
			}
		}
	}
	else // primary color 
	{
		for(int i = 0; i < 255; i++) // 전체 탐색
		{
			GetStartEndPos(i, &start_pos,&end_pos);

			int k = 0;
			for(k = start_pos; k < end_pos; k++)
			{
				m_key_point[k].m_color = m_key_point[start_pos].m_color;
			}

			i = end_pos - 1;
		}
	}
}

void Spectrum::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	CDICOMViewerDoc* pDoc = NULL;
	pDoc = (CDICOMViewerDoc*) ((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	
	if(pDoc == NULL)
		return;

	if(!pDoc->IsDataLoaded())
		return;

	CRect rect;
    GetClientRect(&rect);

	CDC memDC;
	CBitmap bitmap;

	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());

    BITMAP bmpInfo;
    bitmap.GetBitmap(&bmpInfo);
 
    CBitmap *pOldBitmap = (CBitmap *) memDC.SelectObject(&bitmap);

	CPen* pOldPen = NULL;
	CBrush* pOldBrush = NULL;

	CBrush white_brush;
	white_brush.CreateSolidBrush(RGB(255,255,255) );
	pOldBrush = memDC.SelectObject(&white_brush);
	memDC.FillRect(CRect(0,0,256,80),&white_brush);

	CPen black_pen;
	black_pen.CreatePen(PS_SOLID, 1, RGB(0,0,0) );

	pOldPen = memDC.SelectObject(&black_pen);

	// spectrum
	for (int i=0; i <256; i++)
	{	
		int r = GetRValue(m_key_point[i].m_color);
		int g = GetGValue(m_key_point[i].m_color);
		int b = GetBValue(m_key_point[i].m_color);

		COLORREF rgb = RGB(r,g,b);
		CPen rainbow_pen;
		rainbow_pen.CreatePen(PS_SOLID, 1, rgb );

		memDC.SelectObject(&rainbow_pen);

		// y line draw
		memDC.MoveTo(CPoint(i,0));
		memDC.LineTo(CPoint(i,80));
	}
	
	// key point
	for (int i=0; i <256; i++)
	{
		if(m_key_point[i].m_bExist == true)
		{
			CBrush white_brush;
			white_brush.CreateSolidBrush(m_key_point[i].m_color);
			memDC.SelectObject(&white_brush);

			memDC.SelectObject(&black_pen);

			if(m_key_point[i].m_bDrag == true)
			{
				RECT rt;
				rt.left = m_point.x-5;
				rt.right = m_point.x+5;
				
				rt.top = m_point.y-5;
				rt.bottom = m_point.y+5;

				m_key_point[i].m_rt = rt;
			}

			memDC.Ellipse(&m_key_point[i].m_rt);
		}
	}
	
	//BLENDFUNCTION bf;
	//bf.BlendOp=AC_SRC_OVER; 
	//bf.BlendFlags=0; 
	//bf.AlphaFormat=0;

	//for (int y=0; y <80; y++)
	//{
	//	bf.SourceConstantAlpha=y*255.0/80.0; 
	//	dc.AlphaBlend(0,y,256,1,&memDC,0,y,256,1,bf);
	//}

	dc.StretchBlt(0, 0,
		256, 80, &memDC,
		0,0, 256, 80, SRCCOPY);

	//CString str1;
	//str1.Format("%f %f",);
	//TextOut(dc.GetSafeHdc(),0,100,str,lstrlen(str1));

	memDC.SelectObject(pOldBrush);
	memDC.SelectObject(pOldPen);
	memDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	memDC.DeleteDC();
}

int Spectrum::select_key_point(int x, int y)
{
	for(int j = -5; j <= 5; j++)
	{
		if(m_key_point[x+j].m_bExist == false)
			continue;

		if(m_key_point[x+j].InsideRect(x+j,y+j)== false)
			continue;

		return x+j;
	}
	return -1;
}

void Spectrum::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDICOMViewerDoc* pDoc = NULL;
	pDoc = (CDICOMViewerDoc*) ((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	if(pDoc == NULL)
		return;

	CRect rect;
    GetClientRect(&rect);


	m_drag_point_index = select_key_point(point.x, point.y);
	if(m_drag_point_index == -1)
		return;
	
	if(m_key_point[m_drag_point_index].GetFixed())
		return;
		
	if(m_drag_point_index < 0)
	{
		m_drag_point_index = 0;
	}
		
	if(m_drag_point_index > 255)
	{
		m_drag_point_index = 255;
	}

	m_key_point[m_drag_point_index].m_bDrag = true;
	m_DragDown = true;

	CDialogEx::OnLButtonDown(nFlags, point);
}


void Spectrum::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_DragDown == true)
	{
		// 현재 좌표에 있는 것을 설정
		
		if(point.x < 0)
		{
			point.x = 0;
		}
		if(point.x > 255)
		{
			point.x = 255;
		}

		// 현재 포인트로 설정
		m_key_point[point.x].m_bExist = true;
		m_key_point[point.x].m_color = m_key_point[m_drag_point_index].m_color;


		// 포인트 원 좌표설정
		m_key_point[point.x].m_rt.left = point.x-5;
		if(m_key_point[point.x].m_rt.left < 0)
		{
			m_key_point[point.x].m_rt.left = 0;
		}
		m_key_point[point.x].m_rt.top = point.y-5;
		if(m_key_point[point.x].m_rt.top < 0)
		{
			m_key_point[point.x].m_rt.top = 0;
		}
		m_key_point[point.x].m_rt.right = point.x+5;
		if(m_key_point[point.x].m_rt.right > 255)
		{
			m_key_point[point.x].m_rt.right = 255;
		}
		m_key_point[point.x].m_rt.bottom = point.y+5;
		if(m_key_point[point.x].m_rt.bottom > 255)
		{
			m_key_point[point.x].m_rt.bottom = 255;
		}


		// 포인트 위치교체
		m_key_point[m_drag_point_index].m_bExist = false;
		m_key_point[m_drag_point_index].m_bDrag = false;
		m_drag_point_index = -1;
		// 화면갱신 후 초기화
		
		UpdateColorAll();

		//// 화면 갱신
		//CFormView1* pView1 = (CFormView1*) ((CMainFrame*)AfxGetMainWnd())->GetView1();
		//if(!pView1) 
		//	return;
		//CFormView2* pView2 = (CFormView2*) ((CMainFrame*)AfxGetMainWnd())->GetView2();
		//if(!pView2) 
		//	return;
		//CFormView3* pView3 = (CFormView3*) ((CMainFrame*)AfxGetMainWnd())->GetView3();
		//if(!pView3) 
		//	return;
		//CFormView4* pView4 = (CFormView4*) ((CMainFrame*)AfxGetMainWnd())->GetView4();
		//if(!pView4) 
		//	return;

		//pView1->Invalidate(FALSE);
		//pView2->Invalidate(FALSE);
		//pView3->Invalidate(FALSE);
		//pView4->Invalidate(FALSE);

		Invalidate(FALSE);

		m_DragDown = false;
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void Spectrum::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_DragDown == true)
	{
		if(m_key_point[m_drag_point_index].GetFixed() == true)
		{
			m_point.y = point.y;
		}
		else
		{
			m_point = point;
		}
		Invalidate(FALSE);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

void Spectrum::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(point.x < 0 )
	{
		point.x = 0;
	}

	if(point.x > 255)
	{
		point.x = 255;
	}

	if((point.y <= 10+80)&&(point.y >= 10))
	{
		// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
		 CColorDialog ins_dlg;
		// 초기 색상값을 사용하고 대화상자는 확장된 형식으로 출력하겠다고 플래그를 추가합니다.
		ins_dlg.m_cc.Flags |= CC_RGBINIT | CC_FULLOPEN;
		// 기존컬러를 초기 선택값으로 사용하겠다고 지정합니다.
		ins_dlg.m_cc.rgbResult = m_key_point[point.x].m_color;
		if(ins_dlg.DoModal() == IDOK)
		{
			COLORREF c = ins_dlg.GetColor();
			if(m_key_point[point.x].m_bExist == true)
			{
				int key_point = select_key_point(point.x, point.y);
				m_key_point[point.x].m_color = c;
				UpdateColorAll();
			}
			else
			{
				SetKeyColor(point.x, c);
				UpdateColorAll();
			}
			Invalidate(FALSE);
			//theApp.UpdateView();
		}
	}
	CDialogEx::OnLButtonDblClk(nFlags, point);
}



void Spectrum::OnBnClickedGradient()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(((CButton *)GetDlgItem(IDC_GRADIENT))->GetCheck() == BST_CHECKED) 
	{ 
		//((CButton*)GetDlgItem(IDC_GRADIENT))->SetCheck(0);  //체크->체크 해제 
		m_bGradient = true;
	}
	else 
	{ 
		//((CButton*)GetDlgItem(IDC_GRADIENT))->SetCheck(1);//체크 해제->체크 
		m_bGradient = false;
	}
	UpdateColorAll();
	Invalidate(FALSE);
	//theApp.UpdateView();
}