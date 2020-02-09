
#include "stdio.h"
#include <math.h>
#include <cstdio>
#include <vector>

#pragma once

#define sizex	512
#define sizey	512

typedef struct vector_T
{
    float fX;
    float fY;
    float fZ;     
} tVector;

typedef struct facet_T
{
	float normal[3];
	float v1[3];
	float v2[3];
	float v3[3];
} facet;

typedef struct Tri_T
{
	float normal[3];
	float vertex[3][3];
	unsigned int Attr;
	COLORREF color;
} tTri;

typedef struct STLTri_T
{
	float normal[3];
	float v1[3];
	float v2[3];
	float v3[3];
	unsigned short attri; 
} STLTri;

typedef struct vPoint_T
{
	unsigned short x;
	unsigned short y;
	unsigned short z;
	bool IsInside;
} vPoint;

using namespace std;

class CMarchingCube
{
protected:
	
private:
//	int ***m_volume;
	int m_sizez;
	float m_thres;
	float m_StepSize;
	float m_Vfactor;

	float VoxelVal(float fX, float fY, float fZ);
	float fGetOffset(float fValue1, float fValue2, float fValueDesired);
	void GetNormal(tVector &rfNormal, float *p1, float *p2, float *p3);
	void MarchingCube(float fX, float fY, float fZ, vector<tTri> *triangles, COLORREF _c);
	
public:
	CMarchingCube();
	virtual ~CMarchingCube();
	void Start(int threshold, vector<tTri> *triangles, int sizez);
};


