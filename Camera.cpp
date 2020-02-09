#include "StdAfx.h"
#include "camera.h"
#include "math.h"
#include <iostream>
#include "windows.h"

#define SQR(x) (x*x)

#define NULL_VECTOR F3dVector(0.0f,0.0f,0.0f)

SF3dVector F3dVector ( GLfloat x, GLfloat y, GLfloat z )
{
	SF3dVector tmp;
	tmp.x = x;
	tmp.y = y;
	tmp.z = z;
	return tmp;
} 

GLfloat GetF3dVectorLength( SF3dVector * v)
{
	return (GLfloat)(sqrt(SQR(v->x)+SQR(v->y)+SQR(v->z)));
}

SF3dVector Normalize3dVector( SF3dVector v)
{
	SF3dVector res;
	float l = GetF3dVectorLength(&v);
	if (l == 0.0f) return NULL_VECTOR;
	res.x = v.x / l;
	res.y = v.y / l;
	res.z = v.z / l;
	return res;
}

SF3dVector operator+ (SF3dVector v, SF3dVector u)
{
	SF3dVector res;
	res.x = v.x+u.x;
	res.y = v.y+u.y;
	res.z = v.z+u.z;
	return res;
}
SF3dVector operator- (SF3dVector v, SF3dVector u)
{
	SF3dVector res;
	res.x = v.x-u.x;
	res.y = v.y-u.y;
	res.z = v.z-u.z;
	return res;
}


SF3dVector operator* (SF3dVector v, float r)
{
	SF3dVector res;
	res.x = v.x*r;
	res.y = v.y*r;
	res.z = v.z*r;
	return res;
}

SF3dVector CrossProduct (SF3dVector * u, SF3dVector * v)
{
	SF3dVector resVector;
	resVector.x = u->y*v->z - u->z*v->y;
	resVector.y = u->z*v->x - u->x*v->z;
	resVector.z = u->x*v->y - u->y*v->x;

	return resVector;
}
float operator* (SF3dVector v, SF3dVector u)	//dot product
{
	return v.x*u.x+v.y*u.y+v.z*u.z;
}




/***************************************************************************************/

CCamera::CCamera()
{
	//Init with standard OGL values:
	Position = F3dVector (0.0, 0.0,	0.0);
	ViewDir = F3dVector( 0.0, 0.0, -1.0);
	RightVector = F3dVector (1.0, 0.0, 0.0);
	UpVector = F3dVector (0.0, 1.0, 0.0);

	//Only to be sure:
	//RotatedX = RotatedY = RotatedZ = 0.0;

	mdRotation[0]=mdRotation[5]=mdRotation[10]=mdRotation[15] = 1.0f;
	mdRotation[1]=mdRotation[2]=mdRotation[3]=mdRotation[4] = 0.0f;
	mdRotation[6]=mdRotation[7]=mdRotation[8]=mdRotation[9] = 0.0f;
	mdRotation[11]=mdRotation[12]=mdRotation[13]=mdRotation[14] = 0.0f;

	mfRot[0]=mfRot[1]=mfRot[2]=0.0f;

}

void CCamera::Move (SF3dVector Direction)
{
	Position = Position + Direction;
}

void CCamera::RotateX (GLfloat Angle)
{
	mfRot[0] += Angle;
	
	//Rotate viewdir around the right vector:
	ViewDir = Normalize3dVector(ViewDir*cos(Angle*PIdiv180)
								+ UpVector*sin(Angle*PIdiv180));

	//now compute the new UpVector (by cross product)
	UpVector = CrossProduct(&ViewDir, &RightVector)*-1;

	
}

void CCamera::RotateY (GLfloat Angle)
{
	mfRot[1] += Angle;
	
	//Rotate viewdir around the up vector:
	ViewDir = Normalize3dVector(ViewDir*cos(Angle*PIdiv180)
								- RightVector*sin(Angle*PIdiv180));

	//now compute the new RightVector (by cross product)
	RightVector = CrossProduct(&ViewDir, &UpVector);
}

void CCamera::RotateZ (GLfloat Angle)
{
	mfRot[2] += Angle;
	
	//Rotate viewdir around the right vector:
	RightVector = Normalize3dVector(RightVector*cos(Angle*PIdiv180)
								+ UpVector*sin(Angle*PIdiv180));

	//now compute the new UpVector (by cross product)
	UpVector = CrossProduct(&ViewDir, &RightVector)*-1;
}

void CCamera::Render( void )
{

	//The point at which the camera looks:
	SF3dVector ViewPoint = Position+ViewDir;

	//as we know the up vector, we can easily use gluLookAt:
	gluLookAt(	Position.x,Position.y,Position.z,
				ViewPoint.x,ViewPoint.y,ViewPoint.z,
				UpVector.x,UpVector.y,UpVector.z);

}

void CCamera::MoveForward( GLfloat Distance )
{
	Position = Position + (ViewDir*-Distance);
}

void CCamera::StrafeRight ( GLfloat Distance )
{
	Position = Position + (RightVector*Distance);
}

void CCamera::MoveUpward( GLfloat Distance )
{
	Position = Position + (UpVector*Distance);
}

void CCamera::Rotate(float fx_i, float fy_i, float fz_i )
{
	mfRot[0] = fx_i;
	mfRot[1] = fy_i;
	mfRot[2] = fz_i;

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixd( mdRotation );
	glRotated( mfRot[0], 1.0f, 0,0 );
	glRotated( mfRot[1], 0, 1.0f,0 );
	glRotated( mfRot[2], 0, 0,1.0f );

	glGetDoublev( GL_MODELVIEW_MATRIX, mdRotation );
	glLoadIdentity();
}

void CCamera::ResetRotation()
{
	mdRotation[0]=mdRotation[5]=mdRotation[10]=mdRotation[15] = 1.0f;
	mdRotation[1]=mdRotation[2]=mdRotation[3]=mdRotation[4] = 0.0f;
	mdRotation[6]=mdRotation[7]=mdRotation[8]=mdRotation[9] = 0.0f;
	mdRotation[11]=mdRotation[12]=mdRotation[13]=mdRotation[14] = 0.0f;
}