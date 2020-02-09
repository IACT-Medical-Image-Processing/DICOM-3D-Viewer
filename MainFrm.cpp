
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include <Windows.h>
#include <crtdbg.h>

#include "DICOM Viewer.h"

#include "MainFrm.h"
#include "FormView1.h"
#include "FormView2.h"
#include "FormView3.h"
#include "FormView4.h"
#include "segmentation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_BUTTON_VOL, &CMainFrame::OnButtonVol)
	ON_COMMAND(ID_BUTTON_SEG, &CMainFrame::OnButtonSeg)
	ON_COMMAND(ID_BUTTON_MODEL, &CMainFrame::OnButtonModel)

	ON_COMMAND(ID_MAX_VIEW_1, &CMainFrame::OnMaxView1)
	ON_COMMAND(ID_MAX_VIEW_2, &CMainFrame::OnMaxView2)
	ON_COMMAND(ID_MAX_VIEW_3, &CMainFrame::OnMaxView3)
	ON_COMMAND(ID_MAX_VIEW_4, &CMainFrame::OnMaxView4)
	ON_COMMAND(ID_DEFAULT_VIEW, &CMainFrame::OnDefaultView)
	ON_COMMAND(ID_THRSHOLD, &CMainFrame::OnThreshold)
	ON_COMMAND(ID_A_THRESHOLD, &CMainFrame::OnAThreshold)
	ON_COMMAND(ID_GRAPH_CUT, &CMainFrame::OnGraphCut)
	ON_COMMAND(ID_LEVEL_SET_2D, &CMainFrame::OnLevelSet2D)
	ON_COMMAND(ID_LEVEL_SET_3D, &CMainFrame::OnLevelSet3D)
	ON_COMMAND(ID_VOL_RENDER, &CMainFrame::OnVolRender)
	ON_COMMAND(ID_GROWING, &CMainFrame::OnGrowing)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	//_CrtSetBreakAlloc(132);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

	m_pView1 = NULL;
	m_pView2 = NULL;
	m_pView3 = NULL;
	m_pView4 = NULL;

	maxwindow_type = 0;
}

CMainFrame::~CMainFrame()
{
	Destroy();
}

