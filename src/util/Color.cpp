// Color.cpp: implementation of the CColor class.
//
//////////////////////////////////////////////////////////////////////

#include "Color.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ColorEntry::ColorEntry()
{
    m_color[0] = 0.8f;
    m_color[1] = 0.8f;
    m_color[2] = 0.8f;
    m_color[3] = 1.0f;
}

ColorEntry::~ColorEntry()
{
}

std::shared_ptr<ColorEntry> ColorEntry::create()
{
    return std::make_shared<ColorEntry>();
}

void ColorEntry::setColor(float red, float green, float blue, float alpha)
{
    m_color[0] = red;
    m_color[1] = green;
    m_color[2] = blue;
    m_color[3] = alpha;
}

void ColorEntry::setAlpha(float alpha)
{
    m_color[3] = alpha;
}

void ColorEntry::getColor(float *array)
{
    array[0] = m_color[0];
    array[1] = m_color[1];
    array[2] = m_color[2];
}

void ColorEntry::getColor(float &r, float &g, float &b, float &a)
{
    r = m_color[0];
    g = m_color[1];
    b = m_color[2];
    a = m_color[3];
}
