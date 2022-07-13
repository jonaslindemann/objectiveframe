#include "FemBeamNodeBC.h"

// ------------------------------------------------------------
FemBeamNodeBC::FemBeamNodeBC()
    :FemNodeBC()
{
    m_name = "Noname";
    m_color = 1;
}

// ------------------------------------------------------------
FemBeamNodeBC::~FemBeamNodeBC()
{
}

// ------------------------------------------------------------
void FemBeamNodeBC::setName(const std::string& name)
{
    m_name = name;
}

// ------------------------------------------------------------
const std::string FemBeamNodeBC::getName()
{
    return m_name;
}

// ------------------------------------------------------------
void FemBeamNodeBC::saveToStream(std::ostream &out)
{
    using namespace std;
    FemNodeBC::saveToStream(out);
    if (m_name=="")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

// ------------------------------------------------------------
void FemBeamNodeBC::readFromStream(std::istream &in)
{
    char buffer[255];
    FemNodeBC::readFromStream(in);
    in.getline(buffer,255);
    in.getline(buffer,255);
    this->setName(buffer);
    in >> m_color;
}

// ------------------------------------------------------------
void FemBeamNodeBC::setColor(int color)
{
    m_color = color;
}

// ------------------------------------------------------------
int FemBeamNodeBC::getColor()
{
    return m_color;
}
