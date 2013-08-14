// Implementation of: public class CFemLoad

#include "FemLoad.h"

// ------------------------------------------------------------
CFemLoad::CFemLoad ()
		:CFemObject()
{
	m_number = -1;
	m_value = 0.0;
}

// ------------------------------------------------------------
CFemLoad::~CFemLoad ()
{
}

// ------------------------------------------------------------
void CFemLoad::setNumber(long number)
{
	m_number = number;
}

// ------------------------------------------------------------
long CFemLoad::getNumber()
{
	return m_number;
}

// ------------------------------------------------------------
void CFemLoad::print(std::ostream &out)
{

}

// ------------------------------------------------------------
void CFemLoad::saveToStream(std::ostream &out)
{
	using namespace std;
	out << m_number << " " << m_value << endl;
}

// ------------------------------------------------------------
void CFemLoad::readFromStream(std::istream &in)
{
	in >> m_number;
	in >> m_value;
}

// ------------------------------------------------------------
void CFemLoad::setValue(double value)
{
	m_value = value;
}

// ------------------------------------------------------------
double CFemLoad::getValue()
{
	return m_value;
}

