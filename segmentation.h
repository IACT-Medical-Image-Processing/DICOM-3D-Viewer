

#include "stdio.h"
#include <math.h>
#include <cstdio>
#include <vector>

#include "MarchingCube.h"

#if 0
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkShapeDetectionLevelSetImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesWriter.h"
#include "itkNumericSeriesFileNames.h"
#include "itkRescaleIntensityImageFilter.h"
#endif

#define	__ITERATION__
//#define __INTERPOLATION__

#define sizex	512
#define sizey	512
#define sizexy sizex*sizey
#define MaxElementNo	10000000
#define NoOfVector		MaxElementNo/1000000
#define sigmalevel	66
#define WinSize		19	// Odd value
#define HWinSize		(int)((WinSize-1)/2)
#define BlockSize		9

enum SEG_MODE
{
	NONE = 0,
	THRESHOLD,
	ATHRESHOLD,
	GRAPHCUT,
	DYNAMICREGIONGROWING,
	LEVELSET_2D,
	LEVELSET_3D,
};

#pragma once

typedef struct index_T
{	
	bool IsObject		: 1;
	bool IsBoundary		: 1;
	bool IsBackground	: 1;
	bool Seed			: 1;
	bool TempObject		: 1;
} tIndex;

using namespace std;

#if 0
class CSegmentation
{
protected:
	
private:
#endif
//	int ***m_Voxel;
//	int ***m_InputImg;
//	int ***m_Thres;
//	tIndex ***m_Index;
//	double m_Gauss[WinSize][WinSize][WinSize];
//	double m_Gauss2D[WinSize][WinSize];
	
//	vector<vPoint> m_CurrentBoundary, m_NextBoundary;
//	vector<vPoint> m_CurBound[NoOfVector], m_NextBound[NoOfVector];
//	unsigned int m_CurBoundNo, m_NextBoundNo;
//	unsigned int m_CurBoundMax, m_NextBoundMax;

//	int m_IterationCnt;
//	int m_HistoMin, m_HistoMax;
//	int m_CheckRange;
//	int m_TargetValue;
//	int m_TargetValueMin;
//	int m_TargetValueMax;
//	int m_MinThres, m_MaxThres;
//	int m_MaxIterationNo;
//	int m_BgThres;
//	int m_sizez;
//	int m_HUmin;
//	int m_HUmax;
//	int m_HWinSizeZ;
//	double m_ObjectMean;
//	double m_ObjectVariable;
//	SEG_MODE m_SegMode;
//	int m_currentZ;

//	void currentBoundary_clear();
//	void nextBoundary_clear();
//	void currentBoundary_reserve();
//	void nextBoundary_reserve();
//	void currentBoundary_push_back(vPoint point);
//	void nextBoundary_push_back(vPoint point);
//	bool GetCur_IsInside(int vIndex);
//	int GetCur_x(int vIndex);
//	int GetCur_y(int vIndex);
//	int GetCur_z(int vIndex);
//	bool GetNext_IsInside(int vIndex);
//	int GetNext_x(int vIndex);
//	int GetNext_y(int vIndex);
//	int GetNext_z(int vIndex);
	void VectorInit();
//	bool checkUnimodal(double* histogram, int thres);
//	int OtsuThreshold(double* histogram, double total);
	void MakeGaussianKernel();
	void MakeGaussianKernel2D();
//	int GetThreshold3D(int x, int y, int z);
	int GetThreshold2D(int x, int y);
	void MakeVoxelThreshold1();
	void FindInitialArea();
//	void FloodFill();
//	void SetInitialBG();
//	bool FindNextBG();
//	void FillInside();
	void vFindBoundary();
//	int vApplyThreshold1();
	void ReadyForNextIteration();
	void IterationProcess();
	void InitSegmentation();
	void StoreSegmentationResult();
	void DeleteMemory();
	void Thresholding();
	void AdaptiveThreshold3D();
//	void MakeVoxel2D();
	void MakeVoxel3D();
	void GraphCutSegmentation();
	void DynamicRegionGrowing();
	void InitDynamicRegionGrowing();
	void IterationProcessDRG();	
	void vFindBoundaryDRG();	
	int vApplyThresholdDRG();	
	int GetObjectMean(int x, int y, int z);
	void LevelSet2D();
	void LevelSet3D();
	
//public:
//	CSegmentation();
//	virtual ~CSegmentation();
	void Destroy();
	void Segmentation(SEG_MODE SegMode, int StartLevel, int Zsize, vector<tTri> *triangles, int min, int max, int currentZ);
	
//};
