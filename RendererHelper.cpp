#include "StdAfx.h"
#include "RendererHelper.h"
#include "RawDataProcessor.h"
#include "TranformationMgr.h"

#include "MainFrm.h"
#include "DICOM ViewerDoc.h"

GLfloat dOrthoSize = 1.0f;

CRendererHelper::CRendererHelper(void) :
    m_pRawDataProc(NULL),m_zoom(1.0)
{
	AspectRatio = 1;

	for(int i = 0; i < 3; i++)
	{
		m_min_vector[i] = -1.0f;
		m_max_vector[i] = 1.0f;
	}
}

CRendererHelper::~CRendererHelper(void)
{
}


bool CRendererHelper::Initialize( HDC hContext_i 
    ,CRawDataProcessor* pRawDataProc_i )
{
    //Setting up the dialog to support the OpenGL.
    PIXELFORMATDESCRIPTOR stPixelFormatDescriptor;
    memset( &stPixelFormatDescriptor, 0, sizeof( PIXELFORMATDESCRIPTOR ));
    stPixelFormatDescriptor.nSize = sizeof( PIXELFORMATDESCRIPTOR );
    stPixelFormatDescriptor.nVersion = 1;
    stPixelFormatDescriptor.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW ;
    stPixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    stPixelFormatDescriptor.cColorBits = 24;
    stPixelFormatDescriptor.cDepthBits = 32;
    stPixelFormatDescriptor.cStencilBits = 8;
    stPixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE ;
    int nPixelFormat = ChoosePixelFormat( hContext_i, &stPixelFormatDescriptor ); //Collect the pixel format.

    if( nPixelFormat == 0 )
    {
        AfxMessageBox( _T( "Error while Choosing Pixel format" ));
        return false;
    }
    //Set the pixel format to the current dialog.
    if( !SetPixelFormat( hContext_i, nPixelFormat, &stPixelFormatDescriptor ))
    {
        AfxMessageBox( _T( "Error while setting pixel format" ));
        return false;
    }

    //Create a device context.
    m_hglContext = wglCreateContext( hContext_i );
    if( !m_hglContext )
    {
        AfxMessageBox( _T( "Rendering Context Creation Failed" ));
        return false;
    }
    //Make the created device context as the current device context.
    BOOL bResult = wglMakeCurrent( hContext_i, m_hglContext );
    if( !bResult )
    {
        AfxMessageBox( _T( "wglMakeCurrent Failed" ));
        return false;
    }

    glClearColor( 0.0f,0.0f, 0.0f, 0.0f );

    glewInit();
    if(GL_TRUE != glewGetExtension("GL_EXT_texture3D"))
    {
        AfxMessageBox( _T( "3D texture is not supported !" ));
        return false;
    }

	if(m_pRawDataProc == NULL)
	{
		m_pRawDataProc = pRawDataProc_i;
	}

	//if(m_pTransformMgr == NULL)
	//{
	//	m_pTransformMgr = pTransformationMgr_i;
	//}

    return true;
}


