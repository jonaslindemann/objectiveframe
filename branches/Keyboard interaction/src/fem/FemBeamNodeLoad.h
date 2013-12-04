#ifndef _CFemBeamNodeLoad_
#define _CFemBeamNodeLoad_

#include "FemNodeLoad.h"

FemSmartPointer(CFemBeamNodeLoad);

class CFemBeamNodeLoad : public CFemNodeLoad {
private:
    int m_color;
    std::string m_name;
public:
    CFemBeamNodeLoad();
    virtual ~CFemBeamNodeLoad();

    FemClassInfo("CFemBeamNodeLoad",CFemNodeLoad);

    void setColor(int color);
    int getColor();

    void setName(const std::string& name);
    const std::string getName();

    void readFromStream(std::istream &in);
    void saveToStream(std::ostream &out);
};

#endif // !defined(AFX_FEMBEAMNODELOAD_H__1FBC0344_980C_11D3_94C0_004005A46C57__INCLUDED_)
