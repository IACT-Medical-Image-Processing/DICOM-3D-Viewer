// DICOM ViewerView.cpp : CDicomView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "DICOM Viewer.h"
#endif
#include "MainFrm.h"
#include "DICOM ViewerDoc.h"
#include "FormView4.h"
#include "Scale.h"
#include "TranformationMgr.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")

using namespace Concurrency;
using namespace cwc;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <wchar.h>
#define _USE_MATH_DEFINES
#include <math.h>
// CDicomView

IMPLEMENT_DYNCREATE(CFormView4, CScrollView)

BEGIN_MESSAGE_MAP(CFormView4, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView4::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_CHAR()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CDicomView 생성/소멸

CFormView4::CFormView4()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_hRC = NULL;
	m_hDC = NULL;
	//selectItem = 0;
}

CFormView4::~CFormView4()
{
	wglDeleteContext(m_hRC);
	::ReleaseDC(m_hWnd, m_hDC);
}

void CFormView4::Init()
{
	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_hDC = ::GetDC(m_hWnd);

	m_RendererHelp.Initialize( m_hDC, pDoc->m_pRawDataProc );
	m_RendererSeg.Initialize();

	m_hRC = wglCreateContext(m_hDC);
}

void CFormView4::Destroy()
{

}

BOOL CFormView4::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.lpszClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
    LoadCursor(NULL, IDC_ARROW),
    (HBRUSH)GetStockObject(BLACK_BRUSH), 
    LoadIcon(NULL, IDI_APPLICATION)); 

	//cs.dwExStyle &= ~WS_BORDER;
	//cs.dwExStyle &= ~WS_CLIPCHILDREN;
	//ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	
	return CScrollView::PreCreateWindow(cs);
}

// CDicomView 그리기

void CFormView4::OnDraw(CDC* /*pDC*/)
{
	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CDicomView 인쇄


void CFormView4::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CFormView4::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CFormView4::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CFormView4::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CFormView4::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
//#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
//#endif
}


// CDicomView 진단

#ifdef _DEBUG
void CFormView4::AssertValid() const
{
	CScrollView::AssertValid();
}

void CFormView4::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CDICOMViewerDoc* CFormView4::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDICOMViewerDoc)));
	return (CDICOMViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CDicomView 메시지 처리기

void CFormView4::DemoLight(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
     
    // Light model parameters:
    // -------------------------------------------
     
    GLfloat lmKa[] = {0.0, 0.0, 0.0, 0.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);
     
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
     
    // -------------------------------------------
    // Spotlight Attenuation
     
    GLfloat spot_direction[] = {1.0, -1.0, -1.0 };
    GLint spot_exponent = 30;
    GLint spot_cutoff = 180;
     
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
    glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
    
    GLfloat Kc = 1.0;
    GLfloat Kl = 0.0;
    GLfloat Kq = 0.0;
     
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,Kc);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);
     
     
    // ------------------------------------------- 
    // Lighting parameters:

    GLfloat light_pos[] = {0.0f, 5.0f, 5.0f, 1.0f};
    GLfloat light_Ka[]  = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat light_Kd[]  = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat light_Ks[]  = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

    // -------------------------------------------
    // Material parameters:

    GLfloat material_Ka[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat material_Kd[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat material_Ks[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat material_Ke[] = {0.1f, 0.1f, 0.1f, 0.0f};
    GLfloat material_Se = 20.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);
}

void CFormView4::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	CDICOMViewerDoc* pDoc = GetDocument();
	if (!pDoc)
	{
		return;  
	}

	if(pDoc->IsVolumeDone)
	{
		m_RendererHelp.Resize( cx, cy );
	}
	else
	{
		VERIFY(wglMakeCurrent(m_hDC,m_hRC));
		m_RendererSeg.Resize( cx, cy );
		VERIFY(wglMakeCurrent(NULL,NULL));
	}
}

void CFormView4::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	m_RendererHelp.SetParent(this);
	m_RendererSeg.SetParent(this);


	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	glClearColor (0.0, 0.0, 0.0, 0.0);
}


