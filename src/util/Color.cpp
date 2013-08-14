// Color.cpp: implementation of the CColor class.
//
//////////////////////////////////////////////////////////////////////

#include "Color.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColor::CColor()
{
	m_color[0] = 0.8f;
	m_color[1] = 0.8f;
	m_color[2] = 0.8f;
	m_color[3] = 1.0f;
}

CColor::~CColor()
{

}

void CColor::setColor(float red, float green, float blue, float alpha)
{
	m_color[0] = red;
	m_color[1] = green;
	m_color[2] = blue;
	m_color[3] = alpha;
}

void CColor::setAlpha(float alpha)
{
	m_color[3] = alpha;
}

void CColor::getColor(float *array)
{
	array[0] = m_color[0];
	array[1] = m_color[1];
	array[2] = m_color[2];
}

void CColor::getColor(float &r, float &g, float &b, float &a)
{
	r = m_color[0];
	g = m_color[1];
	b = m_color[2];
	a = m_color[3];
}
