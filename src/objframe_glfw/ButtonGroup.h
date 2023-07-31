#ifndef _CIvfButtonGroup_h_
#define _CIvfButtonGroup_h_

#include <ivf/Composite.h>

IvfSmartPointer(ButtonGroup);

class ButtonGroup : public ivf::Composite {
private:
    int m_checkIdx;

public:
    ButtonGroup();
    virtual ~ButtonGroup();

    IvfClassInfo("ButtonGroup", ivf::Composite);
    IvfStdFactory(ButtonGroup);

    void check(int idx);
    void clearChecked();

    void recheck();
};

#endif
