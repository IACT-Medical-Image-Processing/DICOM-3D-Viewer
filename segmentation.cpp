
#include "stdafx.h"
#include "segmentation.h"
#include "Scale.h"
#include "graph.h"
#include "block.h"

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

#include "DICOM ViewerDoc.h"

#include <windows.h>


//#define	_MY_DEBUG

using namespace Concurrency;

unsigned char m_Thres[512][512][512];

vector<vPoint> m_CurrentBoundary, m_NextBoundary;
vector<vPoint> m_CurBound[NoOfVector], m_NextBound[NoOfVector];
unsigned int m_CurBoundNo, m_NextBoundNo;
unsigned int m_CurBoundMax, m_NextBoundMax;

double m_Gauss[WinSize][WinSize][WinSize];
double m_Gauss2D[WinSize][WinSize];

int m_IterationCnt;
int m_HistoMin, m_HistoMax;
int m_CheckRange;
int m_TargetValue;
int m_TargetValueMin;
int m_TargetValueMax;
int m_MinThres, m_MaxThres;
int m_MaxIterationNo;
int m_BgThres;
int m_sizez;
int m_HUmin;
int m_HUmax;
int m_HWinSizeZ;
double m_ObjectMean;
double m_ObjectVariable;
SEG_MODE m_SegMode;
int m_currentZ;

void myOutputDebugString(LPCTSTR pszStr, ...)   
{   
#ifdef _MY_DEBUG   
    TCHAR szMsg[256];   
    va_list args;   
    va_start(args, pszStr);   
    _vstprintf_s(szMsg, 256, pszStr, args);   
    OutputDebugString(szMsg);   
#endif   
}  

#if 0
CSegmentation::CSegmentation()
{
	// TODO: 여기에 생성 코드를 추가합니다.
//	VectorInit();

//	m_Voxel = NULL;
//	m_InputImg = NULL;
//	m_Thres = NULL;
//	m_Index = NULL;

}

CSegmentation::~CSegmentation()
{
	Destroy();
}

void CSegmentation::Destroy()
{
/*	int y, z;

	if(m_Voxel != NULL)
	{
		for(z = 0; z < m_sizez; z++)
		{
			for(int y = 0; y < 512; y++)
				delete [] m_Voxel[z][y];
			delete [] m_Voxel[z] ;
		}

		delete [] m_Voxel;
		m_Voxel = NULL;
	}

	if(m_InputImg != NULL)
	{
		for(z = 0; z < m_sizez; z++)
		{
			for(int y = 0; y < 512; y++)
				delete [] m_InputImg[z][y];
			delete [] m_InputImg[z] ;
		}

		delete [] m_InputImg;
		m_InputImg = NULL;
	}

	if(m_Thres != NULL)
	{
		for(z = 0; z < m_sizez; z++)
		{
			for(int y = 0; y < 512; y++)
				delete [] m_Thres[z][y];
			delete [] m_Thres[z] ;
		}
		delete [] m_Thres;
		m_Thres = NULL;
	}

	if(m_Index != NULL)
	{
		for(z = 0; z < m_sizez; z++)
		{
			for(int y = 0; y < 512; y++)
				delete [] m_Index[z][y];
			delete [] m_Index[z] ;
		}

		delete [] m_Index;
		m_Index = NULL;
	}
*/
}
#endif

void currentBoundary_clear()
{
	for (int k=0; k<NoOfVector; k++)
		m_CurBound[k].clear();

	m_CurBoundNo = 0;
}

void nextBoundary_clear()
{
	for (int k=0; k<NoOfVector; k++)
		m_NextBound[k].clear();

	m_NextBoundNo = 0;
}

void currentBoundary_reserve()
{
	for (int k=0; k<NoOfVector; k++)
		m_CurBound[k].reserve(1000000);
}

void nextBoundary_reserve()
{
	for (int k=0; k<NoOfVector; k++)
		m_NextBound[k].reserve(1000000);
}

void currentBoundary_push_back(vPoint point)
{
	int index = (int)(m_CurBoundNo / 1000000);
	m_CurBound[index].push_back(point);
	m_CurBoundNo ++;
}

void nextBoundary_push_back(vPoint point)
{
	int index = (int)(m_NextBoundNo / 1000000);
	m_NextBound[index].push_back(point);
	m_NextBoundNo ++;
}

bool GetCur_IsInside(int vIndex)
{
	int index1 = (int)(vIndex / 1000000);
	int index2 = (int)(vIndex % 1000000);
	return (m_CurBound[index1][index2].IsInside);
}

int GetCur_x(int vIndex)
{
	int index1 = (int)(vIndex / 1000000);
	int index2 = (int)(vIndex % 1000000);
	return (m_CurBound[index1][index2].x);
}

int GetCur_y(int vIndex)
{
	int index1 = (int)(vIndex / 1000000);
	int index2 = (int)(vIndex % 1000000);
	return (m_CurBound[index1][index2].y);
}

int GetCur_z(int vIndex)
{
	int index1 = (int)(vIndex / 1000000);
	int index2 = (int)(vIndex % 1000000);
	return (m_CurBound[index1][index2].z);
}

bool GetNext_IsInside(int vIndex)
{
	int index1 = (int)(vIndex / 1000000);
	int index2 = (int)(vIndex % 1000000);
	return (m_NextBound[index1][index2].IsInside);
}

int GetNext_x(int vIndex)
{
	int index1 = (int)(vIndex / 1000000);
	int index2 = (int)(vIndex % 1000000);
	return (m_NextBound[index1][index2].x);
}

int GetNext_y(int vIndex)
{
	int index1 = (int)(vIndex / 1000000);
	int index2 = (int)(vIndex % 1000000);
	return (m_NextBound[index1][index2].y);
}

int GetNext_z(int vIndex)
{
	int index1 = (int)(vIndex / 1000000);
	int index2 = (int)(vIndex % 1000000);
	return (m_NextBound[index1][index2].z);
}

void VectorInit()
{
	currentBoundary_clear();
	nextBoundary_clear();

	currentBoundary_reserve();
	nextBoundary_reserve();
}

