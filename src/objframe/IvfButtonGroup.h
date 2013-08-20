#ifndef _CIvfButtonGroup_h_
#define _CIvfButtonGroup_h_

#include <ivf/IvfComposite.h>

class CIvfButtonGroup : public CIvfComposite {
private:
    int m_checkIdx;
public:
    void recheck();
    CIvfButtonGroup();
    virtual ~CIvfButtonGroup();

    void check(int idx);
    void clearChecked();
};

#endif
