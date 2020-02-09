#include "StdAfx.h"
#include "SpectrumPoint.h"


SpectrumPoint::SpectrumPoint(void)
{
	m_bExist = false;
	m_bDrag = false;
	m_bFixed = false;
}


SpectrumPoint::~SpectrumPoint(void)
{
}

bool SpectrumPoint::InsideRect(int x, int y)
{
	if((m_rt.left <= x)&&(x <= m_rt.right)
		&&(m_rt.top <= y)&&(y <= m_rt.bottom))
	{
		return true;
	}

	return false;
}