bool checkUnimodal(double* histogram, int thres)
{
	int val = 0;
	int maxP1 = 0;
	int maxP2 = 0;
	int sigmaL = 0;
	int sigmaH = 0;
	int maximum = 0;
	double max1 = -1.0;
	double max2 = -1.0;
	double histogram1[256] = {0.0};
	double Gaussian1[256] = {0.0};
	double sigma1 = 0.0;
	double sigma2 = 0.0;
	double B = 0.0;
	double D = 0.0;
	double level = (double)((sigmalevel)/100.0);
	bool result;
	
	for (val = 2; val <= 253; val++)
		histogram1[val] = (histogram[val-2] + histogram[val-1] + histogram[val] + histogram[val+1] + histogram[val+2])/5;

	for (val = 2; val <= 253; val++)
		histogram[val] = histogram1[val];

	for (val = m_HistoMin; val <= 255; val++)
		if (histogram[val] > max1)
		{
			max1 = histogram[val];
			maxP1 = val;
		}

	// Estimate Mean and Sigma of Gaussian distribution
	int sigma = 0;
	int minsigma = 0;
	double error = 0.0;
	double minError = 10000000000000.0;

	for (sigma = 0; sigma <= 20; sigma++)
	{
		error = 0.0;
		B = 1.0 / (2 * sigma * sigma);
		
		for (val = maxP1-10; val <= maxP1+10; val++)
		{
			if (val < 0 || val > 255)
				continue;
		
			error += abs(histogram[val] - max1 * exp(-B * (val - maxP1)*(val - maxP1)));
		}

		if (error < minError)
		{
			minError = error;
			minsigma = sigma;
		}
	}

	sigma1 = minsigma;

	B = 1.0 / (2 * sigma1 * sigma1);
	
	for (val = 0; val <= 255; val++)
	{
		histogram1[val] = (int)(histogram[val] - max1 * exp(-B * (val - maxP1)*(val - maxP1)));
		if (val >= (maxP1 - 3*sigma1) && val <= (maxP1 + 3*sigma1))
			histogram1[val] = 0;
	}

	for (val = m_HistoMin; val <= 255; val++)
		if (histogram1[val] > max2)
		{
			max2 = histogram1[val];
			maxP2 = val;
		}

	for (val = maxP2; val >= max(maxP2 - m_CheckRange,0); val --)
	{
		if (histogram1[val] <= (max2*level)){
			sigmaL = maxP2 - val;
			break;
		}
	}
	
	for (val = maxP2; val <= (maxP2 + m_CheckRange); val ++)
	{
		if (histogram1[val] <= (max2*level)){
			sigmaH = val - maxP2;
			break;
		}
	}

	sigma2 = min(sigmaL, sigmaH);

	if ((sigma1*sigma2) == 0)
		result = true;
	else
	{
		D = abs(maxP1 - maxP2) / (2 * sqrt(sigma1 * sigma2));

		if ((D <= 1.0) || (max2 *10 < max1))
			result = true;
		else
			result = false;
	}

	return result;
}

int OtsuThreshold(double* histogram, double total)
{
	double sum = 0.0;
	
	for (int i = 1; i < 256; i++)
	{
		sum += i * histogram[i];
	}

	double sumB = 0.0;
	double wB = 0.0;
	double wF = 0.0;
	double mB = 0.0;
	double mF = 0.0;
	double between = 0.0;
	double maxvalue = 0;
	double max = 0.0;
	int threshold1 = 0.0;
	int threshold2 = 0.0;

	int result = 0;
	
	for (int i = 0; i < 256; ++i) 
	{
		wB += histogram[i];

		if (wB == 0)
			continue;

		wF = total - wB;

		if (wF == 0)
			break;

		sumB += i * histogram[i];
		
		mB = sumB / wB;
		mF = (sum - sumB) / wF;
		between = wB * wF * (mB - mF) * (mB - mF);

		if ( between >= max ) 
		{
			threshold1 = i;
			if ( between > max ) 
			{
				threshold2 = i;
			}
			max = between;
		}
	}

	result = (int)(( threshold1 + threshold2 ) / 2 + 0.5);

	if (checkUnimodal(histogram, result) == true)
	{
		result = m_TargetValue;
	}

	return result;
}

void MakeGaussianKernel()
{	
	int x, y, z;
	
	for (z=0; z<WinSize; z++)
	{
		for (y=0; y<WinSize; y++)
			for (x=0; x<WinSize; x++)
			{
				double dist = sqrt((double)((x-HWinSize)*(x-HWinSize)+(y-HWinSize)*(y-HWinSize)+(z-HWinSize)*(z-HWinSize)));
				m_Gauss[z][y][x] = exp(-dist*dist/30);
			}
	}
}

void MakeGaussianKernel2D()
{	
	int x, y;
	
	for (y=0; y<WinSize; y++)
		for (x=0; x<WinSize; x++)
		{
			double dist = sqrt((double)((x-HWinSize)*(x-HWinSize)+(y-HWinSize)*(y-HWinSize)));
			m_Gauss2D[y][x] = exp(-dist*dist/30);
		}
}

int GetThreshold2D(int x, int y)
{
	double histogram[256] = {0.0f};
	double total = 0.0;
	double weight = 0.0;
	int xx, yy, zz;

	for (yy = (y-HWinSize); yy <= (y+HWinSize); yy++)
	{
		if (yy < 0 || yy > sizey-1)
			continue;

		for (xx = x-HWinSize; xx <= (x+HWinSize); xx++)
		{
			if (xx < 0 || xx > sizex-1)
				continue;

			int value = m_Volume[m_currentZ][yy][xx];

			if (value < m_HistoMin || value > m_HistoMax)
				continue;

			weight = m_Gauss2D[yy-y+HWinSize][xx-x+HWinSize];
			histogram[value] += weight;
			total += weight;
		}
	}

	int threshold = OtsuThreshold(histogram, total);

	return threshold;
}

int GetThreshold3D(int x, int y, int z)
{
	double histogram[256] = {0.0f};
	double total = 0.0;
	double weight = 0.0;
	int xx, yy, zz;
	
	for (int value = 0; value <= 255; value++)
		histogram[value] = 0.0f;

	for (zz = (z-m_HWinSizeZ); zz <= (z+m_HWinSizeZ); zz++)
	{
		if (zz < 0 || zz > m_sizez-1)
			continue;

		for (yy = (y-HWinSize); yy <= (y+HWinSize); yy++)
		{
			if (yy < 0 || yy > sizey-1)
				continue;

			for (xx = x-HWinSize; xx <= (x+HWinSize); xx++)
			{
				if (xx < 0 || xx > sizex-1)
					continue;

				int value = m_Volume[zz][yy][xx];

				if (value < m_HistoMin || value > m_HistoMax)
					continue;

				weight = m_Gauss[zz-z+HWinSize][yy-y+HWinSize][xx-x+HWinSize];
				histogram[value] += weight;
				total += weight;
			}
		}
	}

	int threshold = OtsuThreshold(histogram, total);

	return threshold;
}

int GetObjectMean(int x, int y, int z)
{
	double histogram[256] = {0.0f};
	double total = 0.0;
	double weight = 0.0;
	double sum;
	int xx, yy, zz;
	int mean;

	for (zz = (z-m_HWinSizeZ); zz <= (z+m_HWinSizeZ); zz++)
	{
		if (zz < 0 || zz > m_sizez-1)
			continue;

		for (yy = (y-HWinSize); yy <= (y+HWinSize); yy++)
		{
			if (yy < 0 || yy > sizey-1)
				continue;

			for (xx = x-HWinSize; xx <= (x+HWinSize); xx++)
			{
				if (xx < 0 || xx > sizex-1)
					continue;

				int value = m_Volume[zz][yy][xx];

				if (value < m_HistoMin || value > m_HistoMax)
					continue;

				if (m_TempObject[zz][yy][xx] == 0)
					continue;

				weight = m_Gauss[zz-z+HWinSize][yy-y+HWinSize][xx-x+HWinSize];
				histogram[value] += weight;
				total += weight;
			}
		}
	}

	sum = 0.0;
	for (int i=0; i<256; i++)
	{
		sum += histogram[i];
	}

	mean = sum / total;

	return mean;
}

