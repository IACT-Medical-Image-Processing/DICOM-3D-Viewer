
// DICOM Viewer.h : DICOM Viewer 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
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
	

	// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
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
