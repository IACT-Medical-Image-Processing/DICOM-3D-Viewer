
// DICOM Viewer.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//
#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"

#include "DICOM Viewer.h"
#include "MainFrm.h"
#include "FormView1.h"
#include "FormView2.h"
#include "FormView3.h"
#include "FormView4.h"

#include "DICOM ViewerDoc.h"
#include "DICOM ViewerView.h"
#include "Histogram.h"
#include "LoadingDlg.h"
#include "RawDataProcessor.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkImageRegionConstIterator.h"
#include "itkPhasedArray3DSpecialCoordinatesImage.h"
#include "itkMetaDataObject.h"

#include "Scale.h"
#include "DICOM Viewer.h"
#include "DICOM ViewerDoc.h"

#include <iostream>
#include <string>
#include <process.h>

#define MAX_TOK 10      // 토큰으로 분리된 string배열의 최대값

using namespace std;
using namespace Concurrency;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDICOMViewerApp:
// 이 클래스의 구현에 대해서는 DICOM Viewer.cpp을 참조하십시오.
//
//using namespace cv;

// CDICOMViewerApp

BEGIN_MESSAGE_MAP(CDICOMViewerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CDICOMViewerApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CDICOMViewerApp::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_SAVE_STL, &CDICOMViewerApp::OnSaveStl)
	ON_COMMAND(ID_READ_STL, &CDICOMViewerApp::OnReadStl)
END_MESSAGE_MAP()

int comp(wstring a, wstring b) 
{
  std::locale loc;

  int start1 = a.rfind('\\');
  int end1 = a.rfind('.');
  for(int i = start1; i < end1; i++)
  {
	  if(isdigit(a[i],loc) == false)
	  {
		  start1++;
	  }
  }
  int size1 = end1 - start1;
  wstring n1 = a.substr(start1, size1);
  
  int start2 = b.rfind('\\');
  int end2 = b.rfind('.');
  for(int i = start2; i < end2; i++)
  {
	  if(isdigit(b[i],loc) == false)
	  {
		  start2++;
	  }
  }
  int size2 = end2 - start2;
  wstring n2 = b.substr(start2, size2);

  int value1 = (int) _wtoi( n1.c_str() );
  int value2 = (int) _wtoi( n2.c_str() );

  return (value1 < value2);
}

// CDICOMViewerApp 생성

CDICOMViewerApp::CDICOMViewerApp()
{
	m_bHiColorIcons = TRUE;

	// 다시 시작 관리자 지원
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DICOM Viewer.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CDICOMViewerApp 개체입니다.

CDICOMViewerApp theApp;


// CDICOMViewerApp 초기화

BOOL CDICOMViewerApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDICOMViewerDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CDicomView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	// 끌어서 놓기에 대한 열기를 활성화합니다.
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

int CDICOMViewerApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CDICOMViewerApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CDICOMViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDICOMViewerApp 사용자 지정 로드/저장 메서드

void CDICOMViewerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	//GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CDICOMViewerApp::LoadCustomState()
{
}

void CDICOMViewerApp::SaveCustomState()
{
}

BOOL CDICOMViewerApp::SearchDirectory(CString& strFolder)
{
    CFileFind file;
	BOOL b = file.FindFile(strFolder + _T("\\*.dcm"));
	
	CString strFolderItem;
	CString strFileExt;
	CString strTempString;

	m_vFileList.clear();

	while(b)
	{
		b = file.FindNextFileW();
		if(file.IsDirectory()&&!file.IsDots())
		{
			strFolderItem = file.GetFilePath();
			SearchDirectory(strFolderItem);
		}
		strFolderItem = file.GetFilePath();
		strFileExt = strFolderItem.Mid(strFolderItem.ReverseFind('.'));

		if(!file.IsDots())
		{
			strFileExt.MakeUpper();
			if(file.IsDirectory())
				continue;

			wstring folder_name;
			folder_name = strFolderItem;
			m_vFileList.push_back(folder_name);
		}
	}
	
	file.Close();

	sort(m_vFileList.begin(), m_vFileList.end(), comp);

    return TRUE;
}

int CALLBACK CDICOMViewerApp::BrowseCallbakProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM pData)
{
    switch(uMsg)
    {
    case BFFM_VALIDATEFAILED:
        CString strText;
        strText.Format(_T("%s"), reinterpret_cast<LPTSTR>(lParam));
        AfxMessageBox((LPCTSTR)strText);
        break;
    }

	return 0;
}

