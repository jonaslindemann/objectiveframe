#include <ofem/node.h>

using namespace ofem;

// ------------------------------------------------------------
Node::Node()
    : Base()
{
    m_number = -1;
    m_coord[0] = 0.0;
    m_coord[1] = 0.0;
    m_coord[2] = 0.0;
    this->setKind(FEM_DISPL_ROT_NODE);
}

// ------------------------------------------------------------
Node::Node(double x, double y, double z)
    : Base()
{
    m_number = -1;
    this->setCoord(x, y, z);
    this->setKind(FEM_DISPL_ROT_NODE);
}

// ------------------------------------------------------------
Node::~Node()
{
}

// ------------------------------------------------------------
void Node::setCoord(double x, double y, double z)
{
    m_coord[0] = x;
    m_coord[1] = y;
    m_coord[2] = z;
}

// ------------------------------------------------------------
void Node::getCoord(double& x, double& y, double& z)
{
    x = m_coord[0];
    y = m_coord[1];
    z = m_coord[2];
}

// ------------------------------------------------------------
void Node::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long Node::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void Node::print(std::ostream& out)
{
    using namespace std;
    out << "Node " << m_number << endl;
    ;
    out << "   Location (" << m_coord[0] << ", ";
    out << m_coord[1] << ", ";
    out << m_coord[2] << ")" << endl;
    out << "   Local dofs:  ";
    unsigned int i;
    for (i = 0; i < m_dofs.size(); i++)
        out << m_dofs[i];
    out << endl;
    out << "   Global dofs: ";
    for (i = 0; i < m_dofs.size(); i++)
        out << m_dofs[i]->getNumber() << " ";
    out << endl;
}

json_nl Node::toJson()
{
    /*
    long m_number;
    int m_kind;
    double m_coord[3];
    std::vector<double> m_values;
    std::vector<DofPtr> m_dofs;
    */
    json_nl j;
    j["number"] = m_number;
    j["kind"] = m_kind;
    j["x"] = m_coord[0];
    j["y"] = m_coord[1];
    j["z"] = m_coord[2];
    j["values"] = m_values;

    json_nl jdofs;

    for (auto& dof : m_dofs)
        jdofs.push_back(dof->toJson());

    j["dofs"] = jdofs;

    return j;
}

void Node::fromJson(nlohmann::json& j)
{
}

// ------------------------------------------------------------
void Node::setKind(int kind)
{
    m_kind = kind;

    // Delete previous dofs

    unsigned int i;

    m_dofs.clear();

    // Create new dofs

    switch (m_kind)
    {
    case FEM_DISPL_NODE:
        for (i = 0; i < 3; i++)
        {
            DofPtr dof = new Dof(static_cast<DofKind>(i));
            m_dofs.push_back(dof);
        }
        break;
    case FEM_DISPL_ROT_NODE:
        for (i = 0; i < 6; i++)
        {
            DofPtr dof = new Dof(static_cast<DofKind>(i));
            m_dofs.push_back(dof);
        }
        break;
    default:
        for (i = 0; i < 3; i++)
        {
            DofPtr dof = new Dof(static_cast<DofKind>(i));
            m_dofs.push_back(dof);
        }
        break;
    }
}

// ------------------------------------------------------------
int Node::getKind()
{
    return m_kind;
}

// ------------------------------------------------------------
Dof* Node::getDof(unsigned int dof)
{
    if (dof < m_dofs.size())
        return m_dofs[dof];
    else
        return NULL;
}

// ------------------------------------------------------------
long Node::enumerateDofs(long count)
{
    for (unsigned int i = 0; i < m_dofs.size(); i++)
        m_dofs[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
void Node::saveToStream(std::ostream& out)
{
    using namespace std;
    Base::saveToStream(out);
    out << m_kind << " ";
    out << m_number << " ";
    for (int i = 0; i < 3; i++)
        out << m_coord[i] << " ";
    /*
    switch (m_kind)
    {
    case FEM_DISPL_ROT_NODE:
        for (i=0; i<6; i++)
            out << m_dofs[i]->getNumber() << " ";
        break;
    case FEM_DISPL_NODE:
        for (i=0; i<3; i++)
            out << m_dofs[i]->getNumber() << " ";
        break;
    default:
        for (i=0; i<6; i++)
            out << m_dofs[i]->getNumber() << " ";
        break;
    }
    */
    out << endl;
}

// ------------------------------------------------------------
void Node::readFromStream(std::istream& in)
{
    Base::readFromStream(in);
    in >> m_kind;
    in >> m_number;
    for (int i = 0; i < 3; i++)
        in >> m_coord[i];
    this->setKind(m_kind);
}

// ------------------------------------------------------------
void Node::setValueSize(int size)
{
    int i;
    m_values.resize(size);
    for (i = 0; i < size; i++)
        m_values[i] = 0.0;
}

// ------------------------------------------------------------
double Node::getValue(unsigned int idx)
{
    if (idx < m_values.size())
        return m_values[idx];
    else
        return 0.0;
}

// ------------------------------------------------------------
void Node::setValue(unsigned int idx, double value)
{
    if (idx < m_values.size())
        m_values[idx] = value;
}

// ------------------------------------------------------------
size_t Node::getValueSize()
{
    return m_values.size();
}

// ------------------------------------------------------------
void Node::clearValues()
{
    m_values.clear();
}
