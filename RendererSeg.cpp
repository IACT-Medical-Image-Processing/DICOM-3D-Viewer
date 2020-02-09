#include "StdAfx.h"
#include "RendererSeg.h"
#include "DICOM ViewerDoc.h"
#include "MainFrm.h"
//#include "glsl.h"
#include <glm/gtc/matrix_transform.hpp>

#define M_PI       3.14159265358979323846

void CreateRotationalMatrix(double** matrix, double phi, double theta, double psi)
{
	matrix[0][0] =  cos(theta)*cos(psi);
	matrix[1][0] = -cos(phi)*sin(psi)+sin(phi)*sin(theta)*cos(psi);
	matrix[2][0] =  sin(phi)*sin(psi)+cos(phi)*sin(theta)*cos(psi);

	matrix[0][1] =  cos(theta)*sin(psi);
	matrix[1][1] =  cos(phi)*cos(psi)+sin(phi)*sin(theta)*sin(psi);
	matrix[2][1] = -sin(phi)*cos(psi)+cos(phi)*sin(theta)*sin(psi);

	matrix[0][2] = -sin(theta);
	matrix[1][2] =  sin(phi)*cos(theta);
	matrix[2][2] =  cos(phi)*cos(theta);
}

RendererSeg::RendererSeg(void)
{
	m_bStartToMove4 = FALSE;

	zoom = 30.0f;
	pitch = 0.0f;
	yaw = 0.0f;
}

RendererSeg::~RendererSeg(void)
{
}

bool RendererSeg::Initialize()
{
	return true;
}

bool RendererSeg::SetParent(CScrollView* _parent)
{
	if(_parent == NULL)
		return false;

	m_parent = _parent;

	return true;
}

void RendererSeg::Destroy()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	pDoc->m_triangles.clear();	
}

void RendererSeg::Resize( int cx, int cy)
{
	GLdouble	aspect;

	if(cy == 0)
	{
		aspect = (GLdouble)cx;
	}
	else
	{
		aspect = (GLdouble)cx / (GLdouble)cy;
	}

	glViewport(0,0, cx, cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 1.0f, 10000.0f);

	glShadeModel(GL_SHININESS);

	glEnable( GL_ALPHA_TEST );
	glAlphaFunc( GL_GREATER, 0.05f );

	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat diffuse[] = {0.55, 0.55, 0.55, 1.0};
	GLfloat specular[] = {0.70, 0.70, 0.70, 1.0};
	GLfloat shininess = 0.25;

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);

	// -------------------------------------------
	// Material parameters:

	GLfloat material_Ka[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat material_Kd[] = {0.4f, 0.4f, 0.4f, 1.0f};
	GLfloat material_Ks[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat material_Ke[] = {0.1f, 0.1f, 0.1f, 0.0f};
	GLfloat material_Se = 30.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);

	//glShaderManager SM;
	//shader = NULL;
	//shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file

	//if (shader==NULL) 
	//	return;

	//shader->enable();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RendererSeg::Normalize(float* v)
{
	// calculate the length of the vector
	float len = (float)(sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2])));

	// avoid division by 0
	if (len == 0.0f)
		len = 1.0f;

	// reduce to unit size
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

void RendererSeg::Normal(float v[3][3], float* normal)
{
	float a[3], b[3];

	// calculate the vectors A and B
	// note that v[3] is defined with counterclockwise winding in mind
	// a
	a[0] = v[0][0] - v[1][0];
	a[1] = v[0][1] - v[1][1];
	a[2] = v[0][2] - v[1][2];
	// b
	b[0] = v[1][0] - v[2][0];
	b[1] = v[1][1] - v[2][1];
	b[2] = v[1][2] - v[2][2];

	// calculate the cross product and place the resulting vector
	// into the address specified by vertex_t *normal
	normal[0] = (a[1] * b[2]) - (a[2] * b[1]);
	normal[1] = (a[2] * b[0]) - (a[0] * b[2]);
	normal[2] = (a[0] * b[1]) - (a[1] * b[0]);

	// normalize
	Normalize(normal);
}

