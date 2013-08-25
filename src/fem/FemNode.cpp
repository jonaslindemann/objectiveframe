// Implementation of: public class CFemNode

#include "FemNode.h"

// ------------------------------------------------------------
CFemNode::CFemNode ()
    :CFemObject()
{
    m_number = -1;
    m_coord[0] = 0.0;
    m_coord[1] = 0.0;
    m_coord[2] = 0.0;
    this->setKind(FEM_DISPL_ROT_NODE);
}

// ------------------------------------------------------------
CFemNode::CFemNode (double x, double y, double z)
    :CFemObject()
{
    m_number = -1;
    this->setCoord(x, y, z);
    this->setKind(FEM_DISPL_ROT_NODE);
}

// ------------------------------------------------------------
CFemNode::~CFemNode ()
{

}

// ------------------------------------------------------------
void CFemNode::setCoord(double x, double y, double z)
{
    m_coord[0] = x;
    m_coord[1] = y;
    m_coord[2] = z;
}

// ------------------------------------------------------------
void CFemNode::getCoord(double &x, double &y, double &z)
{
    x = m_coord[0];
    y = m_coord[1];
    z = m_coord[2];
}

// ------------------------------------------------------------
void CFemNode::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long CFemNode::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void CFemNode::print(std::ostream &out)
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

// ------------------------------------------------------------
void CFemNode::setKind(int kind)
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
            CFemDofPtr dof = new CFemDof(i);
            m_dofs.push_back(dof);
        }
        break;
    case FEM_DISPL_ROT_NODE:
        for (i=0; i<6; i++)
        {
            CFemDofPtr dof = new CFemDof(i);
            m_dofs.push_back(dof);
        }
        break;
    default:
        for (i=0; i<3; i++)
        {
            CFemDofPtr dof = new CFemDof(i);
            m_dofs.push_back(dof);
        }
        break;
    }
}

// ------------------------------------------------------------
int CFemNode::getKind()
{
    return m_kind;
}

// ------------------------------------------------------------
CFemDof* CFemNode::getDof(unsigned int dof)
{
    if (dof<m_dofs.size())
        return m_dofs[dof];
    else
        return NULL;
}

// ------------------------------------------------------------
long CFemNode::enumerateDofs(long count)
{
    for (unsigned int i=0; i<m_dofs.size(); i++)
        m_dofs[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
void CFemNode::saveToStream(std::ostream &out)
{
    using namespace std;
    CFemObject::saveToStream(out);
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
void CFemNode::readFromStream(std::istream &in)
{
    CFemObject::readFromStream(in);
    in >> m_kind;
    in >> m_number;
    for (int i=0; i<3; i++)
        in >> m_coord[i];
    this->setKind(m_kind);
}

// ------------------------------------------------------------
void CFemNode::setValueSize(int size)
{
    int i;
    m_values.resize(size);
    for (i=0; i<size; i++)
        m_values[i] = 0.0;
}

// ------------------------------------------------------------
double CFemNode::getValue(unsigned int idx)
{
    if (idx<m_values.size())
        return m_values[idx];
    else
        return 0.0;
}

// ------------------------------------------------------------
void CFemNode::setValue(unsigned int idx, double value)
{
    if (idx<m_values.size())
        m_values[idx] = value;
}

// ------------------------------------------------------------
int CFemNode::getValueSize()
{
    return m_values.size();
}

// ------------------------------------------------------------
void CFemNode::clearValues()
{
    m_values.clear();
}

