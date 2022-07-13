#ifndef _CFemBeamNodeBC_h_
#define _CFemBeamNodeBC_h_

#include "FemNodeBC.h"

FemSmartPointer(FemBeamNodeBC);

class FemBeamNodeBC : public FemNodeBC {
private:
    int m_color;
    std::string m_name;
public:
    FemBeamNodeBC();
    virtual ~FemBeamNodeBC();

    FemClassInfo("FemBeamNodeBC",FemNodeBC);

    int getColor();
    void setColor(int color);
    const std::string getName();
    void setName(const std::string& name);

    void readFromStream(std::istream &in);
    void saveToStream(std::ostream &out);
};

#endif