HRESULT CDICOMViewerApp::SHPathToPidl(LPWSTR path, LPITEMIDLIST* ppidl) 
{
    LPSHELLFOLDER pShellFolder = NULL;
    OLECHAR wszPath[MAX_PATH] = {0};
    ULONG nCharsParsed = 0;
          
    HRESULT hr = SHGetDesktopFolder(&pShellFolder);
          
    if(FAILED(hr)) return FALSE;
          
    hr = pShellFolder->ParseDisplayName(NULL, NULL, path, &nCharsParsed, ppidl, NULL);
    pShellFolder->Release();
          
    return hr;
}

// CDICOMViewerApp 메시지 처리기
void CDICOMViewerApp::UpdateView()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	if(pDoc->ReadVolumeFile( 512, 512, m_vFileList.size() ) == false)
	{
		AfxMessageBox( _T( "Failed to read the data" ));
	}

	CFormView1* pView1 = (CFormView1*)(((CMainFrame *)AfxGetMainWnd())->GetView1());
	CFormView2* pView2 = (CFormView2*)(((CMainFrame *)AfxGetMainWnd())->GetView2());
	CFormView3* pView3 = (CFormView3*)(((CMainFrame *)AfxGetMainWnd())->GetView3());
	CFormView4* pView4 = (CFormView4*)(((CMainFrame *)AfxGetMainWnd())->GetView4());

	pView1->SetCurrentSize(pDoc->GetFileNumber());
	pView2->SetCurrentSize(512);
	pView3->SetCurrentSize(512);

	pView1->Invalidate(FALSE);
	pView2->Invalidate(FALSE);
	pView3->Invalidate(FALSE);
	pView4->Invalidate(FALSE);

	((CMainFrame *)AfxGetMainWnd())->GetPanel()->Invalidate(FALSE);
}

