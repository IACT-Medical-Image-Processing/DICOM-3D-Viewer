
// DICOM ViewerDoc.h : CDICOMViewerDoc Ŭ������ �������̽�
//

#include "segmentation.h"
#include "MarchingCube.h"

#pragma once

class CRawDataProcessor;
class CTranformationMgr;

extern signed short m_DicomData[512][512][512];
extern unsigned char m_Volume[512][512][512];
//extern tIndex m_Index[512][512][512];
extern unsigned char m_Object[512][512][512];
extern unsigned char m_Boundary[512][512][512];
extern unsigned char m_Seed[512][512][512];
extern unsigned char m_Voxel[512][512][512];
extern unsigned char m_TempObject[512][512][512];

class CDICOMViewerDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CDICOMViewerDoc();
	DECLARE_DYNCREATE(CDICOMViewerDoc)

// Ư���Դϴ�.
public:
	VOID Init();
	VOID Destroy();
	inline VOID DataLoaded(BOOL _bLoaded){ m_bDataloaded = _bLoaded; }
	inline BOOL IsDataLoaded() { return m_bDataloaded; }

	inline VOID SetFileNumber(int _CT_file_num){ m_CT_file_num = _CT_file_num; }
	inline UINT GetFileNumber(){ return m_CT_file_num; }
	BOOL WriteSTL(CString _file_name);
	BOOL ReadSTL(CString _file_name);
	bool ReadVolumeFile(int nWidth_i, int nHeight_i, int nDepth_i);
// �۾��Դϴ�.
public:
//	int*** m_pppVolume;
//	tIndex*** m_pppIndex;
//	int*** m_pppVoxel;
	float* m_pfHistogram;
	CRawDataProcessor* m_pRawDataProc;		
	//CTranformationMgr* m_pTransform;

	float m_fMaxHisto;
	int m_startHisto;
	int m_endHisto;
	int m_HUmax, m_HUmin;
	bool IsSegmentationDone;
	vector<tTri> m_triangles;
	bool IsVolumeDone;

	double spacing_x;
	double spacing_y;
	double spacing_z;

	double m_pixelWidth;
	double m_pixelHeight;
	double m_voxelDepth;

	SEG_MODE m_SegMode;
	bool m_bCalcMean;
	bool m_bSegFirst;

	string m_person_name;
	string m_date;
private:
	BOOL m_bDataloaded;
	UINT m_CT_file_num;
	
// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// �����Դϴ�.
public:
	virtual ~CDICOMViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// �˻� ó���⿡ ���� �˻� �������� �����ϴ� ����� �Լ�
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
