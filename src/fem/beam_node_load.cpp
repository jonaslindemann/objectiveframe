#include <ofem/beam_node_load.h>

using namespace ofem;
using namespace std;

BeamNodeLoad::BeamNodeLoad() : NodeLoad(), m_name{"Noname"}, m_color{1}, m_scale{1.0}
{
}

BeamNodeLoad::~BeamNodeLoad()
{
}

void BeamNodeLoad::setName(const std::string &name)
{
    m_name = name;
}

const std::string BeamNodeLoad::getName()
{
    return m_name;
}

void ofem::BeamNodeLoad::setScale(double scale)
{
    m_scale = scale;
}

double ofem::BeamNodeLoad::getScale()
{
    return m_scale;
}

void BeamNodeLoad::saveToStream(std::ostream &out)
{
    NodeLoad::saveToStream(out);
    if (m_name == "")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

void BeamNodeLoad::readFromStream(std::istream &in)
{
    char buffer[255];
    NodeLoad::readFromStream(in);
    in.getline(buffer, 255);
    in.getline(buffer, 255);
    this->setName(buffer);
    in >> m_color;
}

void BeamNodeLoad::setColor(int color)
{
    m_color = color;
}

int BeamNodeLoad::getColor()
{
    return m_color;
}
