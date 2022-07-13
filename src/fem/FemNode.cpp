// Implementation of: public class CFemNode

#include "FemNode.h"

// ------------------------------------------------------------
FemNode::FemNode ()
    :FemObject()
{
    m_number = -1;
    m_coord[0] = 0.0;
    m_coord[1] = 0.0;
    m_coord[2] = 0.0;
    this->setKind(FEM_DISPL_ROT_NODE);
}

// ------------------------------------------------------------
FemNode::FemNode (double x, double y, double z)
    :FemObject()
{
    m_number = -1;
    this->setCoord(x, y, z);
    this->setKind(FEM_DISPL_ROT_NODE);
}

// ------------------------------------------------------------
FemNode::~FemNode ()
{

}

// ------------------------------------------------------------
void FemNode::setCoord(double x, double y, double z)
{
    m_coord[0] = x;
    m_coord[1] = y;
    m_coord[2] = z;
}

// ------------------------------------------------------------
void FemNode::getCoord(double &x, double &y, double &z)
{
    x = m_coord[0];
    y = m_coord[1];
    z = m_coord[2];
}

// ------------------------------------------------------------
void FemNode::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long FemNode::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void FemNode::print(std::ostream &out)
{
    using namespace std;
    out << "Node " << m_number << endl;;
    out << "   Location (" << m_coord[0] << ", ";
    out << m_coord[1] << ", ";
    out << m_coord[2] << ")" << endl;
    out << "   Local dofs:  ";
    unsigned int i;
    for (i=0; i<m_dofs.size(); i++)
        out << m_dofs[i];
    out << endl;
    out << "   Global dofs: ";
    for (i=0; i<m_dofs.size(); i++)
        out << m_dofs[i]->getNumber() << " ";
    out << endl;
}

json FemNode::toJSON()
{
    /*
    long m_number;
    int m_kind;
    double m_coord[3];
    std::vector<double> m_values;
    std::vector<FemDofPtr> m_dofs;
    */
    json j;
    j["number"] = m_number;
    j["kind"] = m_kind;
    j["x"] = m_coord[0];
    j["y"] = m_coord[1];
    j["z"] = m_coord[2];
    j["values"] = m_values;

    json jdofs;

    for (auto& dof : m_dofs)
        jdofs.push_back(dof->toJSON());

    j["dofs"] = jdofs;

    return j;
}

void FemNode::fromJSON(nlohmann::json& j)
{
}

// ------------------------------------------------------------
void FemNode::setKind(int kind)
{
    m_kind = kind;

    // Delete previous dofs

    unsigned int i;

    m_dofs.clear();

    // Create new dofs

    switch (m_kind)
    {
    case FEM_DISPL_NODE:
        for (i=0; i<3; i++)
        {
            FemDofPtr dof = new FemDof(static_cast<FemDofKind>(i));
            m_dofs.push_back(dof);
        }
        break;
    case FEM_DISPL_ROT_NODE:
        for (i=0; i<6; i++)
        {
            FemDofPtr dof = new FemDof(static_cast<FemDofKind>(i));
            m_dofs.push_back(dof);
        }
        break;
    default:
        for (i=0; i<3; i++)
        {
            FemDofPtr dof = new FemDof(static_cast<FemDofKind>(i));
            m_dofs.push_back(dof);
        }
        break;
    }
}

// ------------------------------------------------------------
int FemNode::getKind()
{
    return m_kind;
}

// ------------------------------------------------------------
FemDof* FemNode::getDof(unsigned int dof)
{
    if (dof<m_dofs.size())
        return m_dofs[dof];
    else
        return NULL;
}

// ------------------------------------------------------------
long FemNode::enumerateDofs(long count)
{
    for (unsigned int i=0; i<m_dofs.size(); i++)
        m_dofs[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
void FemNode::saveToStream(std::ostream &out)
{
    using namespace std;
    FemObject::saveToStream(out);
    out << m_kind << " " ;
    out << m_number << " ";
    for (int i=0; i<3; i++)
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
void FemNode::readFromStream(std::istream &in)
{
    FemObject::readFromStream(in);
    in >> m_kind;
    in >> m_number;
    for (int i=0; i<3; i++)
        in >> m_coord[i];
    this->setKind(m_kind);
}

// ------------------------------------------------------------
void FemNode::setValueSize(int size)
{
    int i;
    m_values.resize(size);
    for (i=0; i<size; i++)
        m_values[i] = 0.0;
}

// ------------------------------------------------------------
double FemNode::getValue(unsigned int idx)
{
    if (idx<m_values.size())
        return m_values[idx];
    else
        return 0.0;
}

// ------------------------------------------------------------
void FemNode::setValue(unsigned int idx, double value)
{
    if (idx<m_values.size())
        m_values[idx] = value;
}

// ------------------------------------------------------------
size_t FemNode::getValueSize()
{
    return m_values.size();
}

// ------------------------------------------------------------
void FemNode::clearValues()
{
    m_values.clear();
}

