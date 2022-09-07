#include <ofem/material.h>

using namespace ofem;


Material::Material()
    : Base()
// TODO: check and complete member initialisation list!
{
    m_number = -1;
}


Material::~Material()
{
}


void Material::setNumber(long number)
{
    m_number = number;
}


long Material::getNumber()
{
    return m_number;
}


void Material::print(std::ostream& out)
{
    using namespace std;
    Base::print(out);
    out << "Material " << this->getNumber() << endl;
}


void Material::saveToStream(std::ostream& out)
{
    using namespace std;
    Base::saveToStream(out);
    out << this->getNumber() << endl;
}


void Material::readFromStream(std::istream& in)
{
    Base::readFromStream(in);
    in >> m_number;
}
