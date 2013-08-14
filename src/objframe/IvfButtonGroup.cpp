// IvfButtonGroup.cpp: implementation of the CIvfButtonGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "IvfButtonGroup.h"

#include <ivf/IvfGenericButton.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIvfButtonGroup::CIvfButtonGroup()
{
	this->setUseName(false);
	m_checkIdx = -1;
}

CIvfButtonGroup::~CIvfButtonGroup()
{

}

void CIvfButtonGroup::clearChecked()
{
	int i;

	for (i=0; i<this->getSize(); i++)
	{
		CIvfGenericButton* button = (CIvfGenericButton*) this->getChild(i);
		button->setButtonState(CIvfGenericButton::BS_NORMAL);
	}
}

void CIvfButtonGroup::check(int idx)
{
	if ((idx>=0)&&(idx<this->getSize()))
	{
		m_checkIdx = idx;
		this->clearChecked();
		CIvfGenericButton* button = (CIvfGenericButton*) this->getChild(idx);
		button->setButtonState(CIvfGenericButton::BS_CHECKED);
	}
}


void CIvfButtonGroup::recheck()
{
	if (m_checkIdx!=-1)
		this->check(m_checkIdx);
}
