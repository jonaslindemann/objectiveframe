#ifndef _CFemCoord_H_
#define _CFemCoord_H_

#include "FemObject.h"

FemSmartPointer(FemCoord);

class FemCoord : public FemObject {
private:
    double m_coord[3];
public:
    FemCoord ();
    virtual ~FemCoord ();

    FemClassInfo("FemCoord",FemObject);

    void setCoord(double x, double y, double z);
    void getCoord(double &x, double &y, double &z);

    void readFromStream(std::istream &in);
    void saveToStream(std::ostream &out);
};
#endif
