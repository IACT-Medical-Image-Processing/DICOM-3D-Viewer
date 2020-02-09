#pragma once
#include "afxcmn.h"


// LoadingDlg 대화 상자입니다.

class LoadingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LoadingDlg)

public:
	LoadingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LoadingDlg();

	BOOL Processing();

	inline VOID SetFileCount(int _fileCount){ m_nFileCount = _fileCount; }
private:
	int m_nFileCount;
	CProgressCtrl m_progress;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOAD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