void CDICOMViewerApp::OnFileOpen()
{
	//char szFilter[] = "DICOM (*.dcm) | *.dcm | All Files(*.*)|*.*||";

	//CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL);

	//if(IDOK == dlg.DoModal()) 
	//{
	//	CString strPathName = dlg.GetPathName(); 
	//}

	ITEMIDLIST *pildBrowse;
	WCHAR pathName[MAX_PATH];
	      
	LPITEMIDLIST pidl = NULL;
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(BROWSEINFO));
	      
	SHPathToPidl(CSIDL_DESKTOP, &pidl);       // 루트 디렉토리 설정 부분.
	
	HWND hWnd = GetMainWnd()->GetSafeHwnd();
	bInfo.hwndOwner = hWnd;
	bInfo.pidlRoot = pidl;
	bInfo.pszDisplayName = pathName;
	bInfo.lpszTitle = _T("검색할 디렉토리를 선택해주십시요.");
	bInfo.lpfn = BrowseCallbakProc;
	// 콜백함수가 필요없다면 NULL이나 지움. 위의 static int 함수 필요 없어짐
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE ;
	
	// 디렉토리만 설정가능하도록 하였습니다... 플래그 설정은 맨 아래 참조하세요, 또는 MSDN
	
	pildBrowse = (ITEMIDLIST*)SHBrowseForFolder(&bInfo);
	      
	if( pildBrowse != NULL )
	{
	    SHGetPathFromIDList(pildBrowse, pathName);
		
		CString cs;
		cs.Format(_T("%s"), pathName);
	    SearchDirectory(cs);		
		
		//LoadingDlg loading_dlg;
		//loading_dlg.Create(IDD_LOAD_DIALOG,AfxGetMainWnd());
		//loading_dlg.CenterWindow();
		//loading_dlg.ShowWindow(SW_SHOW);
		
		CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
		if(!pDoc)
			return;
		
		pDoc->SetFileNumber(m_vFileList.size());
		pDoc->Init();
		pDoc->m_HUmax = 0;
		pDoc->m_HUmin = 0;
		pDoc->m_bCalcMean = false;
		pDoc->IsVolumeDone = true;
		pDoc->m_bSegFirst = true;


		int file_size =pDoc->GetFileNumber();

		HANDLE hand1 = (HANDLE)_beginthreadex(NULL,0, ReadImagePart1, (void*)pDoc,0,NULL);
		Sleep(100);
		HANDLE hand2 = (HANDLE)_beginthreadex(NULL,0, ReadImagePart2, (void*)pDoc,0,NULL);
		Sleep(100);
		HANDLE hand3 = (HANDLE)_beginthreadex(NULL,0, ReadImagePart3, (void*)pDoc,0,NULL);
		Sleep(100);
		HANDLE hand4 = (HANDLE)_beginthreadex(NULL,0, ReadImagePart4, (void*)pDoc,0,NULL);

		//for(int i = 0; i < pDoc->GetFileNumber(); i++)
		//{
		//	ReadImage(i);
		//	loading_dlg.SetFileCount(i);
		//	loading_dlg.Processing();
		//	loading_dlg.UpdateWindow();
		//}
		WaitForSingleObject(hand1, INFINITE);
		WaitForSingleObject(hand2, INFINITE);
		WaitForSingleObject(hand3, INFINITE);
		WaitForSingleObject(hand4, INFINITE);

		//DWORD test = 0;
		//GetExitCodeThread(hand, &test);
		CloseHandle(hand1);
		CloseHandle(hand2);
		CloseHandle(hand3);
		CloseHandle(hand4);

		DICOMtoImage(pDoc->m_HUmin, pDoc->m_HUmax);
		MakeHistogram();

		//loading_dlg.DestroyWindow();

		pDoc->DataLoaded(true);

		theApp.UpdateView();
	}
}

string CDICOMViewerApp::ConvertUnicodeToMultibyte(CString strUnicode)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);

	char* pMultibyte = new char[nLen];
	memset(pMultibyte, 0x00, (nLen)*sizeof(char));

	WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, pMultibyte, nLen, NULL, NULL);

	string strMulti = pMultibyte;

	delete [] pMultibyte;

	return strMulti;
}

std::string FindDicomTag( const std::string & entryId, const itk::GDCMImageIO::Pointer dicomIO )
{
	std::string tagvalue;
	bool found = dicomIO->GetValueFromTag(entryId, tagvalue);
	if ( !found )
	{
		tagvalue = "NOT FOUND";
    }
	
	return tagvalue;
}

string* StringSplit(string strOrigin, string strTok)
{
    int     cutAt;                            //자르는위치
    int     index     = 0;                    //문자열인덱스
    string* strResult = new string[MAX_TOK];  //결과return 할변수

    //strTok을찾을때까지반복
    while ((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
    {
        if (cutAt > 0)  //자르는위치가0보다크면(성공시)
        {
            strResult[index++] = strOrigin.substr(0, cutAt);  //결과배열에추가
        }
        strOrigin = strOrigin.substr(cutAt+1);  //원본은자른부분제외한나머지
    }

    if(strOrigin.length() > 0)  //원본이아직남았으면
    {
        strResult[index++] = strOrigin.substr(0, cutAt);  //나머지를결과배열에추가
    }

    return strResult;  //결과return
}

UINT CDICOMViewerApp::ReadImagePart1(void* arg)
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc*)arg;

	for(int i = 0; i < pDoc->GetFileNumber()/4; i++)
	{
		CDICOMViewerApp::MultiTheadReadImage(i, pDoc);
	}

	return 0;
}

