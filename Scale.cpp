
#include "stdafx.h"
#include "Scale.h"
#include "MainFrm.h"
#include "Panel.h"
#include "Spectrum.h"
#include "DICOM ViewerDoc.h"

void DICOMtoImage(int min, int max)
{
	int value, x, y, z;

	for (z=0; z<512; z++)
		for (y=0; y<512; y++)
			for (x=0; x<512; x++)
			{
				value = 255 * 2.5 * (m_DicomData[z][y][x]- min)/(max - min) - 50;
				
				if(value < 0)		value = 0;
				if(value > 255)	value = 255;

				m_Volume[z][y][x] = value;
			}
}

COLORREF GetKeyColor(int input)
{
	static Spectrum* sp = (Spectrum*)&(((CMainFrame *)AfxGetMainWnd())->GetPanel()->m_SpectDlg);
	if(sp == NULL)
		return 0;

	return sp->m_key_point[input].m_color;
}
