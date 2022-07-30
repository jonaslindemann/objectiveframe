#pragma once

#include <ofem/base.h>

namespace ofem
{

SmartPointer(Coord);

class Coord : public Base
{
private:
    double m_coord[3];

public:
    Coord();
    virtual ~Coord();

    ClassInfo("Coord", Base);

    void setCoord(double x, double y, double z);
    void getCoord(double& x, double& y, double& z);

    virtual void readFromStream(std::istream& in) override;
    virtual void saveToStream(std::ostream& out) override;
};
}
