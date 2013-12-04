#include "FemBeamNodeBC.h"

// ------------------------------------------------------------
CFemBeamNodeBC::CFemBeamNodeBC()
    :CFemNodeBC()
{
    m_name = "Noname";
    m_color = 1;
}

// ------------------------------------------------------------
CFemBeamNodeBC::~CFemBeamNodeBC()
{
}

// ------------------------------------------------------------
void CFemBeamNodeBC::setName(const std::string& name)
{
    m_name = name;
}

// ------------------------------------------------------------
const std::string CFemBeamNodeBC::getName()
{
    return m_name;
}

// ------------------------------------------------------------
void CFemBeamNodeBC::saveToStream(std::ostream &out)
{
    using namespace std;
    CFemNodeBC::saveToStream(out);
    if (m_name=="")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

// ------------------------------------------------------------
void CFemBeamNodeBC::readFromStream(std::istream &in)
{
    char buffer[255];
    CFemNodeBC::readFromStream(in);
    in.getline(buffer,255);
    in.getline(buffer,255);
    this->setName(buffer);
    in >> m_color;
}

// ------------------------------------------------------------
void CFemBeamNodeBC::setColor(int color)
{
    m_color = color;
}

// ------------------------------------------------------------
int CFemBeamNodeBC::getColor()
{
    return m_color;
}