void RendererSeg::Render()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glClearDepth(1.0f);
	//glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	DemoLight();

	glLoadIdentity();

	if(pDoc->m_bCalcMean == false)
	{
		if(pDoc->m_triangles.empty())
			return;

		GetMeanVector3();
		pDoc->m_bCalcMean = true;

		glTranslatef(0.0f, 0.0f, 0.0f);
		GLfloat x = m_max_vector[0] - m_min_vector[0];
		GLfloat y = m_max_vector[1] - m_min_vector[1];
		
		if(x > y) 
		{
			zoom = x + 5.0f;
		} 
		else 
		{
			zoom = y + 5.0f;
		}
		
		glTranslatef(-m_mean_vector[0], -m_mean_vector[1], -zoom);
		m_StartPoint4.x = -(m_mean_vector[0] * 10.0f);
		m_StartPoint4.y = -(m_mean_vector[1] * 10.0f);
		m_MovePoint4.x = 0;
		m_MovePoint4.y = 0;
	}

	//glPushMatrix();
	//if (shader) shader->begin();

	glTranslatef((m_StartPoint4.x + m_MovePoint4.x) * 0.1f, (m_StartPoint4.y + m_MovePoint4.y) * 0.1f, -zoom);
	glTranslatef(m_mean_vector[0], m_mean_vector[1], m_mean_vector[2]);
	glRotatef(yaw, 0.0f, 1.0f, 0.0f);
	glRotatef(abs(pitch), 1.0f, 0.0f, 0.0f);
	glTranslatef(-m_mean_vector[0], -m_mean_vector[1], -m_mean_vector[2]);

	glBegin(GL_TRIANGLES);

	for(size_t i = 0; i < pDoc->m_triangles.size(); i++)
	{
		float* n1 = pDoc->m_triangles[i].normal;
		COLORREF c = pDoc->m_triangles[i].color;	

		GLfloat* v1 = pDoc->m_triangles[i].vertex[0];
		GLfloat* v2 = pDoc->m_triangles[i].vertex[1];
		GLfloat* v3 = pDoc->m_triangles[i].vertex[2];

		glNormal3f(n1[0], n1[1], n1[2]);
		glColor4f(GetRValue(c)/255.0, GetGValue(c)/255.0, GetBValue(c)/255.0, 1.0f);
		glVertex3f(v1[0], v1[1], v1[2]);
		glVertex3f(v2[0], v2[1], v2[2]);
		glVertex3f(v3[0], v3[1], v3[2]);
	}
	glEnd();

	//if (shader) shader->end();

	//// 메쉬 중심
	//glBegin(GL_LINES);  // X, Y, Z 선 표시
	//glColor3f(1.0, 0.0, 0.0); // X축 
	//glVertex3f(m_mean_vector[0]+0.0, m_mean_vector[1], m_mean_vector[2]);
	//glVertex3f(m_mean_vector[0]+10.0, m_mean_vector[1], m_mean_vector[2]);

	//glColor3f(0.0, 1.0, 0.0); // Y축 
	//glVertex3f(m_mean_vector[0], m_mean_vector[1]+0.0, m_mean_vector[2]);
	//glVertex3f(m_mean_vector[0], m_mean_vector[1]+10.0, m_mean_vector[2]);

	//glColor3f(0.0, 0.0, 1.0); // Z축 
	//glVertex3f(m_mean_vector[0], m_mean_vector[1], m_mean_vector[2]+0.0);
	//glVertex3f(m_mean_vector[0], m_mean_vector[1], m_mean_vector[2]+10.0);
	//glEnd();

	//// 중심축
	//glBegin(GL_LINES);  // X, Y, Z 선 표시
	//glColor3f(1.0, 0.0, 0.0); // X축 
	//glVertex3f(0.0, 0, 0);
	//glVertex3f(10.0,0, 0);

	//glColor3f(0.0, 1.0, 0.0); // Y축 
	//glVertex3f(0, 0.0,  0);
	//glVertex3f(0, 10.0, 0);

	//glColor3f(0.0, 0.0, 1.0); // Z축 
	//glVertex3f(0, 0, 0.0);
	//glVertex3f(0, 0, 10.0);
	//glEnd();

	//DrawBox();


	//gluLookAt(0.0f, 0.0f, 100.0f, 0.0f, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f);	

	//if(selectItem == 1)
	//{
	//	glColor4f(1.0f, 1.0f, 0.5f, 0.8f);
	//}
	//else
	//{
	//	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	//}

	//// contour
	//glNewList( 1, GL_COMPILE );
	//glPushAttrib( GL_ALL_ATTRIB_BITS );
	//glEnable( GL_LIGHTING );
	//glClearStencil(0);
	//glClear( GL_STENCIL_BUFFER_BIT );
	//glEnable( GL_STENCIL_TEST );
	//glStencilFunc( GL_ALWAYS, 1, 0xFFFF );
	//glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	//glColor3f( 0.2f, 0.2f, 1.0f );

	//// contour
	//glDisable( GL_LIGHTING );
	//glStencilFunc( GL_NOTEQUAL, 1, 0xFFFF );
	//glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	//glLineWidth( 3.0f );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	//glColor3f( 0.0f, 0.0f, 1.0f );

	//glBegin(GL_TRIANGLES);

	//for(size_t i = 0; i < pDoc->m_triangles.size(); i++)
	//{
	//	//float* n1 = pDoc->m_triangles[i].normal;
	//	COLORREF c = pDoc->m_triangles[i].color;	

	//	GLfloat* v1 = pDoc->m_triangles[i].vertex[0];
	//	GLfloat* v2 = pDoc->m_triangles[i].vertex[1];
	//	GLfloat* v3 = pDoc->m_triangles[i].vertex[2];

	//	glVertex3f(v1[0], v1[1], v1[2]);
	//	glVertex3f(v2[0], v2[1], v2[2]);
	//	glVertex3f(v3[0], v3[1], v3[2]);
	//}
	//glEnd();

	//// contour
	//glPopAttrib();
	//glEndList();

	//glCallList( 1 );
	////

	//// 외곽선 처리
	//bool		outlineDraw		= true;								// Flag To Draw The Outline ( NEW )
	//bool		outlineSmooth	= false;							// Flag To Anti-Alias The Lines ( NEW )
	//float		outlineColor[3]	= { 0.0f, 0.0f, 0.0f };				// Color Of The Lines ( NEW )
	//float		outlineWidth	= 0.5f;								// Width Of The Lines ( NEW )
	//
	//if (outlineDraw)											// Check To See If We Want To Draw The Outline ( NEW )
	//{
	//	glEnable (GL_BLEND);									// Enable Blending ( NEW )
	//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);		// Set The Blend Mode ( NEW )
	//	glPolygonMode (GL_BACK, GL_LINE);						// Draw Backfacing Polygons As Wireframes ( NEW )
	//	glLineWidth (outlineWidth);								// Set The Line Width ( NEW )
	//	glCullFace (GL_FRONT);									// Don't Draw Any Front-Facing Polygons ( NEW )
	//	glDepthFunc (GL_LEQUAL);								// Change The Depth Mode ( NEW )
	//	glColor3fv (&outlineColor[0]);							// Set The Outline Color ( NEW )
	//	glBegin (GL_TRIANGLES);									// Tell OpenGL What We Want To Draw

	//	for (int i = 0; i < pDoc->m_triangles.size(); i++)						// Loop Through Each Polygon ( NEW )
	//	{
	//		GLfloat* v1 = pDoc->m_triangles[i].vertex[0];
	//		GLfloat* v2 = pDoc->m_triangles[i].vertex[1];
	//		GLfloat* v3 = pDoc->m_triangles[i].vertex[2];

	//		//glVertex3f(v1[0], v1[1], v1[2]);
	//		//glVertex3f(v2[0], v2[1], v2[2]);
	//		//glVertex3f(v3[0], v3[1], v3[2]);

	//		glVertex3fv (v1);
	//		glVertex3fv (v2);
	//		glVertex3fv (v3);
	//	}

	//	glEnd ();												// Tell OpenGL We've Finished
	//	glDepthFunc (GL_LESS);									// Reset The Depth-Testing Mode ( NEW )
	//	glCullFace (GL_BACK);									// Reset The Face To Be Culled ( NEW )
	//	glPolygonMode (GL_BACK, GL_FILL);						// Reset Back-Facing Polygon Drawing Mode ( NEW )
	//	glDisable (GL_BLEND);									// Disable Blending ( NEW )
	//}

	glFlush();
}