UINT CDICOMViewerApp::ReadImagePart2(void* arg)
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc*)arg;

	for(int i = pDoc->GetFileNumber()/4; i < pDoc->GetFileNumber()/2; i++)
	{
		CDICOMViewerApp::MultiTheadReadImage(i, pDoc);
	}

	return 0;
}

UINT CDICOMViewerApp::ReadImagePart3(void* arg)
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc*)arg;

	for(int i = pDoc->GetFileNumber()/2; i < pDoc->GetFileNumber()/4*3; i++)
	{
		CDICOMViewerApp::MultiTheadReadImage(i, pDoc);
	}

	return 0;
}

UINT CDICOMViewerApp::ReadImagePart4(void* arg)
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc*)arg;

	for(int i = pDoc->GetFileNumber()/4*3; i < pDoc->GetFileNumber(); i++)
	{
		CDICOMViewerApp::MultiTheadReadImage(i, pDoc);
	}

	return 0;
}

void CDICOMViewerApp::MultiTheadReadImage(int _image_no, CDICOMViewerDoc* pDoc)
{
	int x, y;
		
	//CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;
	// File Read ------------------------------------------------------

	typedef signed short InputPixelType;
	const unsigned int	 InputDimension = 2;		
	typedef itk::Image< InputPixelType, InputDimension > InputImageType;
	typedef itk::ImageFileReader< InputImageType > ReaderType;	

	ReaderType::Pointer reader = ReaderType::New();
	wstring wFileName = theApp.m_vFileList[_image_no];
	string strFileName = theApp.ConvertUnicodeToMultibyte(wFileName.c_str());
	reader->SetFileName(strFileName);
		
	typedef itk::GDCMImageIO		   ImageIOType;
	ImageIOType::Pointer gdcmImageIO = ImageIOType::New();

	//pDoc->spacing_x = gdcmImageIO->GetSpacing(0);
	//pDoc->spacing_y = gdcmImageIO->GetSpacing(1);
	//pDoc->spacing_z = gdcmImageIO->GetSpacing(2);

	reader->SetImageIO( gdcmImageIO );
	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject & e)
	{
		std::cerr << "exception in file reader " << std::endl;
		std::cerr << e << std::endl;
		return;
	}

	//static std::string pixel_size = "";
	//static std::string pos0 = "";
	//static std::string posn = "";
	//static string* xyz = NULL;
	//static string* pixel = NULL;
	//static double z0;
	//static double zn;
	//static double pixelWidth = -1.0;
	//static double pixelHeight = -1.0;
	//static double voxelDepth = -1.0;

	//if(_image_no == 0)
	//{	
	//	pixel_size  = FindDicomTag("0028|0030", gdcmImageIO);
	//	pixel = StringSplit(pixel_size,"\\");

	//	pDoc->m_pixelWidth = (double)(atof(pixel[0].c_str()));
	//	pDoc->m_pixelHeight = (double)(atof(pixel[1].c_str()));

	//	pos0  = FindDicomTag("0020|0032", gdcmImageIO);
	//	xyz = StringSplit(pos0,"\\\\");

	//	z0 = (double)(atof(xyz[2].c_str()));

	//	pDoc->m_person_name  = FindDicomTag("0040|A043", gdcmImageIO);
	//	pDoc->m_date  = FindDicomTag("004|A121", gdcmImageIO);
	//	
	//}

	//if(_image_no == (pDoc->GetFileNumber()-1))
	//{
	//	posn = FindDicomTag("0020|0032", gdcmImageIO);
	//	xyz = StringSplit(posn,"\\\\");
	//	
	//	zn = (double)(atof(xyz[2].c_str()));

	//	pDoc->m_voxelDepth = abs((zn - z0) / (pDoc->GetFileNumber() -1));
	//}

	// Copy to Volume -------------------------------------------------
	InputImageType::Pointer image = reader->GetOutput();

	itk::ImageRegionConstIterator<InputImageType> imageIterator(image,image->GetRequestedRegion());
	
	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 >        ImageType;

	imageIterator = imageIterator.Begin();
	int value;

	for (y=0; y<512; y++)
	{
		for (x=0; x<512; x++)
		{
			value = imageIterator.Get();
			
			if (_image_no == 0 && x==0 && y==0)
				theApp.m_bgLevel = value + 100;
				
			m_DicomData[_image_no][y][x] = value;

			if (value > pDoc->m_HUmax)	pDoc->m_HUmax = value;
			if (value > theApp.m_bgLevel && value < pDoc->m_HUmin)		pDoc->m_HUmin = value;
			imageIterator++;
		}
	}

	// Scale change  ------------------------------------------------------