void CRendererHelper::Resize( int nWidth_i, int nHeight_i )
{
	AspectRatio = ( GLdouble )(nWidth_i) / ( GLdouble )(nHeight_i ); 

	if(nHeight_i == 0)
		nHeight_i = 1;

	double view_size = 0;
	if(nWidth_i <= nHeight_i)
	{
		view_size = nWidth_i;
	}
	else
	{
		view_size = nHeight_i;
	}

	AspectWidth = nWidth_i / view_size;
	AspectHeight = nHeight_i / view_size;


	//glViewport((nWidth_i - view_size)/2, (nHeight_i - view_size)/2, view_size, view_size);
	glViewport(0, 0, nWidth_i, nHeight_i);

	//fAspect = (GLfloat)nWidth_i/(GLfloat)nHeight_i;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glFrustum(0,view_size,0,view_size,-10,20);
	glFrustum(0,nWidth_i,0,nHeight_i,-10,20);


	//gluPerspective(45.0f, fAspect, -10.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0.05f );

    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void CRendererHelper::DrawPolygon(void)
{
	/* draws the sides of a unit cube (0,0,0)-(1,1,1) */
	glBegin(GL_POLYGON);/* f1: front */
	glNormal3f(-1.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,1.0f);
	glVertex3f(1.0f,0.0f,1.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glEnd();
	glBegin(GL_POLYGON);/* f2: bottom */
	glNormal3f(0.0f,0.0f,-1.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glVertex3f(1.0f,1.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glEnd();
	glBegin(GL_POLYGON);/* f3:back */
	glNormal3f(1.0f,0.0f,0.0f);
	glVertex3f(1.0f,1.0f,0.0f);
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(0.0f,1.0f,1.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glEnd();
	glBegin(GL_POLYGON);/* f4: top */
	glNormal3f(0.0f,0.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(1.0f,0.0f,1.0f);
	glVertex3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f,1.0f,1.0f);
	glEnd();
	glBegin(GL_POLYGON);/* f5: left */
	glNormal3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,1.0f,1.0f);
	glVertex3f(0.0f,0.0f,1.0f);
	glEnd();
	glBegin(GL_POLYGON);/* f6: right */
	glNormal3f(0.0f,-1.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(1.0f,1.0f,0.0f);
	glEnd();
}

void CRendererHelper::DrawBox(void)
{
	m_min_vector[0] = -1.0f*AspectHeight;
	m_max_vector[0] = 1.0f*AspectHeight;

	m_min_vector[1] = -1.0f*AspectWidth;
	m_max_vector[1] = 1.0f*AspectWidth;

	//m_min_vector[1] = -1.0f;
	//m_max_vector[1] = 1.0f;

	m_min_vector[2] = -1.0f;
	m_max_vector[2] = 1.0f;

	glBegin(GL_LINES);  // X, Y, Z 선 표시
	glColor3f(1.0, 0.0, 0.0); // X축 

	//
	glVertex3f(m_min_vector[0], m_min_vector[1], m_min_vector[2]);
	glVertex3f(m_max_vector[0], m_min_vector[1], m_min_vector[2]);

	glVertex3f(m_min_vector[0], m_min_vector[1], m_max_vector[2]);
	glVertex3f(m_max_vector[0], m_min_vector[1], m_max_vector[2]);

	glVertex3f(m_min_vector[0], m_min_vector[1], m_min_vector[2]);
	glVertex3f(m_min_vector[0], m_min_vector[1], m_max_vector[2]);

	glVertex3f(m_max_vector[0], m_min_vector[1], m_min_vector[2]);
	glVertex3f(m_max_vector[0], m_min_vector[1], m_max_vector[2]);

	//
	glVertex3f(m_min_vector[0], m_max_vector[1], m_max_vector[2]);
	glVertex3f(m_max_vector[0], m_max_vector[1], m_max_vector[2]);

	glVertex3f(m_min_vector[0], m_max_vector[1], m_min_vector[2]);
	glVertex3f(m_max_vector[0], m_max_vector[1], m_min_vector[2]);

	glVertex3f(m_max_vector[0], m_max_vector[1], m_min_vector[2]);
	glVertex3f(m_max_vector[0], m_max_vector[1], m_max_vector[2]);

	glVertex3f(m_min_vector[0], m_max_vector[1], m_min_vector[2]);
	glVertex3f(m_min_vector[0], m_max_vector[1], m_max_vector[2]);

	//
	glVertex3f(m_min_vector[0], m_min_vector[1], m_max_vector[2]);
	glVertex3f(m_min_vector[0], m_max_vector[1], m_max_vector[2]);

	glVertex3f(m_max_vector[0], m_min_vector[1], m_max_vector[2]);
	glVertex3f(m_max_vector[0], m_max_vector[1], m_max_vector[2]);

	glVertex3f(m_min_vector[0], m_min_vector[1], m_min_vector[2]);
	glVertex3f(m_min_vector[0], m_max_vector[1], m_min_vector[2]);

	glVertex3f(m_max_vector[0], m_min_vector[1], m_min_vector[2]);
	glVertex3f(m_max_vector[0], m_max_vector[1], m_min_vector[2]);

	glEnd();
}


 // Macro to draw the quad.
 // Performance can be achieved by making a call list.
 // To make it simple i am not using that now :-)
  #define MAP_3DTEXT( TexIndex ) \
            glTexCoord3f(0.0f, 0.0f, ((float)TexIndex+1.0f)/2.0f);  \
        glVertex3f(-dOrthoSize,-dOrthoSize,TexIndex);\
        glTexCoord3f(1.0f, 0.0f, ((float)TexIndex+1.0f)/2.0f);  \
        glVertex3f(dOrthoSize,-dOrthoSize,TexIndex);\
        glTexCoord3f(1.0f, 1.0f, ((float)TexIndex+1.0f)/2.0f);  \
        glVertex3f(dOrthoSize,dOrthoSize,TexIndex);\
        glTexCoord3f(0.0f, 1.0f, ((float)TexIndex+1.0f)/2.0f);  \
        glVertex3f(-dOrthoSize,dOrthoSize,TexIndex);

void CRendererHelper::Render()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if (!pDoc)
		return;  

	if(!pDoc->IsDataLoaded())
		return;

    float fFrameCount = (float)m_pRawDataProc->GetDepth();
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
	m_camera.Render();

    // Translate and make 0.5f as the center 
    // (texture co ordinate is from 0 to 1. so center of rotation has to be 0.5f)
    glTranslatef( 0.5f, 0.5f, 0.5f );
	glScalef(m_zoom,m_zoom,m_zoom);
	//glRotatef(90.0f,1,0,0);
	//glRotatef(180.0f,0,1,0);

	glMultMatrixd( m_camera.GetMatrix());
    
    glTranslatef( -0.5f, -0.5f, -0.5f );

    glEnable(GL_TEXTURE_3D);
    glBindTexture( GL_TEXTURE_3D,  m_pRawDataProc->GetTexture3D() );
	
    for ( float fIndx = -1.0f; fIndx <= 1.0f; fIndx+=0.01f )
    {
        glBegin(GL_QUADS);
            //MAP_3DTEXT( fIndx );
			glTexCoord3f(0.0f, 0.0f, ((float)fIndx+1.0f)/2.0f);
			glVertex3f(-dOrthoSize*AspectHeight,-dOrthoSize*AspectWidth,fIndx);

			glTexCoord3f(1.0f, 0.0f, ((float)fIndx+1.0f)/2.0f);
			glVertex3f(dOrthoSize*AspectHeight,-dOrthoSize*AspectWidth,fIndx);
			
			glTexCoord3f(1.0f, 1.0f, ((float)fIndx+1.0f)/2.0f);
			glVertex3f(dOrthoSize*AspectHeight,dOrthoSize*AspectWidth,fIndx);
			
			glTexCoord3f(0.0f, 1.0f, ((float)fIndx+1.0f)/2.0f);
			glVertex3f(-dOrthoSize*AspectHeight,dOrthoSize*AspectWidth,fIndx);
        glEnd();
	}

	//DrawBox();

	glFlush();
}

void CRendererHelper::MouseMove(UINT nFlags, CPoint point)
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;  
	}

	if( nFlags & MK_RBUTTON )
	{
		m_camera.Rotate( mRotReference.y-point.y, mRotReference.x-point.x, 0 );
		mRotReference = point;
		m_parent->Invalidate(FALSE);
	}
}

void CRendererHelper::LButtonDown(UINT nFlags, CPoint point)
{

}

void CRendererHelper::LButtonUp(UINT nFlags, CPoint point)
{

}

BOOL CRendererHelper::MouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(zDelta >= 0)
	{
		setZoomDelta(-0.1);
	}
	else
	{
		setZoomDelta(0.1);
	}

	m_parent->Invalidate(FALSE);

	return TRUE;
}

void CRendererHelper::RButtonDown(UINT nFlags, CPoint point)
{
	mRotReference = point;
}

void CRendererHelper::RButtonUp(UINT nFlags, CPoint point)
{

}