void CMainFrame::Destroy()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	m_bAutoMenuEnable = FALSE;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("메뉴 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	m_wndMenuBar.EnableMenuShadows(FALSE);
	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 사용자 정의 도구 모음 이미지를 로드합니다.
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	//CMFCToolBar::SetBasicCommands(lstBasicCommands);

	//CMFCToolBar::SetSizes(CSize(32,32),CSize(32,32));
	// 제어판
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
	WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI;

	CString strCaption(_T("제어판"));
	if(!m_Panel.Create(strCaption, this, CRect(0,0,300,300),TRUE,(UINT)1,dwStyle))
	{
		TRACE(_T("제어판 생성 실패"));
		return -1;
	}

	m_Panel.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_Panel);
	//

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.dwExStyle &= ~WS_CLIPCHILDREN;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 기본 클래스가 실제 작업을 수행합니다.

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)  
{
	CRect rect;
	GetClientRect(&rect);
 
	// 아래는 좌우 폭 비율
	CSize size1(MulDiv(rect.Width()-300, 50, 100), MulDiv(rect.Height(), 50, 100)); // 폭(30%), 높이(50%)
	CSize size2(MulDiv(rect.Width()-300, 50, 100), MulDiv(rect.Height(), 50, 100)); // 폭(70%), 높이(50%)

  // 아래는 상하 폭 비율 (절반)
  //CSize size1(MulDiv(rect.Width(), 50, 100), ::GetSystemMetrics(SM_CXSCREEN)); // 높이(50%)
 
  // create a splitter with 2 row, 2 columns 
  if (!m_wndSplitter.CreateStatic(this, 2, 2, WS_CHILD | WS_VISIBLE)) { 
      TRACE0("Failed to CreateStatic Splitter \n"); 
      return FALSE; 
  }
 
  // 사용자가 만든 CFormView1 을 좌측에 배치합니다... 
  if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CFormView1), size1, pContext)) { 
      TRACE0("Failed to create CFormView1 pane \n"); 
      return FALSE; 
  }
 
  // 사용자가 만든 CFormView2 을 우측에 배치합니다... 
  if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CFormView2), size2, pContext)) { 
      TRACE0("Failed to create CFormView2 pane \n"); 
      return FALSE; 
  }
 
  if (!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CFormView3), size1, pContext)) { 
      TRACE0("Failed to create CFormView3 pane \n"); 
      return FALSE; 
  }
 
  if (!m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CFormView4), size2, pContext)) { 
      TRACE0("Failed to create CFormView4 pane \n"); 
      return FALSE; 
  }
 
	m_pView1 = (CFormView1*)m_wndSplitter.GetPane(0, 0);
	m_pView2 = (CFormView2*)m_wndSplitter.GetPane(1, 0);
	m_pView3 = (CFormView3*)m_wndSplitter.GetPane(0, 1);
	m_pView4 = (CFormView4*)m_wndSplitter.GetPane(1, 1);
  
	return TRUE;
  //return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_pView1 == NULL)
		return;
	if(m_pView2 == NULL)
		return;
	if(m_pView3 == NULL)
		return;
	if(m_pView4 == NULL)
		return;

	//RedrawWindow(NULL, NULL, RDW_NOCHILDREN | RDW_NOERASE | RDW_INVALIDATE);

	CRect rect;
	GetClientRect(&rect);
 
	// 아래는 좌우 폭 비율
	CSize size1(MulDiv(rect.Width()-300, 50, 100), MulDiv(rect.Height(), 50, 100));
	CSize size2(MulDiv(rect.Width()-300, 50, 100), MulDiv(rect.Height(), 50, 100));

	HDWP hdwp = ::BeginDeferWindowPos(4);

	CRect Rect1(0,0,size1.cx,size1.cy);
	::DeferWindowPos (hdwp, m_pView1->GetSafeHwnd(), HWND_TOP,
	Rect1.left, Rect1.top, Rect1.Width (), Rect1.Height (),
	SWP_NOREDRAW); 

	CRect Rect2(0,size1.cy,size1.cx,size1.cy*2);
	::DeferWindowPos (hdwp, m_pView2->GetSafeHwnd(), HWND_TOP,
	Rect2.left, Rect2.top, Rect2.Width (), Rect2.Height (),
	SWP_NOREDRAW);

	CRect Rect3(size1.cx,0,size1.cx*2,size1.cy);
	::DeferWindowPos (hdwp, m_pView3->GetSafeHwnd(), HWND_TOP,
	Rect3.left, Rect3.top, Rect3.Width (), Rect3.Height (),
	SWP_NOREDRAW);

	CRect Rect4(size1.cx,size1.cy,size1.cx*2,size1.cy*2);
	::DeferWindowPos (hdwp, m_pView4->GetSafeHwnd(), HWND_TOP,
	Rect4.left, Rect4.top, Rect4.Width (), Rect4.Height (),
	SWP_NOREDRAW);

	::EndDeferWindowPos (hdwp);

	//RecalcLayout();
	
	//m_pView1->ResizeParentToFit(TRUE);
	//m_pView2->ResizeParentToFit(TRUE);
	//m_pView3->ResizeParentToFit(TRUE);
	////m_pView4->ResizeParentToFit(FALSE);

	
	if ( nType != SIZE_MINIMIZED )
	{
		switch(maxwindow_type)
		{
			case 0:
				m_wndSplitter.SetColumnInfo( 0, size1.cx, 50);
				m_wndSplitter.SetColumnInfo( 1, size2.cx, 50);
				m_wndSplitter.SetRowInfo( 0, size1.cy, 50 );
				m_wndSplitter.SetRowInfo( 1, size2.cy, 50 );
				m_wndSplitter.RecalcLayout();
				break;
			case 1:
				m_wndSplitter.SetColumnInfo( 0, rect.right, 0);
				m_wndSplitter.SetColumnInfo( 1, 0, 0);
				m_wndSplitter.SetRowInfo( 0, rect.bottom, 0 );
				m_wndSplitter.SetRowInfo( 1, 0, 0 );
				m_wndSplitter.RecalcLayout();
				break;
			case 2:
				m_wndSplitter.SetColumnInfo( 0, 0, 0);
				m_wndSplitter.SetColumnInfo( 1, rect.right, 0);
				m_wndSplitter.SetRowInfo( 0, rect.bottom, 0 );
				m_wndSplitter.SetRowInfo( 1, 0, 0 );
				m_wndSplitter.RecalcLayout();
				break;
			case 3:
				m_wndSplitter.SetColumnInfo( 0, rect.right, 0);
				m_wndSplitter.SetColumnInfo( 1, 0, 0);
				m_wndSplitter.SetRowInfo( 0, rect.bottom, 0 );
				m_wndSplitter.SetRowInfo( 1, 0, 0 );
				m_wndSplitter.RecalcLayout();
				break;
			case 4:
				m_wndSplitter.SetColumnInfo( 0, 0, 0);
				m_wndSplitter.SetColumnInfo( 1, rect.right, 0);
				m_wndSplitter.SetRowInfo( 0, 0, 0 );
				m_wndSplitter.SetRowInfo( 1, rect.bottom, 0 );
				m_wndSplitter.RecalcLayout();
				break;
		}
	}
}


BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CFrameWndEx::OnEraseBkgnd(pDC);
}


void CMainFrame::OnButtonSeg()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	pDoc->m_triangles.clear();
	pDoc->m_triangles.reserve(1000000);

	if(m_pView1 == NULL)
		return;
	
	int pageNo1 = m_pView1->GetPageNo();
//	test1.
	Segmentation(pDoc->m_SegMode, pDoc->m_startHisto, 
					pDoc->GetFileNumber(), &(pDoc->m_triangles), pDoc->m_HUmin, pDoc->m_HUmax, pageNo1);

	pDoc->m_bSegFirst = false;
	
	pDoc->IsSegmentationDone = true;
	pDoc->IsVolumeDone = false;
	pDoc->m_bCalcMean = false;

	theApp.UpdateView();
}


void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//Destroy();

//	test1.Destroy();

	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	pDoc->Destroy();

	CFrameWndEx::OnClose();
}


void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_Panel.OnLButtonDown(nFlags,point);
	CFrameWndEx::OnLButtonDown(nFlags, point);
}


void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_Panel.OnLButtonUp(nFlags,point);
	CFrameWndEx::OnLButtonUp(nFlags, point);
}


void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_Panel.OnMouseMove(nFlags,point);
	CFrameWndEx::OnMouseMove(nFlags, point);
}


void CMainFrame::OnButtonVol()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;
	
	pDoc->SetFileNumber((int)theApp.m_vFileList.size());
	//pDoc->Init();
	pDoc->m_HUmax = 0;
	pDoc->m_HUmin = 0;
	pDoc->m_bCalcMean = false;

	pDoc->IsVolumeDone = true;
	pDoc->IsSegmentationDone = false;

	m_pView4->Init();
	
	for(int i = 0; i < (int)pDoc->GetFileNumber(); i++)
	{
		theApp.ReadImage(i);
	}

	theApp.MakeHistogram();

	CRect rc;
	m_pView4->GetClientRect(rc);
	m_pView4->SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
	m_pView4->Invalidate(TRUE);

	pDoc->DataLoaded(TRUE);

	theApp.UpdateView();
}

void CMainFrame::OnButtonModel()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();

	pDoc->m_triangles.clear();
	CMarchingCube model;
	model.Start(pDoc->m_startHisto, &(pDoc->m_triangles), pDoc->GetFileNumber());

	// 뷰4의 OnSize를 호출하기 위한 코드
	CRect rc;
	m_pView4->GetClientRect(rc);
	m_pView4->SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	m_pView4->Invalidate(TRUE);
	theApp.UpdateView();
}

void CMainFrame::OnDefaultView()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	maxwindow_type = 0;
	
	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}


void CMainFrame::OnMaxView1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	maxwindow_type = 1;
	
	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}


void CMainFrame::OnMaxView2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	maxwindow_type = 2;

	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}


void CMainFrame::OnMaxView3()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	maxwindow_type = 3;

	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}


void CMainFrame::OnMaxView4()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	maxwindow_type = 4;

	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}



void CMainFrame::OnThreshold()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_Panel.OnThreshold();
}


void CMainFrame::OnAThreshold()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_Panel.OnAThreshold();
}


void CMainFrame::OnGraphCut()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_Panel.OnGraphCut();
}


void CMainFrame::OnLevelSet2D()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_Panel.OnLevelSet2D();
}

void CMainFrame::OnLevelSet3D()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_Panel.OnLevelSet3D();
}

void CMainFrame::OnVolRender()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	OnButtonSeg();
}

void CMainFrame::OnGrowing()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_Panel.OnDynamicRegionGrowing();	
}
