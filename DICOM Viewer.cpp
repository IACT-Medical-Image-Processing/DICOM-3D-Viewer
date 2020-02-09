
// DICOM Viewer.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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

#define MAX_TOK 10      // ��ū���� �и��� string�迭�� �ִ밪

using namespace std;
using namespace Concurrency;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDICOMViewerApp:
// �� Ŭ������ ������ ���ؼ��� DICOM Viewer.cpp�� �����Ͻʽÿ�.
//
//using namespace cv;

// CDICOMViewerApp

BEGIN_MESSAGE_MAP(CDICOMViewerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CDICOMViewerApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CDICOMViewerApp::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
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

// CDICOMViewerApp ����

CDICOMViewerApp::CDICOMViewerApp()
{
	m_bHiColorIcons = TRUE;

	// �ٽ� ���� ������ ����
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DICOM Viewer.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CDICOMViewerApp ��ü�Դϴ�.

CDICOMViewerApp theApp;


// CDICOMViewerApp �ʱ�ȭ

BOOL CDICOMViewerApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDICOMViewerDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CDicomView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// DDE Execute ���⸦ Ȱ��ȭ�մϴ�.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	//  SDI ���� ���α׷������� ProcessShellCommand �Ŀ� �̷��� ȣ���� �߻��ؾ� �մϴ�.
	// ��� ���⿡ ���� ���⸦ Ȱ��ȭ�մϴ�.
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

int CDICOMViewerApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CDICOMViewerApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CDICOMViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDICOMViewerApp ����� ���� �ε�/���� �޼���

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

// CDICOMViewerApp �޽��� ó����
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
	      
	SHPathToPidl(CSIDL_DESKTOP, &pidl);       // ��Ʈ ���丮 ���� �κ�.
	
	HWND hWnd = GetMainWnd()->GetSafeHwnd();
	bInfo.hwndOwner = hWnd;
	bInfo.pidlRoot = pidl;
	bInfo.pszDisplayName = pathName;
	bInfo.lpszTitle = _T("�˻��� ���丮�� �������ֽʽÿ�.");
	bInfo.lpfn = BrowseCallbakProc;
	// �ݹ��Լ��� �ʿ���ٸ� NULL�̳� ����. ���� static int �Լ� �ʿ� ������
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE ;
	
	// ���丮�� ���������ϵ��� �Ͽ����ϴ�... �÷��� ������ �� �Ʒ� �����ϼ���, �Ǵ� MSDN
	
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
    int     cutAt;                            //�ڸ�����ġ
    int     index     = 0;                    //���ڿ��ε���
    string* strResult = new string[MAX_TOK];  //���return �Һ���

    //strTok��ã���������ݺ�
    while ((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
    {
        if (cutAt > 0)  //�ڸ�����ġ��0����ũ��(������)
        {
            strResult[index++] = strOrigin.substr(0, cutAt);  //����迭���߰�
        }
        strOrigin = strOrigin.substr(cutAt+1);  //�������ڸ��κ������ѳ�����
    }

    if(strOrigin.length() > 0)  //�����̾�����������
    {
        strResult[index++] = strOrigin.substr(0, cutAt);  //������������迭���߰�
    }

    return strResult;  //���return
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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
		if(temp.Find(L".stl") == -1) // ã�� ���ڿ��� ���� ���
		{
			strPathName.Append(L".stl");
		}
		pDoc->WriteSTL(strPathName);
	}
}

void CDICOMViewerApp::OnReadStl()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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

