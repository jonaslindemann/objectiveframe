// Implementation of: public class CFemBeamLoad

#include "FemBeamLoad.h"

// ------------------------------------------------------------
FemBeamLoad::FemBeamLoad ()
    :FemElementLoad(),
     m_name{"Noname"},
     m_color{1}
{
}

// ------------------------------------------------------------
FemBeamLoad::~FemBeamLoad ()
{
}

// ------------------------------------------------------------
void FemBeamLoad::setName(const std::string& name)
{
    m_name = name;
}

// ------------------------------------------------------------
const std::string FemBeamLoad::getName()
{
    return m_name;
}

// ------------------------------------------------------------
void FemBeamLoad::saveToStream(std::ostream &out)
{
    FemElementLoad::saveToStream(out);
    if (m_name=="")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

// ------------------------------------------------------------
void FemBeamLoad::readFromStream(std::istream &in)
{
    char buffer[255];
    FemElementLoad::readFromStream(in);
    in.getline(buffer,255);
    in.getline(buffer,255);
    this->setName(buffer);
    in >> m_color;
}

// ------------------------------------------------------------
void FemBeamLoad::setColor(int color)
{
    m_color = color;
}

// ------------------------------------------------------------
int FemBeamLoad::getColor()
{
    return m_color;
}