void MakeVoxelThreshold1()
{
	int x, y, z, thres, t1, t2, p1, p2;
	
	
	for(z=0; z<m_sizez; z+= BlockSize){
		for(y=0; y<sizey; y+= BlockSize){
			for(x=0; x<sizex; x+= BlockSize){
				thres = GetThreshold3D(x, y, z);
				if (thres < m_MinThres)	thres = m_MinThres;
				if (thres > m_MaxThres)	thres= m_MaxThres;
				m_Thres[z][y][x] = thres;
			}
		}
	}
	
	
	for(z=0; z<m_sizez; z+= BlockSize)			
		for(y=0; y<sizey; y+= BlockSize)
			for(x=0; x<sizex; x++){
				p1 = (int)(x/BlockSize) * BlockSize;
				p2 = ((int)(x/BlockSize) + 1) * BlockSize;
				if (p2 >= sizex) p2 = sizex-1;
				t1 = m_Thres[z][y][p1];
				t2 = m_Thres[z][y][p2];
				m_Thres[z][y][x] = (int)((t1*(p2-x) + t2*(x-p1))/(p2-p1));
			}

	
	for(z=0; z<m_sizez; z+= BlockSize)			
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++){
				p1 = (int)(y/BlockSize) * BlockSize;
				p2 = ((int)(y/BlockSize) + 1) * BlockSize;
				if (p2 >= sizey) p2 = sizey-1;
				t1 = m_Thres[z][p1][x];
				t2 = m_Thres[z][p2][x];
				m_Thres[z][y][x] = (int)((t1*(p2-y) + t2*(y-p1))/(p2-p1));
			}

	
	for(z=0; z<m_sizez; z++)			
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++){
				p1 = (int)(z/BlockSize) * BlockSize;
				p2 = ((int)(z/BlockSize) + 1) * BlockSize;
				if (p2 >= m_sizez) p2 = m_sizez-1;
				t1 = m_Thres[p1][y][x];
				t2 = m_Thres[p2][y][x];
				m_Thres[z][y][x] = (int)((t1*(p2-z) + t2*(z-p1))/(p2-p1));
			}
}

void FindInitialArea()
{
	int x, y, z;

	
	for(z=0; z<m_sizez; z++){
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++){
#if 1//def __ITERATION__
				if (m_Volume[z][y][x] > (m_TargetValue))
#else
			//	m_Voxel[z][y][x] = m_InputImg[z][y][x] + (m_TargetValue - GetThreshold3D(x, y, z, 0));
				m_Voxel[z][y][x] = m_InputImg[z][y][x] + (m_TargetValue - m_Thres[z][y][x]);
				if (m_InputImg[z][y][x] > m_Thres[z][y][x])
#endif
					m_Object[z][y][x] = 1;
				else	
					m_Object[z][y][x] = 0;
			}
	}
}

#if 0
void CSegmentation::SetInitialBG()
{
	int x, y, z;

	currentBoundary_clear();

	vPoint a[8];
	a[0].x = 0;		a[0].y = 0;		a[0].z = 0;
	a[1].x = 0;		a[1].y = 511;	a[1].z = 0;
	a[2].x = 511;	a[2].y = 0;		a[2].z = 0;
	a[3].x = 511;	a[3].y = 511;	a[3].z = 0;
	
	for(int i=0; i<4; i++)
	{
		x = a[i].x;		y = a[i].y;		z = a[i].z;
		if ((m_Index[z][y][x].IsObject == false) &&	(m_InputImg[z][y][x] < m_BgThres))
		{	
			vPoint currentPoint;
			currentPoint.x = x;
			currentPoint.y = y;
			currentPoint.z = z;
			m_Index[z][y][x].IsBackground = true;
			currentBoundary_push_back(currentPoint);
		}
	}
}

bool CSegmentation::FindNextBG()
{
	int vIndex;
	int x, y, z;
	bool IsChanged = false;

	nextBoundary_clear();

	for (vIndex = 0; vIndex < m_CurBoundNo; vIndex ++)
	{
		x = GetCur_x(vIndex);
		y = GetCur_y(vIndex);
		z = GetCur_z(vIndex);

		if ((x > 0) && (m_Index[z][y][x-1].IsBackground == false) && (m_Index[z][y][x-1].IsObject == false))
		{
			vPoint cP;
			cP.x = x - 1;		cP.y = y;		cP.z = z;
			nextBoundary_push_back(cP);
			m_Index[z][y][x-1].IsBackground = true;
			IsChanged = true;
		}

		if ((x < (sizex-1)) && (m_Index[z][y][x+1].IsBackground == false) && (m_Index[z][y][x+1].IsObject == false))
		{
			vPoint cP;
			cP.x = x + 1;		cP.y = y;		cP.z = z;
	
			nextBoundary_push_back(cP);
			m_Index[z][y][x+1].IsBackground = true;
			IsChanged = true;
		}

		if ((y > 0) && (m_Index[z][y-1][x].IsBackground == false) && (m_Index[z][y-1][x].IsObject == false))
		{
			vPoint cP;
			cP.x = x;		cP.y = y - 1;		cP.z = z;
			nextBoundary_push_back(cP);
			m_Index[z][y-1][x].IsBackground = true;
			IsChanged = true;
		}

		if ((y < (sizey-1)) && (m_Index[z][y+1][x].IsBackground == false) && (m_Index[z][y+1][x].IsObject == false))
		{
			vPoint cP;
			cP.x = x;		cP.y = y + 1;		cP.z = z;
			nextBoundary_push_back(cP);
			m_Index[z][y+1][x].IsBackground = true;
			IsChanged = true;
		}

		if ((z > 0) && (m_Index[z-1][y][x].IsBackground == false) && (m_Index[z-1][y][x].IsObject == false))
		{
			vPoint cP;
			cP.x = x;		cP.y = y;		cP.z = z - 1;
			nextBoundary_push_back(cP);
			m_Index[z-1][y][x].IsBackground = true;
			IsChanged = true;
		}

		if ((z < (m_sizez-1)) && (m_Index[z+1][y][x].IsBackground == false) && (m_Index[z+1][y][x].IsObject == false))
		{
			vPoint cP;
			cP.x = x;		cP.y = y;		cP.z = z + 1;
			nextBoundary_push_back(cP);
			m_Index[z+1][y][x].IsBackground = true;
			IsChanged = true;
		}
	}

	return IsChanged;
}

void CSegmentation::FillInside()
{
	int x, y, z;

	
	for(z=0; z<m_sizez; z++)	
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++)
				if (m_Index[z][y][x].IsBackground)
					m_Index[z][y][x].IsObject = false;
				else
					m_Index[z][y][x].IsObject = true;
}

void CSegmentation::FloodFill()
{
	bool IsChanged;
	int iterationCnt = 0;
	
	m_BgThres = m_HistoMin;

	SetInitialBG();
	
	do{
		IsChanged = FindNextBG();
		ReadyForNextIteration();
		iterationCnt ++;
	}while(iterationCnt < 5000 && IsChanged);

	FillInside();

	currentBoundary_clear();
	nextBoundary_clear();
}
#endif

void vFindBoundary()
{
	int x, y, z;

	currentBoundary_clear();	
	
	for(z=0; z<m_sizez; z++)	
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++)
				if (((y-1) >= 0 && (m_Object[z][y-1][x] != m_Object[z][y][x]))	||
					((y+1) < sizey && (m_Object[z][y+1][x] != m_Object[z][y][x])) ||
					((x-1) >= 0 && (m_Object[z][y][x-1] != m_Object[z][y][x]))	||
					((x+1) < sizex && (m_Object[z][y][x+1] != m_Object[z][y][x])) ||
					((z-1) >= 0 && (m_Object[z-1][y][x] != m_Object[z][y][x]))	||
					((z+1) < m_sizez && (m_Object[z+1][y][x] != m_Object[z][y][x])))
				{	
					vPoint currentPoint;
					currentPoint.x = x;
					currentPoint.y = y;
					currentPoint.z = z;
				//	currentPoint.IsInside = m_Index[z][y][x].IsObject? true:false;
					currentBoundary_push_back(currentPoint);
				}
}