#if 0
	typedef unsigned char DataType;
	typedef itk::Image< DataType, 2 > DataImageType;
	typedef itk::RescaleIntensityImageFilter< InputImageType, DataImageType > RescaleFilterType;
	RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

	rescaler->SetOutputMinimum( 0 );
	rescaler->SetOutputMaximum( 255 );
	rescaler->SetInput( reader->GetOutput() );
	rescaler->Update();

	// Copy to Volume -------------------------------------------------

	Mat imgIn = itk::OpenCVImageBridge::ITKImageToCVMat< DataImageType >( rescaler->GetOutput() );

	for (y=0; y<512; y++)
	{
		for (x=0; x<512; x++)
		{
			pDoc->m_Volume[_image_no][y][x] = imgIn.at<unsigned char>(y,x);
			int index = pDoc->m_Volume[_image_no][y][x]; 
			pDoc->m_fHistogram[index]++;
		}
	}
#endif
}

void CDICOMViewerApp::ReadImage(int _image_no)
{
	int x, y;

	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;
	// File Read ------------------------------------------------------

	typedef signed short InputPixelType;
	const unsigned int	 InputDimension = 2;		
	typedef itk::Image< InputPixelType, InputDimension > InputImageType;
	typedef itk::ImageFileReader< InputImageType > ReaderType;

	ReaderType::Pointer reader = ReaderType::New();
	wstring wFileName = theApp.m_vFileList[_image_no];
	string strFileName = theApp.ConvertUnicodeToMultibyte(wFileName.c_str());
	reader->SetFileName(strFileName);

	typedef itk::GDCMImageIO		   ImageIOType;
	ImageIOType::Pointer gdcmImageIO = ImageIOType::New();

	pDoc->spacing_x = gdcmImageIO->GetSpacing(0);
	pDoc->spacing_y = gdcmImageIO->GetSpacing(1);
	pDoc->spacing_z = gdcmImageIO->GetSpacing(2);

	reader->SetImageIO( gdcmImageIO );
	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject & e)
	{
		std::cerr << "exception in file reader " << std::endl;
		std::cerr << e << std::endl;
		return;
	}

	static std::string pixel_size = "";
	static std::string pos0 = "";
	static std::string posn = "";
	static string* xyz;
	static string* pixel;
	static double z0;
	static double zn;
	static double pixelWidth = -1.0;
	static double pixelHeight = -1.0;
	static double voxelDepth = -1.0;

	if(_image_no == 0)
	{	
		pixel_size  = FindDicomTag("0028|0030", gdcmImageIO);
		pixel = StringSplit(pixel_size,"\\");

		pDoc->m_pixelWidth = (double)(atof(pixel[0].c_str()));
		pDoc->m_pixelHeight = (double)(atof(pixel[1].c_str()));

		pos0  = FindDicomTag("0020|0032", gdcmImageIO);
		xyz = StringSplit(pos0,"\\\\");

		z0 = (double)(atof(xyz[2].c_str()));
	}

	if(_image_no == (pDoc->GetFileNumber()-1))
	{
		posn = FindDicomTag("0020|0032", gdcmImageIO);
		xyz = StringSplit(posn,"\\\\");

		zn = (double)(atof(xyz[2].c_str()));

		pDoc->m_voxelDepth = abs((zn - z0) / (pDoc->GetFileNumber() -1));
	}

	// Copy to Volume -------------------------------------------------
	InputImageType::Pointer image = reader->GetOutput();

	itk::ImageRegionConstIterator<InputImageType> imageIterator(image,image->GetRequestedRegion());

	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 >        ImageType;

	imageIterator = imageIterator.Begin();
	int value;

	for (y=0; y<512; y++)
	{
		for (x=0; x<512; x++)
		{
			value = imageIterator.Get();

			if (_image_no == 0 && x==0 && y==0)
				theApp.m_bgLevel = value + 100;

			m_DicomData[_image_no][y][x] = value;
			if (value > pDoc->m_HUmax)	pDoc->m_HUmax = value;
			if (value > theApp.m_bgLevel && value < pDoc->m_HUmin)		pDoc->m_HUmin = value;
			imageIterator++;
		}
	}

	// Scale change  ------------------------------------------------------
