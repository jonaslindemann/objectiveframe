#include "FemBeamNodeLoad.h"

// ------------------------------------------------------------
FemBeamNodeLoad::FemBeamNodeLoad()
    :FemNodeLoad()
{
    m_name = "Noname";
    m_color = 1;
}

// ------------------------------------------------------------
FemBeamNodeLoad::~FemBeamNodeLoad()
{
}

// ------------------------------------------------------------
void FemBeamNodeLoad::setName(const std::string& name)
{
    m_name = name;
}

// ------------------------------------------------------------
const std::string FemBeamNodeLoad::getName()
{
    return m_name;
}

// ------------------------------------------------------------
void FemBeamNodeLoad::saveToStream(std::ostream &out)
{
    FemNodeLoad::saveToStream(out);
    if (m_name=="")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

// ------------------------------------------------------------
void FemBeamNodeLoad::readFromStream(std::istream &in)
{
    char buffer[255];
    FemNodeLoad::readFromStream(in);
    in.getline(buffer,255);
    in.getline(buffer,255);
    this->setName(buffer);
    in >> m_color;
}

// ------------------------------------------------------------
void FemBeamNodeLoad::setColor(int color)
{
    m_color = color;
}

// ------------------------------------------------------------
int FemBeamNodeLoad::getColor()
{
    return m_color;
}