int vApplyThreshold1()
{
	int IsInside, thres, thresdiff, IsChanged = 0;
	unsigned char maskBackup;
	int vIndex;
	int abn = 0;
	int xx, yy, zz;

	nextBoundary_clear();

	myOutputDebugString(_T("\tm_CurBoundNo = %d\n"), m_CurBoundNo);  

	for (vIndex = 0; vIndex < m_CurBoundNo; vIndex ++)
	{
		xx = GetCur_x(vIndex);
		yy = GetCur_y(vIndex);
		zz = GetCur_z(vIndex);
	//	IsInside = (int)GetCur_IsInside(vIndex);

		maskBackup = m_Object[zz][yy][xx];
#ifdef __INTERPOLATION__
		thres = m_Thres[zz][yy][xx];
#else
		thres = GetThreshold3D(xx, yy, zz);
#endif

		if (thres < m_MinThres)	thres = m_MinThres;
		if (thres > m_MaxThres)	thres = m_MaxThres;

		thresdiff = m_TargetValue - thres;
		m_Voxel[zz][yy][xx] = m_Volume[zz][yy][xx] + thresdiff;
		
		if (m_Volume[zz][yy][xx] >= thres)
			m_Object[zz][yy][xx] = 1;
		else
			m_Object[zz][yy][xx] = 0;
		
		if (m_Object[zz][yy][xx] != maskBackup)
		{
			IsChanged = 1;

			vPoint currentPoint;
			currentPoint.x = xx;		currentPoint.y = yy;		currentPoint.z = zz;	
		//	currentPoint.IsInside = m_Index[zz][yy][xx].IsObject;

			if (currentPoint.x > 0 && m_Boundary[zz][yy][xx - 1] == 0){
				currentPoint.x = xx - 1; 	//	currentPoint.IsInside = m_Index[zz][yy][xx - 1].IsObject;
				nextBoundary_push_back(currentPoint);
				m_Boundary[zz][yy][xx - 1] = 1;
			}
			currentPoint.x = xx;
			if (currentPoint.x < sizex-1 && m_Boundary[zz][yy][xx + 1] == 0){
				currentPoint.x = xx + 1; 	//	currentPoint.IsInside = m_Index[zz][yy][xx + 1].IsObject;
				nextBoundary_push_back(currentPoint);
				m_Boundary[zz][yy][xx + 1] = 1;
			}
			currentPoint.x = xx;

			if (currentPoint.y > 0 && m_Boundary[zz][yy - 1][xx] == 0){
				currentPoint.y = yy - 1; 	//	currentPoint.IsInside = m_Index[zz][yy - 1][xx].IsObject;
				nextBoundary_push_back(currentPoint);
				m_Boundary[zz][yy - 1][xx] = true;
			}
			currentPoint.y = yy; 
			if (currentPoint.y < sizey-1 && m_Boundary[zz][yy + 1][xx] == 0){
				currentPoint.y = yy + 1; 	//	currentPoint.IsInside = m_Index[zz][yy + 1][xx].IsObject;
				nextBoundary_push_back(currentPoint);
				m_Boundary[zz][yy + 1][xx] = true;
			}
			currentPoint.y = yy;

			if (currentPoint.z > 0 && m_Boundary[zz - 1][yy][xx] == 0){
				currentPoint.z = zz - 1; 	//	currentPoint.IsInside = m_Index[zz - 1][yy][xx].IsObject;
				nextBoundary_push_back(currentPoint);
				m_Boundary[zz - 1][yy][xx] = true;
			}
			currentPoint.z = zz; 
			if (currentPoint.z < m_sizez-1 && m_Boundary[zz + 1][yy][xx] == 0){
				currentPoint.z = zz + 1; 	//	currentPoint.IsInside = m_Index[zz + 1][yy][xx].IsObject;
				nextBoundary_push_back(currentPoint);
				m_Boundary[zz + 1][yy][xx] = true;
			}
		}
	}

	return IsChanged;
}

void ReadyForNextIteration()
{
	int vIndex;
	
	currentBoundary_clear();
	for (vIndex = 0; vIndex < m_NextBoundNo; vIndex ++){
		vPoint currentPoint;

		currentPoint.x = GetNext_x(vIndex);		currentPoint.y = GetNext_y(vIndex);		currentPoint.z = GetNext_z(vIndex);
	//	currentPoint.IsInside = GetNext_IsInside(vIndex);
		currentBoundary_push_back(currentPoint);
	}
}

void IterationProcess()
{
	int IsChanged;

//	cout << "Iteration ... " << endl;
	m_IterationCnt = 0;
	myOutputDebugString(_T("vFindBoundary().....................\n"), m_IterationCnt);  
	vFindBoundary();

	do{
		myOutputDebugString(_T("m_IterationCnt = %d\n"), m_IterationCnt);  

		IsChanged = vApplyThreshold1();
		ReadyForNextIteration();
		m_IterationCnt ++;
	}while(m_IterationCnt < m_MaxIterationNo && IsChanged);

}

void InitSegmentation()
{
//	int x, y, z;

	VectorInit();

/*	m_InputImg = new int ** [m_sizez];
	for(z=0; z<m_sizez; z++)
	{
		m_InputImg[z] = new int * [sizey];
		for (y=0; y<sizey; y++)
			m_InputImg[z][y] = new int [sizex];
	}

	m_Voxel = new int ** [m_sizez];
	for(z=0; z<m_sizez; z++)
	{
		m_Voxel[z] = new int * [sizey];
		for (y=0; y<sizey; y++)
			m_Voxel[z][y] = new int [sizex];
	}

	m_Thres = new int ** [m_sizez];
	for(z=0; z<m_sizez; z++)
	{
		m_Thres[z] = new int * [sizey];
		for (y=0; y<sizey; y++)
			m_Thres[z][y] = new int [sizex];
	}

	m_Index = new tIndex ** [m_sizez];
	for(z=0; z<m_sizez; z++)
	{
		m_Index[z] = new tIndex * [sizey];
		for (y=0; y<sizey; y++)
			m_Index[z][y] = new tIndex [sizex];
	}

	int value;
	
	for(z=0; z<m_sizez; z++)			
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++)
			{
				value = DICOMtoImage(volume[z][y][x], m_HUmin, m_HUmax);
				m_InputImg[z][y][x] = value;
				m_Voxel[z][y][x] = Voxel[z][y][x];
				m_Index[z][y][x].IsObject = Index[z][y][x].IsObject;
				m_Index[z][y][x].IsBoundary = false;
				m_Index[z][y][x].IsBackground = false;
				m_Index[z][y][x].Seed = Index[z][y][x].Seed;
				m_Index[z][y][x].TempObject = false;
			}
*/		
}

void StoreSegmentationResult()
{
/*	int x, y, z;

	for(z=0; z<m_sizez; z++)			
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++){
				Index[z][y][x].IsObject = m_Index[z][y][x].IsObject;
			//	Index[z][y][x].IsObject = m_Index[z][y][x].TempObject;
				Index[z][y][x].IsBackground = m_Index[z][y][x].IsBackground;
				Index[z][y][x].Seed = false;
				Voxel[z][y][x] = m_Voxel[z][y][x];
			}
*/
}

