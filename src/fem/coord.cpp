#include <ofem/coord.h>

using namespace ofem;


Coord::Coord()
    : Base()
{
    m_coord[0] = 0.0;
    m_coord[1] = 0.0;
    m_coord[2] = 0.0;
}


Coord::~Coord()
{
}


void Coord::setCoord(double x, double y, double z)
{
    m_coord[0] = x;
    m_coord[1] = y;
    m_coord[2] = z;
}


void Coord::getCoord(double& x, double& y, double& z)
{
    x = m_coord[0];
    y = m_coord[1];
    z = m_coord[2];
}

void Coord::saveToStream(std::ostream& out)
{
    using namespace std;
    out << m_coord[0] << " " << m_coord[1] << " " << m_coord[2] << endl;
}

void Coord::readFromStream(std::istream& in)
{
    in >> m_coord[0] >> m_coord[1] >> m_coord[2];
}
