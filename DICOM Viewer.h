
// DICOM Viewer.h : DICOM Viewer ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include <vector>
#include <string>
#include <algorithm>
#include <locale>

using namespace std;

class CDICOMViewerDoc;

class CDICOMViewerApp : public CWinAppEx
{
public:
	CDICOMViewerApp();

	BOOL SearchDirectory(CString& strFolder);
	virtual void OnFileOpen();
	HRESULT SHPathToPidl(LPWSTR path, LPITEMIDLIST* ppidl);
	static int CALLBACK BrowseCallbakProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM pData);

	static UINT ReadImagePart1(void* arg);
	static UINT ReadImagePart2(void* arg);
	static UINT ReadImagePart3(void* arg);
	static UINT ReadImagePart4(void* arg);

	static void MultiTheadReadImage(int _image_no, CDICOMViewerDoc* pDoc);

	void ReadImage(int _image_no);
	void MakeHistogram();
	void UpdateView();

	vector<wstring> m_vFileList;
	int m_nFileReadCount;

private:
	string ConvertUnicodeToMultibyte(CString strUnicode);
	

	// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	int m_bgLevel;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	
	afx_msg void OnSaveStl();
	afx_msg void OnReadStl();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDICOMViewerApp theApp;