void DeleteMemory()
{
/*	int y, z;

	if(m_Voxel != NULL)
	{
		for(z = 0; z < m_sizez; z++)
		{
			for(int y = 0; y < 512; y++)
				delete [] m_Voxel[z][y];
			delete [] m_Voxel[z] ;
		}
		delete [] m_Voxel;
		m_Voxel = NULL;
	}
	if(m_InputImg != NULL)
	{
		for(z = 0; z < m_sizez; z++)
		{
			for(int y = 0; y < 512; y++)
				delete [] m_InputImg[z][y];
			delete [] m_InputImg[z] ;
		}
		delete [] m_InputImg;
		m_InputImg = NULL;
	}

	if(m_Thres != NULL)
	{
		for(z = 0; z < m_sizez; z++)
		{
			for(int y = 0; y < 512; y++)
				delete [] m_Thres[z][y];
			delete [] m_Thres[z] ;
		}
		delete [] m_Thres;
		m_Thres = NULL;
	}

	if(m_Index != NULL)
	{
		for(z = 0; z < m_sizez; z++)
		{
			for(int y = 0; y < 512; y++)
				delete [] m_Index[z][y];
			delete [] m_Index[z] ;
		}
		delete [] m_Index;
		m_Index = NULL;
	}
*/
}

void Thresholding()
{
	int x, y, z, thresdiff;
	
	for(z=0; z<m_sizez; z++){
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++){
				if (m_Volume[z][y][x] > m_TargetValue)
					m_Object[z][y][x] = 1;
				else	
					m_Object[z][y][x] = 0;

				m_Voxel[z][y][x] = m_Volume[z][y][x];
			}
	}
}

void AdaptiveThreshold3D()
{
	double t1;
	t1 = (double)GetTickCount();
	
	myOutputDebugString(_T("MakeGaussianKernel().....................\n"), m_IterationCnt);  
	MakeGaussianKernel();

#ifdef __INTERPOLATION__
	MakeVoxelThreshold1();
#endif

	myOutputDebugString(_T("FindInitialArea().....................\n"), m_IterationCnt);  
	FindInitialArea();

#ifdef __ITERATION__
	myOutputDebugString(_T("IterationProcess().....................\n"), m_IterationCnt);  
	IterationProcess();
#endif

	t1 = (double)GetTickCount() - t1;
	myOutputDebugString(_T("total time = %f\n"), t1/1000);
}

int Lamda1 = 1;
int Lamda2 = 31;
int sigma = 10;	//51;
int edge_th = 35;
int number = 170;
int maxdiff = 60;
//int center = 180;
int centerBackup;
int probmax = 200;

double DifferenceFunction(int input)
{
	double result;
//	double sigma2 = (double)(sigma*sigma);
	double sigma2 = (double)(sigma);

#if 1
	result = exp(-(double)(input*input)/sigma2);

#else
	if (input < 0)
		input = input * (-1);

	if (input < 10)
		result = 1.0;
	else if (input < 30)
		result = 1.0 - ((double)input - 10.0)/20.0;
	else
		result = 0.0;
#endif
	return result;
}
/*
void CSegmentation::MakeVoxel2D()
{
	int x, y, z;
	int vIndex, diff;

	z = m_currentZ;
	
	for(y=0; y<sizey; y++)
		for(x=0; x<sizex; x++)
		{
			if (m_Index[z][y][x].IsObject == true)
				m_Voxel[z][y][x] = m_TargetValue+1;
			else
				m_Voxel[z][y][x] = 0;
		}

	bool IsBoundary;
	int x1, y1;

	for(y=0; y<sizey; y++)
		for(x=0; x<sizex; x++)
		{
			IsBoundary = false;

			for (y1=y-1; y1<=y+1; y1++)
				for (x1=x-1; x1<=x+1; x1++)
				{
					if (y1>=0 && y1<sizey && x1>=0 && x1<sizex)
					{
						if (m_Index[z][y][x].IsObject!= m_Index[z][y1][x1].IsObject)
						{
							IsBoundary = true;

							goto Next;
						}
					}
				}

			Next :
				if (IsBoundary)
					m_Voxel[z][y][x] = m_TargetValue + (m_InputImg[z][y][x] - GetThreshold3D(x, y, z));
		}
}
*/

void MakeVoxel3D()
{
	int x, y, z;
	int vIndex, diff;

	for(z=0; z<m_sizez; z++)	
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++)
			{
				if (m_Object[z][y][x] == 1)
					m_Voxel[z][y][x] = m_TargetValue+1;
				else
					m_Voxel[z][y][x] = 0;
			}

	bool IsBoundary;
	int x1, y1, z1;

	for(z=0; z<m_sizez; z++)	
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++)
			{
				IsBoundary = false;

				for (z1=z-1; z1<= z+1; z1++)
					for (y1=y-1; y1<=y+1; y1++)
						for (x1=x-1; x1<=x+1; x1++)
						{
							if (z1>=0 && z1<m_sizez && y1>=0 && y1<sizey && x1>=0 && x1<sizex)
							{
								if (m_Object[z][y][x]!= m_Object[z1][y1][x1])
								{
									IsBoundary = true;

									goto Next;
								}
							}
						}

				Next :
					if (IsBoundary)
						m_Voxel[z][y][x] = m_TargetValue + (m_Volume[z][y][x] - GetThreshold3D(x, y, z));
			}
}

