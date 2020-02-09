#pragma once
#include "afxcmn.h"


// LoadingDlg ��ȭ �����Դϴ�.

class LoadingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LoadingDlg)

public:
	LoadingDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LoadingDlg();

	BOOL Processing();

	inline VOID SetFileCount(int _fileCount){ m_nFileCount = _fileCount; }
private:
	int m_nFileCount;
	CProgressCtrl m_progress;
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOAD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
