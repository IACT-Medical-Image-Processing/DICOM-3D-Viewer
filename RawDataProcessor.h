#pragma once


// CRawDataProcessor
class CDICOMViewerDoc;

class CRawDataProcessor
{
public:
	CRawDataProcessor();
	virtual ~CRawDataProcessor();

    // Call this only after the open gl is initialized.
    bool ReadFile(CDICOMViewerDoc* _pDoc, int nWidth_i, int nHeight_i, int nSlices_i );

	int GetTexture3D()
    {
        return m_nTexId;
    }
    
	const int GetWidth()
    {
        return m_uImageWidth;
    }
    
	const int GetHeight()
    {
        return m_uImageHeight;
    }
    
	const int GetDepth()
    {
        return m_uImageCount;
    }

private:
    int m_uImageCount;
    int m_uImageWidth;
    int m_uImageHeight;
    int m_nTexId;
};