void GraphCutSegmentation()
{
	typedef Graph<int,int,int> GraphType;
//	GraphType *g = new GraphType(262144, 523264); 	//estimated # of nodes, estimated # of edges
	GraphType *g = new GraphType(140000000, 512*512*300*3);	//estimated # of nodes, estimated # of edges

	int x, y, z;
	int node1, node2;
	double cap1, cap2, cap, I1, I2, P1, P2;
	double Lamda_1 = (double)(Lamda1);	//1;//pow(10.0, (double)(Lamda1 - 5));
	double Lamda_2 = (double)(Lamda2);			//31;//(double)(Lamda2 - 50);
//	double sigma1 = pow(10.0, (double)(sigma - 50));		//10;//pow(10.0, (double)(sigma - 50));
	double sigma1 = sigma;
	double sigma2 = sigma1*sigma1;
	int edgeValue;

	for(z=0; z<m_sizez; z++)
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++)
				g -> add_node(); 

	int max = maxdiff;
	double BigProbability = 0.99999;
	
	cout << "Add Tweight" << endl;
	for(z=0; z<m_sizez; z++)
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++)
			{
				I1 = m_Volume[z][y][x] - m_TargetValue;

				if (I1 > max)		{	P1 = 0.5 + ((double)max/probmax);		P2 = 1 - P1;	}
				else if (I1 < -max)	{	P1 = 0.5 - ((double)max/probmax);		P2 = 1 - P1;	}
				else 
				{
					P1 = 0.5 + I1/probmax;
					P2 = 0.5 - I1/probmax;
				}

			/*	if (User[z][y][x] == OBJECT)	
			//	if (UserOB[z][y][x])
				{
					P1 = BigProbability;
					P2 = 1.0 - P1;	
				}
				if (User[z][y][x] == BACKGROUND)
				{
					P2 = BigProbability;
					P1 = 1.0 - P2;
				}*/
				
				cap1 = -Lamda_1 * log(P2);	// source cap
				cap2 = -Lamda_1 * log(P1);	// sink cap

			//	cap1 = - log(P2);	// source cap
			//	cap2 = - log(P1);	// sink cap

				g -> add_tweights(z*sizexy + y*sizex + x,  cap1, cap2);			
		}


	cout << "Add Edge" << endl;
	for(z=0; z<m_sizez; z++)
		for(y=0; y<(sizey-1); y++)
			for(x=0; x<sizex; x++){
				node1 = z*sizexy + y*sizex + x;
				node2 = z*sizexy + (y+1)*sizex + x;
				I1 = m_Volume[z][y][x];
				I2 = m_Volume[z][y+1][x];
			//	cap = Lamda_2 * exp(-(I1-I2)*(I1-I2)/sigma1);
			//	cap = Lamda_2 * exp(-(I1-I2)*(I1-I2)/sigma2);
				cap = Lamda_2 * DifferenceFunction(I1-I2);
			//	edgeValue = Edge[z][y][x];
			//	cap = Lamda_2 * exp(-(double)(edgeValue * edgeValue / 10));

				if (I1 > I2){
					cap1 = cap;	// forward direction cap
					cap2 = cap;	// reverse direction cap
				}
				else{
					cap1 = cap;	// forward direction cap
					cap2 = cap;	// reverse direction cap
				}
				g -> add_edge(node1, node2, cap1, cap2);
			}

	for(z=0; z<m_sizez; z++)
		for(y=0; y<sizey; y++)
			for(x=0; x<(sizex-1); x++){
				node1 = z*sizexy + y*sizex + x;
				node2 = z*sizexy + y*sizex + (x+1);
				I1 = m_Volume[z][y][x];
				I2 = m_Volume[z][y][x+1];
			//	cap = Lamda_2 * exp(-(I1-I2)*(I1-I2)/sigma1);
			//	cap = Lamda_2 * exp(-(I1-I2)*(I1-I2)/sigma2);
				cap = Lamda_2 * DifferenceFunction(I1-I2);
			//	edgeValue = Edge[z][y][x];
			//	cap = Lamda_2 * exp(-(double)(edgeValue * edgeValue / 10));

				if (I1 > I2){
					cap1 = cap;	// forward direction cap
					cap2 = cap;	// reverse direction cap
				}
				else{
					cap1 = cap;	// forward direction cap
					cap2 = cap;	// reverse direction cap
				}	
				g -> add_edge(node1, node2, cap1, cap2);
			}

	for(z=0; z<(m_sizez-1); z++)
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++){
				node1 = z*sizexy + y*sizex + x;
				node2 = (z+1)*sizexy + y*sizex + x;
				I1 = m_Volume[z][y][x];
				I2 = m_Volume[z+1][y][x];
			//	cap = Lamda_2 * exp(-(I1-I2)*(I1-I2)/sigma1);
			//	cap = Lamda_2 * exp(-(I1-I2)*(I1-I2)/sigma2);
				cap = Lamda_2 * DifferenceFunction(I1-I2);
			//	edgeValue = Edge[z][y][x];
			//	cap = Lamda_2 * exp(-(double)(edgeValue * edgeValue / 10));

				if (I1 > I2){
					cap1 = cap;	// forward direction cap
					cap2 = cap;	// reverse direction cap
				}
				else{
					cap1 = cap;	// forward direction cap
					cap2 = cap;	// reverse direction cap
				}	
				g -> add_edge(node1, node2, cap1, cap2);
			}
			
	cout << "Max Flow" << endl;
	int flow = g -> maxflow();

	for(z=0; z<m_sizez; z++)
		for(y=0; y<sizey; y++)
			for(x=0; x<sizex; x++){
				node1 = z*sizexy + y*sizex + x;
				if (g->what_segment(node1) == GraphType::SOURCE)
					m_Object[z][y][x] = 1;
				else
					m_Object[z][y][x] = 0;
			}
	
	delete g;

	cout << "Finished" << endl;
}

void InitDynamicRegionGrowing()
{
	int x, y;

	for (y=0; y<sizey; y++)
		for (x=0; x<sizex; x++)
		{
			m_TempObject[m_currentZ][y][x] = m_Seed[m_currentZ][y][x];
		}

	int histo[256], i, sum, num;

	for (i=0; i<256; i++)
		histo[i] = 0;
	
	for (y=0; y<sizey; y++)
		for (x=0; x<sizex; x++)
		{
			if (m_TempObject[m_currentZ][y][x])
				histo[m_Volume[m_currentZ][y][x]] ++;
		}

	sum = 0;
	num = 0;
	for (i=0; i<256; i++)
	{
		num += histo[i];
		sum += histo[i] * i;
	}

	m_ObjectMean= sum / num;

	sum = 0;
	for (i=0; i<256; i++)
	{
		sum += histo[i] * histo[i] * i;
	}

	m_ObjectVariable = sum / num - m_ObjectMean * m_ObjectMean;
	
	m_MaxIterationNo = 500;
	
}

void vFindBoundaryDRG()
{
	int x, y;

	currentBoundary_clear();	
	
	for(y=0; y<sizey; y++)
		for(x=0; x<sizex; x++)
			if (((y-1) >= 0 && (m_TempObject[m_currentZ][y-1][x] != m_TempObject[m_currentZ][y][x]))	||
				((y+1) < sizey && (m_TempObject[m_currentZ][y+1][x] != m_TempObject[m_currentZ][y][x])) ||
				((x-1) >= 0 && (m_TempObject[m_currentZ][y][x-1] != m_TempObject[m_currentZ][y][x]))	||
				((x+1) < sizex && (m_TempObject[m_currentZ][y][x+1] != m_TempObject[m_currentZ][y][x])) )
			{	
				vPoint currentPoint;
				currentPoint.x = x;
				currentPoint.y = y;
				currentPoint.z = m_currentZ;
				currentBoundary_push_back(currentPoint);
			}
}

int vApplyThresholdDRG()
{
	int IsInside, thres, thresdiff, IsChanged = 0;
	unsigned char maskBackup;
	int vIndex;
	int abn = 0;
	int xx, yy, zz;
	int oMean;

	nextBoundary_clear();

	for (vIndex = 0; vIndex < m_CurBoundNo; vIndex ++)
	{
		xx = GetCur_x(vIndex);
		yy = GetCur_y(vIndex);
		zz = GetCur_z(vIndex);

		maskBackup = m_TempObject[zz][yy][xx];
		thres = GetThreshold2D(xx, yy);
	//	oMean = GetObjectMean(xx, yy, zz);

		if (thres < m_MinThres)		thres = m_MinThres;
		if (thres > m_MaxThres)	thres = m_MaxThres;

		thresdiff = m_TargetValue - thres;
		m_Voxel[zz][yy][xx] = m_Volume[zz][yy][xx] + thresdiff;
		
		if (m_Volume[zz][yy][xx] >= thres)
			m_TempObject[zz][yy][xx] = 1;
		else
			m_TempObject[zz][yy][xx] = 0;
		
		if (m_TempObject[zz][yy][xx] != maskBackup)
		{
			IsChanged = 1;

			vPoint currentPoint;
			currentPoint.x = xx;		currentPoint.y = yy;		currentPoint.z = zz;	

			if (currentPoint.x > 0 && m_Boundary[zz][yy][xx - 1] == 0){
				currentPoint.x = xx - 1; 	//	currentPoint.IsInside = m_Index[zz][yy][xx - 1].TempObject;
				nextBoundary_push_back(currentPoint);
			}
			currentPoint.x = xx;
			if (currentPoint.x < sizex-1 && m_Boundary[zz][yy][xx + 1] == 0){
				currentPoint.x = xx + 1; 	//	currentPoint.IsInside = m_Index[zz][yy][xx + 1].TempObject;
				nextBoundary_push_back(currentPoint);
			}
			currentPoint.x = xx;

			if (currentPoint.y > 0 && m_Boundary[zz][yy - 1][xx] == 0){
				currentPoint.y = yy - 1; 	//	currentPoint.IsInside = m_Index[zz][yy - 1][xx].TempObject;
				nextBoundary_push_back(currentPoint);
			}
			currentPoint.y = yy; 
			if (currentPoint.y < sizey-1 && m_Boundary[zz][yy + 1][xx] == 0){
				currentPoint.y = yy + 1; 	//	currentPoint.IsInside = m_Index[zz][yy + 1][xx].TempObject;
				nextBoundary_push_back(currentPoint);
			}
			currentPoint.y = yy;
		}
	}

	return IsChanged;
}