void RendererSeg::DemoLight(void)
{
	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_NORMALIZE);

	// Light model parameters:
	// -------------------------------------------

	GLfloat lmKa[] = {0.3, 0.3, 0.3, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

	// -------------------------------------------
	// Spotlight Attenuation

	GLfloat spot_direction[] = {0.0, 0.0, 0.0 };
	//glColor3f(1.0,0.0,0.0);
	//DrawBox(spot_direction[0],spot_direction[1],spot_direction[2],2,2,2);
	GLint spot_exponent = 30;
	GLint spot_cutoff = 180;

	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
	glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);

	GLfloat Kc = 1.0;
	GLfloat Kl = 0.0;
	GLfloat Kq = 0.0;

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,Kc);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);

	// ------------------------------------------- 
	// Lighting parameters:
	
	// LIGHT 0
	//GLfloat light_pos0[] = {m_mean_vector[0], m_min_vector[1] - 7.0f, m_mean_vector[2] + 5.0f, 1.0f};
	////GLfloat light_pos0[] = {0.0f, -7.0f, 5.0f, 1.0f};
	//glColor3f(1.0,0.0,0.0);
	//DrawBox(light_pos0[0],light_pos0[1],light_pos0[2],2,2,2);

	GLfloat light_Ka0[]  = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat light_Kd0[]  = {0.1f, 0.1f, 0.1f, 1.0f};
	GLfloat light_Ks0[]  = {1.0f, 1.0f, 1.0f, 1.0f};

	//glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka0);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd0);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks0);

	// ligth 1
	GLfloat light_pos1[] = {m_max_vector[0] + 5.0f, m_mean_vector[1], m_max_vector[2] + 5.0f, 1.0f};
	//GLfloat light_pos1[] = {15.0f, 0.0f, 10.0f, 1.0f};
	glColor3f(1.0,0.0,0.0);
	DrawBox(light_pos1[0],light_pos1[1],light_pos1[2],2,2,2);

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_Ka0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_Kd0);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_Ks0);


	// light 2
	GLfloat light_pos2[] = {m_min_vector[0] - 5.0f, m_max_vector[1] + 5.0f, m_max_vector[2] + 5.0f, 1.0f};
	//GLfloat light_pos2[] = {-15.0f, 5.0f, 10.0f, 1.0f};
	glColor3f(1.0,0.0,0.0);
	DrawBox(light_pos2[0],light_pos2[1],light_pos2[2],2,2,2);

	glLightfv(GL_LIGHT2, GL_POSITION, light_pos2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_Ka0);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_Kd0);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_Ks0);

	// -------------------------------------------
	// Material parameters:

	GLfloat material_Ka[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat material_Kd[] = {0.01f, 0.01f, 0.01f, 1.0f};
	GLfloat material_Ks[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat material_Ke[] = {0.0f, 0.0f, 0.0f, 0.0f};
	GLfloat material_Se = 0.25f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);
}

