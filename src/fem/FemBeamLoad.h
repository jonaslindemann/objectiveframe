#ifndef _CFemBeamLoad_H_
#define _CFemBeamLoad_H_

#include "FemElementLoad.h"
#include "FemBeam.h"

FemSmartPointer(FemBeamLoad);

class FemBeamLoad : public FemElementLoad {
private:
    std::string m_name;
    int m_color;
public:
    FemBeamLoad();
    virtual ~FemBeamLoad ();

    FemClassInfo("FemBeamLoad",FemElementLoad);

    const std::string getName();
    void setName(const std::string& name);
    void setColor(int color);
    int getColor();

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
};
#endif