void CFormView4::GetMeanVector3()
{
	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;  
	}

	float min_vector[3] = {0,};
	float max_vector[3] = {0,};

	min_vector[0] = pDoc->m_triangles[0].vertex[0][0];
	min_vector[1] = pDoc->m_triangles[0].vertex[0][1];
	min_vector[2] = pDoc->m_triangles[0].vertex[0][2];

	max_vector[0] = pDoc->m_triangles[0].vertex[0][0];
	max_vector[1] = pDoc->m_triangles[0].vertex[0][1];
	max_vector[2] = pDoc->m_triangles[0].vertex[0][2];


	for(size_t i = 0; i < pDoc->m_triangles.size(); i++)
	{
		float* v1 = pDoc->m_triangles[i].vertex[0];
		float* v2 = pDoc->m_triangles[i].vertex[1];
		float* v3 = pDoc->m_triangles[i].vertex[2];

		float local_min_vector[3] = {0,};
		float local_max_vector[3] = {0,};

		// x
		local_min_vector[0] = min(min(v1[0],v2[0]),v3[0]);
		local_max_vector[0] = max(max(v1[0],v2[0]),v3[0]);

		// y
		local_min_vector[1] = min(min(v1[1],v2[1]),v3[1]);
		local_max_vector[1] = max(max(v1[1],v2[1]),v3[1]);

		// z
		local_min_vector[2] = min(min(v1[2],v2[2]),v3[2]);
		local_max_vector[2] = max(max(v1[2],v2[2]),v3[2]);

		for(int k = 0; k < 3; k++)
		{
			if(local_min_vector[k] < min_vector[k])
			{
				min_vector[k] = local_min_vector[k];
			}

			if(local_max_vector[k] > max_vector[k])
			{
				max_vector[k] = local_max_vector[k];
			}
		}
	}

	for(int i = 0; i < 3; i++)
	{
		m_mean_vector[i] = (min_vector[i]+max_vector[i])/2.0f;
	}

	return;
}

void CFormView4::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if(IsIconic())
	{
	}
	else
	{
		//CScrollView::OnPaint();
		CDICOMViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
		{
			return;  
		}

		if(pDoc->IsVolumeDone)
		{
			m_RendererHelp.Render();
			SwapBuffers(m_hDC);
		}
		else
		{
			wglMakeCurrent(m_hDC,m_hRC);
			m_RendererSeg.Render();
			SwapBuffers(m_hDC);
			wglMakeCurrent(m_hDC, NULL);
		}
	}	
}

void CFormView4::OnDestroy()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//wglDeleteContext(m_hRC);
	//::ReleaseDC(m_hWnd, m_hDC);

	CView::OnDestroy();
}

int CFormView4::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();

	//glShaderManager SM;
	//glShader* shader = NULL;
	//shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file

	//if (shader==NULL) 
	//	return -1;

	//shader->enable();


	return 0;
}

void CFormView4::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;  
	}

	if(pDoc->IsVolumeDone)
	{
		m_RendererHelp.LButtonDown(nFlags, point);
		return;
	}
	else
	{
		m_RendererSeg.LButtonDown(nFlags,point);
	}

	CScrollView::OnLButtonDown(nFlags, point);
}

void CFormView4::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;  
	}

	if(pDoc->IsVolumeDone)
	{
		m_RendererHelp.MouseMove(nFlags, point);
	}
	else
	{
		m_RendererSeg.MouseMove(nFlags, point);
	}

	//CScrollView::OnMouseMove(nFlags, point);
}

void CFormView4::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;  
	}

	if(pDoc->IsVolumeDone)
	{
		m_RendererHelp.LButtonUp(nFlags, point);
	}
	else
	{
		m_RendererSeg.LButtonUp(nFlags, point);
	}

	CScrollView::OnLButtonUp(nFlags, point);
}

BOOL CFormView4::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return FALSE;  
	}

	if(pDoc->IsVolumeDone)
	{
		m_RendererHelp.MouseWheel(nFlags, zDelta, pt);
	}
	else
	{
		m_RendererSeg.MouseWheel(nFlags, zDelta, pt);
	}

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CFormView4::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}

void CFormView4::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;  
	}

	if(pDoc->IsVolumeDone)
	{
		m_RendererHelp.RButtonDown(nFlags, point);
	}
	else
	{
		m_RendererSeg.RButtonDown(nFlags, point);
	}

	CScrollView::OnRButtonDown(nFlags, point);
}

void CFormView4::OnRButtonUp(UINT nFlags ,CPoint point)
{

	CDICOMViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;  
	}

	if(pDoc->IsVolumeDone)
	{
		m_RendererHelp.RButtonUp(nFlags, point);
	}
	else
	{
		m_RendererSeg.RButtonUp(nFlags, point);
	}
	

	CScrollView::OnRButtonDown(nFlags, point);
}
