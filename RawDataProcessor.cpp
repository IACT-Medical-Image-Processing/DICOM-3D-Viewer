#include "StdAfx.h"
#include "RawDataProcessor.h"
#include "DICOM ViewerDoc.h"
#include "Scale.h"

CRawDataProcessor::CRawDataProcessor(void) : 
m_uImageWidth(0)
,m_uImageHeight(0)
,m_uImageCount(0)
,m_nTexId(0)
{
}


CRawDataProcessor::~CRawDataProcessor(void)
{
	// If not initialized, then this will be zero. So no checking is needed.
	if( 0 != m_nTexId )
	{
		glDeleteTextures( 1, (GLuint*)&m_nTexId );
	}    
}

bool CRawDataProcessor::ReadFile(CDICOMViewerDoc* _pDoc, int nWidth_i, int nHeight_i, int nSlices_i )
{	
	// File has only image data. The dimension of the data should be known.
    m_uImageCount = nSlices_i;
    m_uImageWidth = nWidth_i;
    m_uImageHeight = nHeight_i;

    // Holds the RGBA buffer
    char* pRGBABuffer = new char[ m_uImageWidth*m_uImageHeight*m_uImageCount*4 ];
    if( !pRGBABuffer)
    {
        return false;
    }

    // Convert the data to RGBA data.
    // Here we are simply putting the same value to R, G, B and A channels.
    // Usually for raw data, the alpha value will be constructed by a threshold value given by the user 
	for(int z = 0; z < nSlices_i; z++)
	{
		for(int y = 0; y < nHeight_i; y++)
		{
			for(int x = 0; x < nWidth_i; x++)
			{
				pRGBABuffer[z*512*512*4+y*512*4+x*4] = 0;
				pRGBABuffer[z*512*512*4+y*512*4+x*4+1]= 0;
				pRGBABuffer[z*512*512*4+y*512*4+x*4+2]= 0;
				pRGBABuffer[z*512*512*4+y*512*4+x*4+3]= 0;
				
				int lux = m_Volume[z][y][x];
			//	int lux_d = DICOMtoImage(lux, _pDoc->m_HUmin, _pDoc->m_HUmax);

				if((_pDoc->m_startHisto < lux)&&(lux < _pDoc->m_endHisto))
				{
					COLORREF color = GetKeyColor(lux);
					pRGBABuffer[z*512*512*4+y*512*4+x*4] = GetRValue(color);
					pRGBABuffer[z*512*512*4+y*512*4+x*4+1]= GetGValue(color);
					pRGBABuffer[z*512*512*4+y*512*4+x*4+2]= GetBValue(color);
					pRGBABuffer[z*512*512*4+y*512*4+x*4+3]= lux;
				}
			}
		}
	}

    // If this function is getting called again for another data file.
    // Deleting and creating texture is not a good idea, 
    // we can use the glTexSubImage3D for better performance for such scenario.
    // I am not using that now :-)
    if( 0 != m_nTexId )
    {
        glDeleteTextures( 1, (GLuint*)&m_nTexId );
    }
    glGenTextures(1,(GLuint*)&m_nTexId );

    glBindTexture( GL_TEXTURE_3D, m_nTexId );
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, m_uImageWidth, m_uImageHeight , m_uImageCount, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pRGBABuffer );
    glBindTexture( GL_TEXTURE_3D, 0 );

    delete[] pRGBABuffer;

	return true;
}