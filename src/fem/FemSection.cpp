// Implementation of: public class CFemSection

#include "FemSection.h"

#ifdef HAVE_CMATH
#include <cmath>
using namespace std;
#else
#include <math.h>
#endif

// ------------------------------------------------------------
CFemSection::CFemSection ()
		:CFemObject()
{
	m_nbrOfProps = 0;
	m_autoCalc = false;
	m_sectionType = -1;
}

// ------------------------------------------------------------
CFemSection::~CFemSection ()
{
	this->clear();
}

// ------------------------------------------------------------
void CFemSection::addPoint(double x, double y)
{
	CFemCoord* coord = new CFemCoord();	
	coord->setCoord(x, y, 0.0);
	m_coords.push_back(coord);
}

// ------------------------------------------------------------
void CFemSection::clear()
{
	for (unsigned int i=0; i<m_coords.size(); i++)
		delete m_coords[i];
	m_coords.clear();
}

// ------------------------------------------------------------
void CFemSection::getCoord(unsigned int idx, double &x, double &y)
{
	double z;
	if (idx<m_coords.size())
		m_coords[idx]->getCoord(x, y, z);
}

// ------------------------------------------------------------
unsigned int CFemSection::getSize()
{
	return m_coords.size();
}

// ------------------------------------------------------------
void CFemSection::getNormal(unsigned int idx, double &ex, double &ey)
{
	double x1, y1, z;
	double x2, y2, l;
	if (idx<m_coords.size()-1)
	{
		m_coords[idx]->getCoord(x1, y1, z);
		m_coords[idx+1]->getCoord(x2, y2, z);
		if (y1==y2) 
		{
			ex = 0.0;
			if (x2>=x1)
				ey = 1.0;
			else
				ey = -1.0;
		} 
		else if (x1==x2)
		{
			ey = 0.0;
			if (y2<y1)
				ex = 1.0;
			else
				ex = -1.0;
		}
		else 
		{
			ex = -(y2-y1);
			ey = x2-x1;
			l = sqrt(pow(ex,2)+pow(ey,2));
			ex = ex/l;
			ey = ey/l;
		}
		ex = -ex;
		ey = -ey;
	}
}

// ------------------------------------------------------------
void CFemSection::setPropSize(int size)
{
	m_nbrOfProps = size;
	/*if (m_prop != NULL) 
		delete [] m_prop;*/
	m_prop = new double[m_nbrOfProps];
	for (unsigned int i=0; i<m_nbrOfProps; i++)
		m_prop[i] = 0;
}

// ------------------------------------------------------------
void CFemSection::getPropSize(int &size)
{
	size = m_nbrOfProps;
}

// ------------------------------------------------------------
void CFemSection::getAllProps(double* &props)
{
	props = m_prop;
}

// ------------------------------------------------------------
void CFemSection::setDataSize(int size)
{
	m_nbrOfData = size;
	/*if (m_prop != NULL) 
		delete [] m_prop;*/
	m_data = new double[m_nbrOfData];
	for (unsigned int i=0; i<m_nbrOfData; i++)
		m_data[i] = 0;
}

// ------------------------------------------------------------
void CFemSection::getDataSize(int &size)
{
	size = m_nbrOfData;
}

// ------------------------------------------------------------
void CFemSection::getAllData(double* &data)
{
	data = m_data;
}

// ------------------------------------------------------------
void CFemSection::setAutoCalc(bool mode)
{
	m_autoCalc = mode;
}

// ------------------------------------------------------------
bool CFemSection::autoCalc()
{
	return m_autoCalc;
}

void CFemSection::saveToStream(std::ostream &out)
{
	unsigned int i;

	// Write section properties

	out << m_nbrOfProps << endl;

	for (i=0; i<m_nbrOfProps; i++)
		out << m_prop[i] << " ";
	out << endl;

	// Write material properties

	out << m_nbrOfData << endl;

	for (i=0; i<m_nbrOfData; i++)
		out << m_data[i] << " ";
	out << endl;

	// Write section geometry

	out << m_coords.size() << endl;

	for (i=0; i<m_coords.size(); i++)
		m_coords[i]->saveToStream(out);

}

void CFemSection::setSectionType(int type)
{
	m_sectionType = type;
}

int CFemSection::getSectionType()
{
	return m_sectionType;
}

void CFemSection::readFromStream(std::istream &in)
{
	int i, propSize,  dataSize, coordSize;

	// Write section properties

	in >> propSize;
	this->setPropSize(propSize);

	for (i=0; i<propSize; i++)
		in >> m_prop[i];

	// Write material properties

	in >> dataSize;
	this->setDataSize(dataSize);

	for (i=0; i<dataSize; i++)
		in >> m_data[i];

	// Write section geometry

	in >> coordSize;
	this->clear();
	for (i=0; i<coordSize; i++)
	{
		CFemCoord* coord = new CFemCoord();
		coord->readFromStream(in);
		m_coords.push_back(coord);
	}
}

void CFemSection::getExcY(double &emax, double &emin)
{
	emax = -1.0;
	emin = -1.0;
}

void CFemSection::getExcZ(double &emax, double &emin)
{
	emax = -1.0;
	emin = -1.0;
}
