#include "StdAfx.h"
#include "picking.h"
//#include <glm/gtc/matrix_transform.hpp>

//
///*-----------------------------------------------
//
//Name:    GetColorByIndex
//
//Params:  index - index of color you want to generate
//
//Result:  Returns i-th RGB color.
//
///*---------------------------------------------*/
//
//glm::vec4 GetColorByIndex(int index)
//{
//	int r = index&0xFF;
//	int g = (index>>8)&0xFF;
//	int b = (index>>16)&0xFF;
//
//	return glm::vec4(float(r)/255.0f, float(g)/255.0f, float(b)/255.0f, 1.0f);
//}
//
///*-----------------------------------------------
//
//Name:    GetIndexByColor
//
//Params:  r, g, b - RGB values of color
//
//Result:  Kind of inverse to previous function,
//		 gets index from selected color.
//
///*---------------------------------------------*/
//
//int GetIndexByColor(int r, int g, int b)
//{
//	return (r)|(g<<8)|(b<<16);
//}
//
///*-----------------------------------------------
//
//Name:    GetColorByIndex
//
//Params:  index - index of color you want to generate
//
//Result:  Returns i-th RGB color.
//
///*---------------------------------------------*/
//
//int GetPickedColorIndexUnderMouse()
//{
//	HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
//	POINT mp; GetCursorPos(&mp);
//	ScreenToClient(hWnd, &mp);
//	RECT rect; GetClientRect(appMain.hWnd, &rect);
//	mp.y = rect.bottom-mp.y;
//	BYTE bArray[4];
//	glReadPixels(mp.x, mp.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, bArray);
//	int iResult = GetIndexByColor(bArray[0], bArray[1], bArray[2]) - 1;
//
//	return iResult;
//}
//

//*-----------------------------------------------
//
//Name:    Get3DRayUnderMouse
//
//Params:  v1, v2 - results storage
//
//Result:  Retrieves 3D ray under cursor from near
//		 to far plane.
//
//*---------------------------------------------*/

//void Get3DRayUnderMouse(glm::vec3* v1, glm::vec3* v2)
//{
//	HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
//
//	POINT mp; GetCursorPos(&mp);
//	ScreenToClient(hWnd, &mp);
//	RECT rect; GetClientRect(hWnd, &rect);
//	mp.y = rect.bottom-mp.y;
//
//	glm::vec4 viewport = glm::vec4(0.0f, 0.0f, appMain.oglControl.GetViewportWidth(), appMain.oglControl.GetViewportHeight());
//
//	*v1 = glm::unProject(glm::vec3(float(mp.x), float(mp.y), 0.0f), cCamera.Look(), *appMain.oglControl.GetProjectionMatrix(), viewport);
//	*v2 = glm::unProject(glm::vec3(float(mp.x), float(mp.y), 1.0f), cCamera.Look(), *appMain.oglControl.GetProjectionMatrix(), viewport);
//}

//*-----------------------------------------------
//
//Name:    RaySphereCollision
//
//Params:  vSphereCenter - guess what it is
//	     fSphereRadius - guess what it is
//		 vA, vB - two points of ray
//
//Result:  Checks if a ray given by two points
//		 collides with sphere.
//
//*---------------------------------------------*/

//bool RaySphereCollision(glm::vec3 vSphereCenter, float fSphereRadius, glm::vec3 vA, glm::vec3 vB)
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