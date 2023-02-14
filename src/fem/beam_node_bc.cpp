#include <ofem/beam_node_bc.h>

using namespace ofem;

BeamNodeBC::BeamNodeBC()
    : NodeBC()
{
    m_name = "Noname";
    m_color = 1;
}

BeamNodeBC::~BeamNodeBC()
{
}

void BeamNodeBC::setName(const std::string& name)
{
    m_name = name;
}

const std::string BeamNodeBC::getName()
{
    return m_name;
}

void BeamNodeBC::saveToStream(std::ostream& out)
{
    using namespace std;
    NodeBC::saveToStream(out);
    if (m_name == "")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

void BeamNodeBC::readFromStream(std::istream& in)
{
    char buffer[255];
    NodeBC::readFromStream(in);
    in.getline(buffer, 255);
    in.getline(buffer, 255);
    this->setName(buffer);
    if ((this->getName() == "fixed pos/rot") || (this->getName() == "fixed pos"))
        this->setReadOnly(true);
    in >> m_color;
}

void BeamNodeBC::setColor(int color)
{
    m_color = color;
}

int BeamNodeBC::getColor()
{
    return m_color;
}
