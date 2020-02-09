#pragma once

class SpectrumPoint
{
public:
	SpectrumPoint(void);
	~SpectrumPoint(void);
	bool InsideRect(int x, int y);
	void SetFixed(bool _bFixed){ m_bFixed = _bFixed; }
	bool GetFixed(){ return m_bFixed; }
	CRect m_rt;
	COLORREF m_color;
	bool m_bExist;
	bool m_bDrag;
private:
	bool m_bFixed;
};

