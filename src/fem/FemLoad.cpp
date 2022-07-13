// Implementation of: public class CFemLoad

#include "FemLoad.h"

// ------------------------------------------------------------
FemLoad::FemLoad ()
    :FemObject()
{
    m_number = -1;
    m_value = 0.0;
}

// ------------------------------------------------------------
FemLoad::~FemLoad ()
{
}

// ------------------------------------------------------------
void FemLoad::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long FemLoad::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void FemLoad::print(std::ostream &out)
{

}

// ------------------------------------------------------------
void FemLoad::saveToStream(std::ostream &out)
{
    using namespace std;
    out << m_number << " " << m_value << endl;
}

// ------------------------------------------------------------
void FemLoad::readFromStream(std::istream &in)
{
    in >> m_number;
    in >> m_value;
}

// ------------------------------------------------------------
void FemLoad::setValue(double value)
{
    m_value = value;
}

// ------------------------------------------------------------
double FemLoad::getValue()
{
    return m_value;
}