void RendererSeg::DrawBox()
{
	glBegin(GL_LINES);  // X, Y, Z 선 표시
	glColor3f(0.0, 1.0, 0.0); // X축 
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

void RendererSeg::DrawBox(float x, float y, float z, float w, float h, float d)
{
	glBegin(GL_LINES);  // X, Y, Z 선 표시

	glVertex3f(x, y, z);
	glVertex3f(x+w, y, z);

	glVertex3f(x, y, z+d);
	glVertex3f(x+w, y, z+d);

	glVertex3f(x, y, z);
	glVertex3f(x, y, z+d);

	glVertex3f(x+w, y, z);
	glVertex3f(x+w, y, z+d);

	//
	glVertex3f(x, y+h, z+d);
	glVertex3f(x+w, y+h, z+d);

	glVertex3f(x, y+h, z);
	glVertex3f(x+w, y+h, z);

	glVertex3f(x+w, y+h, z);
	glVertex3f(x+w, y+h, z+d);

	glVertex3f(x, y+h, z);
	glVertex3f(x, y+h, z+d);

	//
	glVertex3f(x, y, z+d);
	glVertex3f(x, y+h, z+d);

	glVertex3f(x+w, y, z+d);
	glVertex3f(x+w, y+h, z+d);

	glVertex3f(x, y, z);
	glVertex3f(x, y+h, z);

	glVertex3f(x+w, y, z);
	glVertex3f(x+w, y+h, z);

	glEnd();
}

int RendererSeg::CreateDisplayLists()
{
	glNewList( 1, GL_COMPILE );
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glEnable( GL_LIGHTING );
	glClearStencil(0);
	glClear( GL_STENCIL_BUFFER_BIT );
	glEnable( GL_STENCIL_TEST );
	glStencilFunc( GL_ALWAYS, 1, 0xFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glColor3f( 0.2f, 0.2f, 1.0f );
	//RenderMesh3();
	glDisable( GL_LIGHTING );
	glStencilFunc( GL_NOTEQUAL, 1, 0xFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	glLineWidth( 3.0f );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glColor3f( 1.0f, 1.0f, 1.0f );
	//RenderMesh3();
	glPopAttrib();
	glEndList();

	glNewList( 2, GL_COMPILE );
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glEnable( GL_LIGHTING );
	glClearStencil(0);
	glClear( GL_STENCIL_BUFFER_BIT );
	glEnable( GL_STENCIL_TEST );
	glStencilFunc( GL_ALWAYS, 1, -1 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glColor3f( 0.2f, 0.2f, 1.0f );
	//RenderMesh2();
	glDisable( GL_LIGHTING );
	glStencilFunc( GL_NOTEQUAL, 1, -1 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	glLineWidth( 3.0f );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glColor3f( 1.0f, 1.0f, 1.0f );
	//RenderMesh2();
	glPopAttrib();
	glEndList();

	return TRUE;
}

void RendererSeg::GetMeanVector3()
{
	CDICOMViewerDoc* pDoc = (CDICOMViewerDoc *)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc)
		return;

	m_min_vector[0] = pDoc->m_triangles[0].vertex[0][0];
	m_min_vector[1] = pDoc->m_triangles[0].vertex[0][1];
	m_min_vector[2] = pDoc->m_triangles[0].vertex[0][2];

	m_max_vector[0] = pDoc->m_triangles[0].vertex[0][0];
	m_max_vector[1] = pDoc->m_triangles[0].vertex[0][1];
	m_max_vector[2] = pDoc->m_triangles[0].vertex[0][2];


	for(size_t i = 0; i < pDoc->m_triangles.size(); i++)
	{
		float* v1 = pDoc->m_triangles[i].vertex[0];
		float* v2 = pDoc->m_triangles[i].vertex[1];
		float* v3 = pDoc->m_triangles[i].vertex[2];

		float local_min_vector[3] = {0,};
		float local_max_vector[3] = {0,};

		// x
		local_min_vector[0] = min(min(v1[0],v2[0]),v3[0]);
		local_max_vector[0] = max(max(v1[0],v2[0]),v3[0]);

		// y

		local_min_vector[1] = min(min(v1[1],v2[1]),v3[1]);
		local_max_vector[1] = max(max(v1[1],v2[1]),v3[1]);

		// z
		local_min_vector[2] = min(min(v1[2],v2[2]),v3[2]);
		local_max_vector[2] = max(max(v1[2],v2[2]),v3[2]);


		for(int k = 0; k < 3; k++)
		{
			if(local_min_vector[k] < m_min_vector[k])
			{
				m_min_vector[k] = local_min_vector[k];
			}

			if(local_max_vector[k] > m_max_vector[k])
			{
				m_max_vector[k] = local_max_vector[k];
			}
		}
	}

	for(int i = 0; i < 3; i++)
	{
		m_mean_vector[i] = (m_min_vector[i]+m_max_vector[i])/2.0f;
	}

	return;
}

void RendererSeg::MouseMove(UINT nFlags, CPoint point)
{
	// 마우스 왼쪽 벝튼
	if(m_bStartToMove4 == TRUE)
	{
		m_MovePoint4.x = point.x - m_DownPoint4.x;
		m_MovePoint4.y = - (point.y - m_DownPoint4.y);

		//Camera.RotateY(m_MovePoint4.x*0.01);
		//Camera.RotateX(m_MovePoint4.y*0.01);
		m_parent->Invalidate(FALSE);
	}

	// 마우스 오른쪽 버튼
	if( mouse_down == 1 )
	{
		//yaw += ((float)point.x - prev_x) * 0.2;
		//pitch += ((float)point.y - prev_y) * 0.2;

		yaw -= ((float)prev_x - (float)point.x)*0.2;
		pitch -= ((float)prev_y - (float)point.y)*0.2;

		if(yaw < 0)
		{
			yaw += 360;
		}
		if(yaw > 360)
		{
			yaw -= 360;
		}

		if(pitch < 0)
		{
			pitch += 360;
		}
		if(pitch > 360)
		{
			pitch -= 360;
		}
		//m_camera.RotateX(pitch);
		//m_camera.RotateY(yaw);

		//m_camera.Rotate( yaw, 0, -pitch );

		prev_x = point.x;
		prev_y = point.y;
		m_parent->Invalidate(FALSE);
	}

	//if( nFlags & MK_RBUTTON )
	//{
	//	//m_camera.Rotate( prev_y	-point.y, prev_x-point.x, 0 );
	//	m_camera.Rotate( mRotReference.y-point.y, mRotReference.x-point.x, 0 );
	//	mRotReference = point;
	//	m_parent->Invalidate(FALSE);
	//}
}

//void RendererSeg::Get3DRayUnderMouse(int x, int y, glm::vec3* v1, glm::vec3* v2)
//{
//	GLdouble modelview[16]; 
//	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
//
//	GLdouble projection[16]; 
//	glGetDoublev(GL_PROJECTION_MATRIX, projection);
//
//	GLint viewport[4]; 
//	glGetIntegerv(GL_VIEWPORT, viewport);
//	
//	GLdouble dv1[3];
//	GLdouble dv2[3];
//	gluUnProject (float(x), float(y), 0.0f, modelview, projection, viewport, &dv1[0],&dv1[1],&dv1[2]);
//	gluUnProject (float(x), float(y), 10.0f, modelview, projection, viewport,&dv2[0],&dv2[1],&dv2[2]);
//
//	v1->x = (GLfloat)dv1[0];
//	v1->y = (GLfloat)dv1[1];
//	v1->z = (GLfloat)dv1[2];
//
//	v2->x = (GLfloat)dv2[0];
//	v2->y = (GLfloat)dv2[1];
//	v2->z = (GLfloat)dv2[2];
//}

//int RendererSeg::GetIntersection( float fDst1, float fDst2, glm::vec3 P1, glm::vec3 P2, glm::vec3 &Hit) 
//{
//	if ( (fDst1 * fDst2) >= 0.0f) 
//		return 0;
//	if ( fDst1 == fDst2) 
//		return 0; 
//
//	Hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
//
//	return 1;
//}
//
//int RendererSeg::InBox( glm::vec3 Hit, glm::vec3 B1, glm::vec3 B2, const int Axis) 
//{
//	if ( Axis==1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
//	if ( Axis==2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
//	if ( Axis==3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
//
//	return 0;
//}
//
//// returns true if line (L1, L2) intersects with the box (B1, B2)
//// returns intersection point in Hit
//int RendererSeg::CheckLineBox( glm::vec3 B1, glm::vec3 B2, glm::vec3 L1, glm::vec3 L2, glm::vec3 &Hit)
//{
//	if (L2.x < B1.x && L1.x < B1.x) return false;
//	if (L2.x > B2.x && L1.x > B2.x) return false;
//	if (L2.y < B1.y && L1.y < B1.y) return false;
//	if (L2.y > B2.y && L1.y > B2.y) return false;
//	if (L2.z < B1.z && L1.z < B1.z) return false;
//	if (L2.z > B2.z && L1.z > B2.z) return false;
//	if (L1.x > B1.x && L1.x < B2.x &&
//		L1.y > B1.y && L1.y < B2.y &&
//		L1.z > B1.z && L1.z < B2.z) 
//	{
//		Hit = L1; 
//		return true;
//	}
//
//	if ( (GetIntersection( L1.x-B1.x, L2.x-B1.x, L1, L2, Hit) && InBox( Hit, B1, B2, 1 ))
//		|| (GetIntersection( L1.y-B1.y, L2.y-B1.y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 )) 
//		|| (GetIntersection( L1.z-B1.z, L2.z-B1.z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 )) 
//		|| (GetIntersection( L1.x-B2.x, L2.x-B2.x, L1, L2, Hit) && InBox( Hit, B1, B2, 1 )) 
//		|| (GetIntersection( L1.y-B2.y, L2.y-B2.y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 )) 
//		|| (GetIntersection( L1.z-B2.z, L2.z-B2.z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 )))
//	{
//		return true;
//	}
//
//	return false;
//}
//
//bool RendererSeg::RaySphereCollision(glm::vec3 vSphereCenter, float fSphereRadius, glm::vec3 vA, glm::vec3 vB)
//{
//	// Create the vector from end point vA to center of sphere
//	glm::vec3 vDirToSphere = vSphereCenter - vA;
//
//	// Create a normalized direction vector from end point vA to end point vB
//	glm::vec3 vLineDir = glm::normalize(vB-vA);
//
//	// Find length of line segment
//	float fLineLength = glm::distance(vA, vB);
//
//	// Using the dot product, we project the vDirToSphere onto the vector vLineDir
//	float t = glm::dot(vDirToSphere, vLineDir);
//
//	glm::vec3 vClosestPoint;
//	// If our projected distance from vA is less than or equal to 0, the closest point is vA
//	if (t <= 0.0f)
//		vClosestPoint = vA;
//	// If our projected distance from vA is greater thatn line length, closest point is vB
//	else if (t >= fLineLength)
//		vClosestPoint = vB;
//	// Otherwise calculate the point on the line using t and return it
//	else
//		vClosestPoint = vA+vLineDir*t;
//
//	// Now just check if closest point is within radius of sphere
//	return glm::distance(vSphereCenter, vClosestPoint) <= fSphereRadius;
//}

void RendererSeg::LButtonDown(UINT nFlags, CPoint point)
{
	m_bStartToMove4 = TRUE;
	m_DownPoint4 = point;

	//if(!m_parent)
	//	return; 

	//HWND hWnd = m_parent->GetSafeHwnd();
	//
	//POINT pt; 
	//GetCursorPos(&pt);
	//ScreenToClient(hWnd, &pt);

	//RECT rect; 
	//GetClientRect(hWnd, &rect);
	//pt.y = rect.bottom-pt.y;


	////bool plane_hit[6];

	//glm::vec3 v1;
	//glm::vec3 v2;
	//glm::vec3 hit;
	//Get3DRayUnderMouse(point.x,point.y,&v1,&v2);


	//glm::vec3 b1;
	//glm::vec3 b2;

	//if(CheckLineBox(b1,b2,v1,v2,hit))
	//{
	//}
}

void RendererSeg::LButtonUp(UINT nFlags, CPoint point)
{
	if(m_bStartToMove4)
	{
		m_bStartToMove4 = FALSE;
		m_StartPoint4.x += m_MovePoint4.x;
		m_StartPoint4.y -= -m_MovePoint4.y;
		m_MovePoint4.x = 0;
		m_MovePoint4.y = 0;
		m_parent->Invalidate(FALSE);
	}
}

BOOL RendererSeg::MouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(zDelta >= 0)
	{
		zoom -= 1;
	}
	else
	{
		zoom += 1;
	}
	m_parent->Invalidate(FALSE);

	return TRUE;
}

void RendererSeg::RButtonDown(UINT nFlags, CPoint point)
{
	mouse_down = 1;
	prev_x = point.x;
	prev_y = point.y;
	//mRotReference = point;
}

void RendererSeg::RButtonUp(UINT nFlags, CPoint point)
{
	mouse_down = 0;
}