#if 0
	typedef unsigned char DataType;
	typedef itk::Image< DataType, 2 > DataImageType;
	typedef itk::RescaleIntensityImageFilter< InputImageType, DataImageType > RescaleFilterType;
	RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

	rescaler->SetOutputMinimum( 0 );
	rescaler->SetOutputMaximum( 255 );
	rescaler->SetInput( reader->GetOutput() );
	rescaler->Update();

	// Copy to Volume -------------------------------------------------

	Mat imgIn = itk::OpenCVImageBridge::ITKImageToCVMat< DataImageType >( rescaler->GetOutput() );

	for (y=0; y<512; y++)
	{
		for (x=0; x<512; x++)
		{
			pDoc->m_Volume[_image_no][y][x] = imgIn.at<unsigned char>(y,x);
			int index = pDoc->m_Volume[_image_no][y][x]; 
			pDoc->m_fHistogram[index]++;
		}
	}
#endif
}

void CDICOMViewerApp::MakeHistogram()
{
	int x, y, z, value;
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	for (z=0; z<pDoc->GetFileNumber(); z++)
		for (y=0; y<512; y++)
			for (x=0; x<512; x++)
			{
				pDoc->m_pfHistogram[m_Volume[z][y][x]]++;
			//	value = m_Volume[z][y][x];
			//	pDoc->m_pfHistogram[DICOMtoImage(value, pDoc->m_HUmin, pDoc->m_HUmax)]++;
			}
}

void CDICOMViewerApp::OnSaveStl()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument(); 
	if(!pDoc)
		return;

	wchar_t szFilter[] = L"STL file (*.stl) | *.STL | All Files(*.*)|*.*||";

	CFileDialog dlg(FALSE, L"*.stl", L"*.stl", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	if(IDOK == dlg.DoModal()) 
	{
		CString strPathName = dlg.GetPathName();
		CString temp = strPathName;
		temp.MakeLower();
		if(temp.Find(L".stl") == -1) // 찾는 문자열이 없을 경우
		{
			strPathName.Append(L".stl");
		}
		pDoc->WriteSTL(strPathName);
	}
}

void CDICOMViewerApp::OnReadStl()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument(); 
	if(!pDoc)
		return;

	wchar_t szFilter[] = L"STL file (*.stl) | *.STL | All Files(*.*)|*.*||";

	CFileDialog dlg(TRUE, L"*.stl", L"*.stl", OFN_HIDEREADONLY, szFilter);

	if(IDOK == dlg.DoModal()) 
	{
		CString strPathName = dlg.GetPathName();
		pDoc->ReadSTL(strPathName);
		CFormView4* pView4 = (CFormView4*)(((CMainFrame *)AfxGetMainWnd())->GetView4());
		pView4->Invalidate(FALSE);
	}
}

