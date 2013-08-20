// Implementation of: public class CFemMaterial

#include "FemMaterial.h"

// ------------------------------------------------------------
CFemMaterial::CFemMaterial ()
    :CFemObject()
    //TODO: check and complete member initialisation list!
{
    m_number = -1;
}

// ------------------------------------------------------------
CFemMaterial::~CFemMaterial ()
{
}

// ------------------------------------------------------------
void CFemMaterial::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long CFemMaterial::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void CFemMaterial::print(std::ostream &out)
{
    using namespace std;
    CFemObject::print(out);
    out << "Material " << this->getNumber() << endl;
}

// ------------------------------------------------------------
void CFemMaterial::saveToStream(std::ostream &out)
{
    using namespace std;
    CFemObject::saveToStream(out);
    out << this->getNumber() << endl;
}

// ------------------------------------------------------------
void CFemMaterial::readFromStream(std::istream &in)
{
    CFemObject::readFromStream(in);
    in >> m_number;
}