void IterationProcessDRG()
{
	int IsChanged;

	m_IterationCnt = 0;
	vFindBoundaryDRG();
	do{
		IsChanged = vApplyThresholdDRG();
		ReadyForNextIteration();
		m_IterationCnt ++;
	}while(m_IterationCnt < m_MaxIterationNo && IsChanged);
}

void DynamicRegionGrowing()
{
	MakeGaussianKernel2D();

	InitDynamicRegionGrowing();

	IterationProcessDRG();

	int x, y, value;
	
	for (y=0; y<512; y++)
		for (x=0; x<512; x++)
		{
			if (m_TempObject[m_currentZ][y][x])
			{
				m_Object[m_currentZ][y][x] = 1;
				m_Voxel[m_currentZ][y][x] = m_TargetValue + 1;
			}
			else
			{
				m_Object[m_currentZ][y][x] = 0;
				m_Voxel[m_currentZ][y][x] = m_TargetValue;
			}
		}
}

void LevelSet2D()
{
	typedef float	InternalPixelType;
	typedef itk::Image< InternalPixelType, 2 >	InternalImageType;
	typedef itk::Image< unsigned char, 2 > OutputImageType;
	typedef itk::BinaryThresholdImageFilter< InternalImageType, OutputImageType >	 ThresholdingFilterType;
	typedef itk::CurvatureAnisotropicDiffusionImageFilter< InternalImageType, InternalImageType >  SmoothingFilterType;
	typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< InternalImageType, InternalImageType >	GradientFilterType;
	typedef itk::SigmoidImageFilter< InternalImageType, InternalImageType >  SigmoidFilterType;
	typedef itk::FastMarchingImageFilter< InternalImageType, InternalImageType >	FastMarchingFilterType;
	typedef itk::ShapeDetectionLevelSetImageFilter< InternalImageType,	InternalImageType >    ShapeDetectionFilterType;
	typedef FastMarchingFilterType::NodeContainer NodeContainer;
	typedef FastMarchingFilterType::NodeType NodeType;

	InternalImageType::Pointer image = InternalImageType::New();
	ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
	SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
	GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
	SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
	FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();;
	ShapeDetectionFilterType::Pointer  shapeDetection = ShapeDetectionFilterType::New();

	InternalImageType::IndexType start;
	start[0] =   0;  // first index on X
	start[1] =   0;  // first index on Y

	InternalImageType::SizeType  size;
	size[0]  = sizex;  // size along X
	size[1]  = sizey;  // size along Y

	InternalImageType::RegionType region;
	region.SetSize( size );
	region.SetIndex( start );

	image->SetRegions( region );
	image->Allocate();

	itk::ImageRegionIterator<InternalImageType> imageIterator1(image,image->GetRequestedRegion());
	imageIterator1 = imageIterator1.Begin();

	int x, y, z;

	for (y=0; y<512; y++)
		for (x=0; x<512; x++)
		{
			imageIterator1.Set((float)m_Volume[m_currentZ][y][x]);
			imageIterator1++;
		}

	thresholder->SetLowerThreshold( -1000.0 );
	thresholder->SetUpperThreshold(     0.0 );
	thresholder->SetOutsideValue(  0  );
	thresholder->SetInsideValue(  255 );

	sigmoid->SetOutputMinimum(  0.0  );
	sigmoid->SetOutputMaximum(  1.0  );

	smoothing->SetInput( image );
	gradientMagnitude->SetInput( smoothing->GetOutput() );
	sigmoid->SetInput( gradientMagnitude->GetOutput() );

	smoothing->SetTimeStep( 0.125 );
	smoothing->SetNumberOfIterations(  5 );
	smoothing->SetConductanceParameter( 9.0 );

	const double sigma = 1.0;
	gradientMagnitude->SetSigma(  sigma  );

	const double alpha =  -0.5;
	const double beta  =  3.0;	//3.0;
	sigmoid->SetAlpha( alpha );
	sigmoid->SetBeta(  beta  );

	NodeContainer::Pointer seeds = NodeContainer::New();

	const double initialDistance = 5;
	const double seedValue = - initialDistance;

	seeds->Initialize();

	int cnt = 0;
	for (y=0; y<512; y++)
		for (x=0; x<512; x++)
		{
			if (m_Seed[m_currentZ][y][x])
			{
				InternalImageType::IndexType  seedPosition;
				seedPosition[0] = x;
				seedPosition[1] = y;
			
				NodeType node;
				node.SetValue( seedValue );
				node.SetIndex( seedPosition );

				seeds->InsertElement( cnt++, node );
			}
		}

	fastMarching->SetTrialPoints(  seeds  );
	fastMarching->SetSpeedConstant( 1.0 );
	fastMarching->SetOutputSize(image->GetBufferedRegion().GetSize() );

	const double curvatureScaling   = 0.05;
	const double propagationScaling = 1;

	shapeDetection->SetPropagationScaling(  propagationScaling );
	shapeDetection->SetCurvatureScaling( curvatureScaling );
	shapeDetection->SetMaximumRMSError( 0.02 );
	shapeDetection->SetNumberOfIterations( 800 );

	fastMarching->Update();
	
	InternalImageType::Pointer fastMarchingResult = InternalImageType::New();
	fastMarchingResult = fastMarching->GetOutput();
	
	shapeDetection->SetInput( fastMarchingResult );
	shapeDetection->SetFeatureImage( sigmoid->GetOutput() );
	thresholder->SetInput( shapeDetection->GetOutput() );

	try
	{
		thresholder->Update();
	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception caught !" << std::endl;
		std::cerr << excep << std::endl;
	}

	OutputImageType::Pointer outimage = thresholder->GetOutput();

	itk::ImageRegionConstIterator<OutputImageType> imageIterator2(outimage,outimage->GetRequestedRegion());
	imageIterator2 = imageIterator2.Begin();
	int value;

	for (y=0; y<512; y++)
		for (x=0; x<512; x++)
		{
			value = imageIterator2.Get();
			if (value > 128)
			{
				m_Object[m_currentZ][y][x] = 1;
				m_Voxel[m_currentZ][y][x] = m_TargetValue + 1;
			}
			else
			{
				m_Object[m_currentZ][y][x] = 0;
				m_Voxel[m_currentZ][y][x] = m_TargetValue;
			}
			
			imageIterator2++;
		}


}

