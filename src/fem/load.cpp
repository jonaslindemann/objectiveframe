#include <ofem/load.h>

using namespace ofem;

// ------------------------------------------------------------
Load::Load ()
    :Base()
{
    m_number = -1;
    m_value = 0.0;
}

// ------------------------------------------------------------
Load::~Load ()
{
}

// ------------------------------------------------------------
void Load::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long Load::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void Load::print(std::ostream &out)
{

}

// ------------------------------------------------------------
void Load::saveToStream(std::ostream &out)
{
    using namespace std;
    out << m_number << " " << m_value << endl;
}

// ------------------------------------------------------------
void Load::readFromStream(std::istream &in)
{
    in >> m_number;
    in >> m_value;
}

// ------------------------------------------------------------
void Load::setValue(double value)
{
    m_value = value;
}

// ------------------------------------------------------------
double Load::getValue()
{
    return m_value;
}

