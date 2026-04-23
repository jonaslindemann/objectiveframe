// IvfButtonGroup.cpp: implementation of the CIvfButtonGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "ButtonGroup.h"
#include "PlaneButton.h"

#include <ivf/GenericButton.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ButtonGroup::ButtonGroup()
{
    this->setUseName(false);
    m_checkIdx = -1;
}

ButtonGroup::~ButtonGroup()
{
}

PlaneButton* ButtonGroup::isUnder(int x, int y)
{
    double parent_x, parent_y, parent_z;
    double button_w, button_h;
    double button_x, button_y, button_z;
    double px = x;
    double py = y;
    double x0, y0, x1, y1;

    this->getPosition(parent_x, parent_y, parent_z);

    for (auto i=0; i<this->getSize(); i++)
    {
        auto button = static_cast<PlaneButton *>(this->getChild(i));
        button->getSize(button_w, button_h);
        button->getPosition(button_x, button_y, button_z);

        x0 = parent_x + button_x - button_w/2.0;
        x1 = parent_x + button_x + button_w/2.0;

        y0 = parent_y + button_y - button_h/2.0;
        y1 = parent_y + button_y + button_h/2.0;

        if ((px>x0)&&(px<x1)&&(py>y0)&&(py<y1))
            return button;
    }

    return nullptr;
}

void ButtonGroup::clearChecked()
{
    int i;

    for (i = 0; i < this->getSize(); i++) {
        ivf::GenericButton *button = static_cast<ivf::GenericButton *>(this->getChild(i));
        button->setButtonState(ivf::GenericButton::BS_NORMAL);
    }
}

void ButtonGroup::check(int idx)
{
    if ((idx >= 0) && (idx < this->getSize())) {
        m_checkIdx = idx;
        this->clearChecked();
        ivf::GenericButton *button = static_cast<ivf::GenericButton *>(this->getChild(idx));
        button->setButtonState(ivf::GenericButton::BS_CHECKED);
    }
}

void ButtonGroup::recheck()
{
    if (m_checkIdx != -1)
        this->check(m_checkIdx);
}