void LevelSet3D()
{
	typedef float	InternalPixelType;
	typedef itk::Image< InternalPixelType, 3 >	InternalImageType;
	typedef itk::Image< unsigned char, 3 > OutputImageType;
	typedef itk::BinaryThresholdImageFilter< InternalImageType, OutputImageType >	 ThresholdingFilterType;
	typedef itk::RescaleIntensityImageFilter<InternalImageType, OutputImageType>   CastFilterType;
	typedef itk::CurvatureAnisotropicDiffusionImageFilter< InternalImageType, InternalImageType >  SmoothingFilterType;
	typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< InternalImageType, InternalImageType >	GradientFilterType;
	typedef itk::SigmoidImageFilter< InternalImageType, InternalImageType >  SigmoidFilterType;
	typedef itk::FastMarchingImageFilter< InternalImageType, InternalImageType >	FastMarchingFilterType;
	typedef itk::ShapeDetectionLevelSetImageFilter< InternalImageType,	InternalImageType >    ShapeDetectionFilterType;
	typedef itk::NumericSeriesFileNames    NameGeneratorType;
	typedef FastMarchingFilterType::NodeContainer NodeContainer;
	typedef FastMarchingFilterType::NodeType NodeType;

	InternalImageType::Pointer image = InternalImageType::New();
	ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
	SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
	GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
	SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
	FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();;
	ShapeDetectionFilterType::Pointer  shapeDetection = ShapeDetectionFilterType::New();
	NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();

	InternalImageType::IndexType start;
	start[0] =   0;  // first index on X
	start[1] =   0;  // first index on Y
	start[2] =   0;  // first index on Z  

	InternalImageType::SizeType  size;
	size[0]  = sizex;  // size along X
	size[1]  = sizey;  // size along Y
	size[2]  = m_sizez;  // size along Z

	InternalImageType::RegionType region;
	region.SetSize( size );
	region.SetIndex( start );

	InternalImageType::SpacingType spacing;
	spacing[0] = 1;	//0.33; // spacing along X
	spacing[1] = 1;	//0.33; // spacing along Y
	spacing[2] = 4;	//1.20; // spacing along Z

	image->SetRegions( region );
	image->SetSpacing( spacing );
	image->Allocate();

	itk::ImageRegionIterator<InternalImageType> imageIterator1(image,image->GetRequestedRegion());
	imageIterator1 = imageIterator1.Begin();

	int x, y, z;

	for (z=0; z<m_sizez; z++)
		for (y=0; y<512; y++)
			for (x=0; x<512; x++)
			{
				imageIterator1.Set((float)m_Volume[z][y][x]);
				imageIterator1++;
			}


	thresholder->SetLowerThreshold( -1000.0 );
	thresholder->SetUpperThreshold(     0.0 );
	thresholder->SetOutsideValue(  0  );
	thresholder->SetInsideValue(  255 );

	sigmoid->SetOutputMinimum(  0.0  );
	sigmoid->SetOutputMaximum(  1.0  );

	smoothing->SetInput( image );
	gradientMagnitude->SetInput( smoothing->GetOutput() );
	sigmoid->SetInput( gradientMagnitude->GetOutput() );

	smoothing->SetTimeStep( 0.0625 );
	smoothing->SetNumberOfIterations(  5 );
	smoothing->SetConductanceParameter( 9.0 );

	const double sigma = 1.0;
	gradientMagnitude->SetSigma(  sigma  );

	const double alpha =  -0.5;
	const double beta  =  6;	//3.0;
	sigmoid->SetAlpha( alpha );
	sigmoid->SetBeta(  beta  );

	NodeContainer::Pointer seeds = NodeContainer::New();

	const double initialDistance = 5;
	const double seedValue = - initialDistance;

	seeds->Initialize();

	int cnt = 0;
	for (z=0; z<m_sizez; z++)
		for (y=0; y<512; y++)
			for (x=0; x<512; x++)
			{
				if (m_Seed[z][y][x])
				{
					InternalImageType::IndexType  seedPosition;
					seedPosition[0] = x;
					seedPosition[1] = y;
					seedPosition[2] = z;

					NodeType node;
					node.SetValue( seedValue );
					node.SetIndex( seedPosition );

					seeds->InsertElement( cnt++, node );
				}
			}

	fastMarching->SetTrialPoints(  seeds  );
	fastMarching->SetSpeedConstant( 1.0 );
	fastMarching->SetOutputSize(image->GetBufferedRegion().GetSize() );

	const double curvatureScaling   = 0.5; //0.05;
	const double propagationScaling = 1;

	shapeDetection->SetPropagationScaling(  propagationScaling );
	shapeDetection->SetCurvatureScaling( curvatureScaling );
	shapeDetection->SetMaximumRMSError( 0.02 );
	shapeDetection->SetNumberOfIterations( 800 );

	fastMarching->Update();
	
	InternalImageType::Pointer fastMarchingResult = InternalImageType::New();
	fastMarchingResult = fastMarching->GetOutput();
	fastMarchingResult->SetSpacing( spacing );
	
	shapeDetection->SetInput( fastMarchingResult );
	shapeDetection->SetFeatureImage( sigmoid->GetOutput() );
	thresholder->SetInput( shapeDetection->GetOutput() );

	try
	{
		thresholder->Update();
	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception caught !" << std::endl;
		std::cerr << excep << std::endl;
	}

	OutputImageType::Pointer outimage = thresholder->GetOutput();

	itk::ImageRegionConstIterator<OutputImageType> imageIterator2(outimage,outimage->GetRequestedRegion());
	imageIterator2 = imageIterator2.Begin();
	int value;

	for (z=0; z<m_sizez; z++)
		for (y=0; y<512; y++)
			for (x=0; x<512; x++)
			{
				value = imageIterator2.Get();
				if (value > 128)
				{
					m_Object[z][y][x] = 1;
					m_Voxel[z][y][x] = m_TargetValue + 1;
				}
				else
				{
					m_Object[z][y][x] = 0;
					m_Voxel[z][y][x] = 0;
				}
				
				imageIterator2++;
			}


}

void Segmentation(SEG_MODE SegMode, int StartLevel, int Zsize, vector<tTri> *triangles, int min, int max, int currentZ)
{
	m_sizez = Zsize;
	m_HWinSizeZ = (int)(HWinSize / (sizex/Zsize) + 0.5);
	m_IterationCnt = 0;
	m_SegMode = SegMode;
	m_currentZ = currentZ;
	m_IterationCnt = 0;
	m_HistoMin = StartLevel - 30;
	m_HistoMax = 255;
	m_CheckRange = 50;
	m_MinThres = m_HistoMin;
	m_MaxThres = m_HistoMax;
	m_MaxIterationNo = 50;
	m_TargetValue = StartLevel;
	m_HUmin = min;
	m_HUmax = max;

	myOutputDebugString(_T("m_sizez : %d\n"), m_sizez);  
	myOutputDebugString(_T("m_HWinSizeZ : %d\n"), m_HWinSizeZ);  
	
	InitSegmentation();

	switch(m_SegMode)
	{
		case THRESHOLD:
			Thresholding();
			break;
		case ATHRESHOLD:
			AdaptiveThreshold3D();
			break;
		case GRAPHCUT:
			GraphCutSegmentation();
			MakeVoxel3D();
			break;
		case DYNAMICREGIONGROWING:
			DynamicRegionGrowing();
		//	MakeVoxel2D();
			break;
		case LEVELSET_2D:
			LevelSet2D();
		//	MakeVoxel2D();
			break;
		case LEVELSET_3D:
			LevelSet3D();
		//	MakeVoxel3D();
			break;
	}

	StoreSegmentationResult();
#if 0
	triangles->clear();
	CMarchingCube model;
	model.Start(m_Voxel, m_TargetValue, triangles, m_sizez);
#endif
	DeleteMemory();
}
