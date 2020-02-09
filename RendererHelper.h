#pragma once

#include "DICOM ViewerView.h"
#include "Camera.h"

class CRawDataProcessor;
class CTranformationMgr;

class CRendererHelper
{
public:
    CRendererHelper(void);
    virtual ~CRendererHelper(void);

    bool Initialize( HDC hContext_i, CRawDataProcessor* pRawDataProc_i);

	void Resize( int nWidth_i, int nHeight_i );
    void Render();
	void setZoomDelta(double _delta)
	{ 
		m_zoom += _delta; 
		if(m_zoom <= 0.1)
		{
			m_zoom = 0.1;
		}
	}
	void SetParent(CScrollView* _parent){ m_parent = _parent; }

	void MouseMove(UINT nFlags, CPoint point);
	void LButtonDown(UINT nFlags, CPoint point);
	void LButtonUp(UINT nFlags, CPoint point);
	BOOL MouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void RButtonDown(UINT nFlags, CPoint point);
	void RButtonUp(UINT nFlags, CPoint point);

private:
	void DrawBox(void);
	void DrawPolygon(void);

	double m_zoom;
    HGLRC m_hglContext;
    CRawDataProcessor* m_pRawDataProc;
	CTranformationMgr* m_pTransformMgr;
	double view_size;
	double AspectWidth;
	double AspectHeight;
	GLdouble AspectRatio;

	CScrollView* m_parent;
	CPoint mRotReference;

	float m_min_vector[3];
	float m_max_vector[3];
	CCamera m_camera;
};

