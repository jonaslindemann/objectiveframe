#ifndef _CIvfButtonGroup_h_
#define _CIvfButtonGroup_h_

#include <ivf/Composite.h>

class CIvfButtonGroup : public ivf::Composite {
private:
    int m_checkIdx;
public:
    CIvfButtonGroup();
    virtual ~CIvfButtonGroup();

    void check(int idx);
    void clearChecked();

    void recheck();
};

#endif
