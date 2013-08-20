#ifndef _CFemCoord_H_
#define _CFemCoord_H_

#include "FemObject.h"

FemSmartPointer(CFemCoord);

class CFemCoord : public CFemObject {
private:
    double m_coord[3];
public:
    CFemCoord ();
    virtual ~CFemCoord ();

    FemClassInfo("CFemCoord",CFemObject);

    void setCoord(double x, double y, double z);
    void getCoord(double &x, double &y, double &z);

    void readFromStream(std::istream &in);
    void saveToStream(std::ostream &out);
};
#endif
