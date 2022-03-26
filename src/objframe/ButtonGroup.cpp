// IvfButtonGroup.cpp: implementation of the CIvfButtonGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "ButtonGroup.h"

#include <ivf/GenericButton.h>

using namespace ivf;

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
        GenericButton* button = static_cast<GenericButton*>(this->getChild(i));
        button->setButtonState(GenericButton::BS_NORMAL);
    }
}

void CIvfButtonGroup::check(int idx)
{
    if ((idx>=0)&&(idx<this->getSize()))
    {
        m_checkIdx = idx;
        this->clearChecked();
        GenericButton* button = static_cast<GenericButton*>(this->getChild(idx));
        button->setButtonState(GenericButton::BS_CHECKED);
    }
}


void CIvfButtonGroup::recheck()
{
    if (m_checkIdx!=-1)
        this->check(m_checkIdx);
}
