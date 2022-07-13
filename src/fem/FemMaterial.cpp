// Implementation of: public class CFemMaterial

#include "FemMaterial.h"

// ------------------------------------------------------------
FemMaterial::FemMaterial ()
    :FemObject()
    //TODO: check and complete member initialisation list!
{
    m_number = -1;
}

// ------------------------------------------------------------
FemMaterial::~FemMaterial ()
{
}

// ------------------------------------------------------------
void FemMaterial::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long FemMaterial::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void FemMaterial::print(std::ostream &out)
{
    using namespace std;
    FemObject::print(out);
    out << "Material " << this->getNumber() << endl;
}

// ------------------------------------------------------------
void FemMaterial::saveToStream(std::ostream &out)
{
    using namespace std;
    FemObject::saveToStream(out);
    out << this->getNumber() << endl;
}

// ------------------------------------------------------------
void FemMaterial::readFromStream(std::istream &in)
{
    FemObject::readFromStream(in);
    in >> m_number;
}

