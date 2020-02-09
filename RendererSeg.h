#pragma once

#include <glm/glm.hpp>

#include "DICOM ViewerView.h"
#include "TranformationMgr.h"
#include "glsl.h"

using namespace cwc;



class RendererSeg
{
public:
	RendererSeg(void);
	~RendererSeg(void);

	bool Initialize();
	bool SetParent(CScrollView* _parent);
	void Destroy();
	void Resize( int cx, int cy);
	void Render();
	void GetMeanVector3();

	void MouseMove(UINT nFlags, CPoint point);
	void LButtonDown(UINT nFlags, CPoint point);
	void LButtonUp(UINT nFlags, CPoint point);
	BOOL MouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void RButtonDown(UINT nFlags, CPoint point);
	void RButtonUp(UINT nFlags, CPoint point);
	
private:
	void Normal(float v[3][3], float* normal);
	void Normalize (float* v);
	void DemoLight(void);
	void DrawBox(float x, float y, float z, float w, float h, float d);
	void DrawBox();
	int CreateDisplayLists();

	//void Get3DRayUnderMouse(int x, int y, glm::vec3* v1, glm::vec3* v2);
	//bool RaySphereCollision(glm::vec3 vSphereCenter, float fSphereRadius, glm::vec3 vA, glm::vec3 vB);
	//int CheckLineBox( glm::vec3 B1, glm::vec3 B2, glm::vec3 L1, glm::vec3 L2, glm::vec3 &Hit);
	//inline int GetIntersection( float fDst1, float fDst2, glm::vec3 P1, glm::vec3 P2, glm::vec3 &Hit);
	//inline int InBox( glm::vec3 Hit, glm::vec3 B1, glm::vec3 B2, const int Axis);

	HGLRC m_hRC;
	HDC m_hDC;

	CScrollView* m_parent;

	BOOL m_bStartToMove4;
	CPoint m_StartPoint4;
	CPoint m_MovePoint4;
	CPoint m_DownPoint4;

	LONG mouse_down;
	float prev_x;
	float prev_y;

	float zoom;
	float pitch;
	float yaw;

	GLfloat m_mean_vector[3];
	float m_min_vector[3];
	float m_max_vector[3];

	CTranformationMgr m_transMgr;
	glShader* shader;
};