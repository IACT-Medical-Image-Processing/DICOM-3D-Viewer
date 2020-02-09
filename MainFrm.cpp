
// MainFrm.cpp : CMainFrame Ŭ������ ����
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
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	//_CrtSetBreakAlloc(132);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
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
	// ������ ���� ���� ���־� ������ �� ��Ÿ���� �����մϴ�.
	OnApplicationLook(theApp.m_nAppLook);

	m_bAutoMenuEnable = FALSE;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("�޴� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// �޴� ������ Ȱ��ȭ�ص� ��Ŀ���� �̵����� �ʰ� �մϴ�.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	// ����� ���� ���� ���� �۾��� ����մϴ�.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	m_wndMenuBar.EnableMenuShadows(FALSE);
	// TODO: ���� ���� �� �޴� ������ ��ŷ�� �� ���� �Ϸ��� �� �ټ� ���� �����Ͻʽÿ�.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// Visual Studio 2005 ��Ÿ�� ��ŷ â ������ Ȱ��ȭ�մϴ�.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 ��Ÿ�� ��ŷ â �ڵ� ���� ������ Ȱ��ȭ�մϴ�.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// ���� ���� �� ��ŷ â �޴� �ٲٱ⸦ Ȱ��ȭ�մϴ�.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ����(<Alt> Ű�� ���� ä ����) ���� ���� ����� ������ Ȱ��ȭ�մϴ�.
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// ����� ���� ���� ���� �̹����� �ε��մϴ�.
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// �޴� ���� ������ Ȱ��ȭ�մϴ�(���� �ֱٿ� ����� ���).
	// TODO: ������� �⺻ ����� �����Ͽ� �� Ǯ�ٿ� �޴��� �ϳ� �̻��� �⺻ ����� ���Խ�ŵ�ϴ�.
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
	// ������
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
	WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI;

	CString strCaption(_T("������"));
	if(!m_Panel.Create(strCaption, this, CRect(0,0,300,300),TRUE,(UINT)1,dwStyle))
	{
		TRACE(_T("������ ���� ����"));
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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	cs.dwExStyle &= ~WS_CLIPCHILDREN;

	return TRUE;
}

// CMainFrame ����

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


// CMainFrame �޽��� ó����

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* �޴��� �˻��մϴ�. */);
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
	// �⺻ Ŭ������ ���� �۾��� �����մϴ�.

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// ��� ����� ���� ������ ����� ���� ���߸� Ȱ��ȭ�մϴ�.
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
 
	// �Ʒ��� �¿� �� ����
	CSize size1(MulDiv(rect.Width()-300, 50, 100), MulDiv(rect.Height(), 50, 100)); // ��(30%), ����(50%)
	CSize size2(MulDiv(rect.Width()-300, 50, 100), MulDiv(rect.Height(), 50, 100)); // ��(70%), ����(50%)

  // �Ʒ��� ���� �� ���� (����)
  //CSize size1(MulDiv(rect.Width(), 50, 100), ::GetSystemMetrics(SM_CXSCREEN)); // ����(50%)
 
  // create a splitter with 2 row, 2 columns 
  if (!m_wndSplitter.CreateStatic(this, 2, 2, WS_CHILD | WS_VISIBLE)) { 
      TRACE0("Failed to CreateStatic Splitter \n"); 
      return FALSE; 
  }
 
  // ����ڰ� ���� CFormView1 �� ������ ��ġ�մϴ�... 
  if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CFormView1), size1, pContext)) { 
      TRACE0("Failed to create CFormView1 pane \n"); 
      return FALSE; 
  }
 
  // ����ڰ� ���� CFormView2 �� ������ ��ġ�մϴ�... 
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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
 
	// �Ʒ��� �¿� �� ����
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	return CFrameWndEx::OnEraseBkgnd(pDC);
}


void CMainFrame::OnButtonSeg()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	//Destroy();

//	test1.Destroy();

	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	pDoc->Destroy();

	CFrameWndEx::OnClose();
}


void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_Panel.OnLButtonDown(nFlags,point);
	CFrameWndEx::OnLButtonDown(nFlags, point);
}


void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_Panel.OnLButtonUp(nFlags,point);
	CFrameWndEx::OnLButtonUp(nFlags, point);
}


void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_Panel.OnMouseMove(nFlags,point);
	CFrameWndEx::OnMouseMove(nFlags, point);
}


void CMainFrame::OnButtonVol()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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

	// ��4�� OnSize�� ȣ���ϱ� ���� �ڵ�
	CRect rc;
	m_pView4->GetClientRect(rc);
	m_pView4->SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	m_pView4->Invalidate(TRUE);
	theApp.UpdateView();
}

void CMainFrame::OnDefaultView()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	maxwindow_type = 0;
	
	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}


void CMainFrame::OnMaxView1()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	maxwindow_type = 1;
	
	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}


void CMainFrame::OnMaxView2()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	maxwindow_type = 2;

	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}


void CMainFrame::OnMaxView3()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	maxwindow_type = 3;

	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}


void CMainFrame::OnMaxView4()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	maxwindow_type = 4;

	CRect rc;
	GetClientRect(&rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));

	Invalidate(FALSE);
}



void CMainFrame::OnThreshold()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	m_Panel.OnThreshold();
}


void CMainFrame::OnAThreshold()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	m_Panel.OnAThreshold();
}


void CMainFrame::OnGraphCut()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	m_Panel.OnGraphCut();
}


void CMainFrame::OnLevelSet2D()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	m_Panel.OnLevelSet2D();
}

void CMainFrame::OnLevelSet3D()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	m_Panel.OnLevelSet3D();
}

void CMainFrame::OnVolRender()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	OnButtonSeg();
}

void CMainFrame::OnGrowing()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	m_Panel.OnDynamicRegionGrowing();	
}
