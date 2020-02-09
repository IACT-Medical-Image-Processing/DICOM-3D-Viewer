// LoadingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DICOM Viewer.h"
#include "LoadingDlg.h"
#include "afxdialogex.h"


// LoadingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(LoadingDlg, CDialogEx)

LoadingDlg::LoadingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(LoadingDlg::IDD, pParent)
{
	m_nFileCount = 0;
}

LoadingDlg::~LoadingDlg()
{
}

void LoadingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOAD_FILE_PROGRESS, m_progress);
}


BEGIN_MESSAGE_MAP(LoadingDlg, CDialogEx)
END_MESSAGE_MAP()


// LoadingDlg 메시지 처리기입니다.

BOOL LoadingDlg::Processing()
{
	int file_size = (int)((CDICOMViewerApp*)AfxGetApp())->m_vFileList.size();
	m_progress.SetRange(0,file_size);

	if(m_nFileCount == file_size)
		return TRUE;

	CString strFileReadNum;
	strFileReadNum.Format(L"%d/%d",m_nFileCount,file_size);
	SetDlgItemText(IDC_LOAD_FILE_NUM, strFileReadNum.GetBuffer(0));
	m_progress.SetPos(m_nFileCount);

	return FALSE;
}