#include "IvfColorTable.h"

// ------------------------------------------------------------
CIvfColorTable::CIvfColorTable()
:CIvfBase()
{
	// Set initial color yellow

	for (int i=0; i<256; i++)
	{
		m_colorTable[i][0] = 1.0;
		m_colorTable[i][1] = 1.0;
		m_colorTable[i][2] = 0.0;
	}
}

// ------------------------------------------------------------
CIvfColorTable::~CIvfColorTable()
{

}

// ------------------------------------------------------------
void CIvfColorTable::getColor(int idx, float &r, float &g, float &b)
{
	if ( (idx>=0)&&(idx<256) )
	{
		r = m_colorTable[idx][0];
		g = m_colorTable[idx][1];
		b = m_colorTable[idx][2];
	}
}

// ------------------------------------------------------------
void CIvfColorTable::assignColor(int idx, CIvfMaterial *material)
{
	if ( (idx>=0)&&(idx<256) )
	{
		material->setDiffuseColor(
			m_colorTable[idx][0],
			m_colorTable[idx][1],
			m_colorTable[idx][2],1.0f);
		material->setAmbientColor(
			m_colorTable[idx][0]*0.25,
			m_colorTable[idx][1]*0.25,
			m_colorTable[idx][2]*0.25,1.0f);
	}
}

// ------------------------------------------------------------
void CIvfColorTable::setColor(int idx, float r, float g, float b)
{
	if ( (idx>=0)&&(idx<256) )
	{
		m_colorTable[idx][0] = r;
		m_colorTable[idx][1] = g;
		m_colorTable[idx][2] = b;
	}	
}
