#pragma once

#include <ivf/Composite.h>
#include <PlaneButton.h>

IvfSmartPointer(ButtonGroup);

class ButtonGroup : public ivf::Composite {
private:
    int m_checkIdx;

public:
    ButtonGroup();
    virtual ~ButtonGroup();

    IvfClassInfo("ButtonGroup", ivf::Composite);
    IvfStdFactory(ButtonGroup);

    PlaneButton* isUnder(int x, int y);

    void check(int idx);
    void clearChecked();

    void recheck();
};
