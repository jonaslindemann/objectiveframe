// Implementation of: public class CFemBeamLoad

#include <ofem/beam_load.h>

using namespace ofem;
using namespace std;

// ------------------------------------------------------------
BeamLoad::BeamLoad ()
    :ElementLoad(),
     m_name{"Noname"},
     m_color{1}
{
}

// ------------------------------------------------------------
BeamLoad::~BeamLoad ()
{
}

// ------------------------------------------------------------
void BeamLoad::setName(const std::string& name)
{
    m_name = name;
}

// ------------------------------------------------------------
const std::string BeamLoad::getName()
{
    return m_name;
}

// ------------------------------------------------------------
void BeamLoad::saveToStream(std::ostream &out)
{
    ElementLoad::saveToStream(out);
    if (m_name=="")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

// ------------------------------------------------------------
void BeamLoad::readFromStream(std::istream &in)
{
    char buffer[255];
    ElementLoad::readFromStream(in);
    in.getline(buffer,255);
    in.getline(buffer,255);
    this->setName(buffer);
    in >> m_color;
}

// ------------------------------------------------------------
void BeamLoad::setColor(int color)
{
    m_color = color;
}

// ------------------------------------------------------------
int BeamLoad::getColor()
{
    return m_color;
}

