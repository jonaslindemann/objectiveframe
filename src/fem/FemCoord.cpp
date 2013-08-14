// Implementation of: public class CFemCoord

#include "FemCoord.h"

// ------------------------------------------------------------
CFemCoord::CFemCoord ()
		:CFemObject()
{
	m_coord[0] = 0.0;
	m_coord[1] = 0.0;
	m_coord[2] = 0.0;
}

// ------------------------------------------------------------
CFemCoord::~CFemCoord ()
{
}

// ------------------------------------------------------------
void CFemCoord::setCoord(double x, double y, double z)
{
	m_coord[0] = x;
	m_coord[1] = y;
	m_coord[2] = z;
}

// ------------------------------------------------------------
void CFemCoord::getCoord(double &x, double &y, double &z)
{
	x = m_coord[0];
	y = m_coord[1];
	z = m_coord[2];
}

void CFemCoord::saveToStream(std::ostream &out)
{
	using namespace std;
	out << m_coord[0] << " " << m_coord[1] << " " << m_coord[2] << endl;
}

void CFemCoord::readFromStream(std::istream &in)
{
	in >> m_coord[0] >> m_coord[1] >> m_coord[2];
}
