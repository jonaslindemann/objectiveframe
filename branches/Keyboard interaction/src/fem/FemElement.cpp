// Implementation of: public class CFemElement

#include "FemElement.h"

// ------------------------------------------------------------
CFemElement::CFemElement ()
    :CFemObject()
{
    m_number = -1;
    m_elementLoad[0] = 0.0;
    m_elementLoad[1] = 0.0;
    m_elementLoad[2] = 0.0;
}

// ------------------------------------------------------------
CFemElement::~CFemElement ()
{
    deleteAll();
}

// ------------------------------------------------------------
void CFemElement::print(std::ostream &out)
{
    using namespace std;
    out << "Element " << this->getNumber() << endl;
    out << "   Nodes: ";
    for (unsigned int i=0; i<m_nodes.size(); i++)
        cout << m_nodes[i]->getNumber() << " ";
    out << endl;
}

// ------------------------------------------------------------
void CFemElement::addNode(CFemNode *node)
{
    m_nodes.push_back(CFemNodePtr(node));
    CFemInternalDofsPtr iDof = new CFemInternalDofs();
    m_internalDofs.push_back(iDof);
}

// ------------------------------------------------------------
void CFemElement::clear()
{
    m_nodes.clear();
    m_internalDofs.clear();
}

// ------------------------------------------------------------
void CFemElement::deleteAll()
{
    m_nodes.clear();
    m_internalDofs.clear();
}

// ------------------------------------------------------------
void CFemElement::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long CFemElement::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void CFemElement::saveToStream(std::ostream &out)
{
    using namespace std;
    CFemObject::saveToStream(out);
    unsigned int i;
    out << m_number << "  ";
    out << m_nodes.size() << " ";
    for (i=0; i<m_nodes.size(); i++)
        out << m_nodes[i]->getNumber() << " ";
    out << endl;
    out << m_elementLoad[0] << " "
        << m_elementLoad[1] << " "
        << m_elementLoad[2] << endl;
    out << getPropertySize() << endl;
    for (i=0; i<getPropertySize(); i++)
        out << getProperty(i) << " ";
    out << endl;
    for (i=0; i<m_nodes.size(); i++)
        m_internalDofs[i]->saveToStream(out);
}

// ------------------------------------------------------------
void CFemElement::readFromStream(std::istream &in)
{
    long nNodes, nProperties;
    double prop;
    long i;
    CFemObject::readFromStream(in);
    in >> m_number;
    in >> nNodes;
    m_nodeIndex.clear();
    for (i=0; i<nNodes; i++)
    {
        long nodeIdx;
        in >> nodeIdx;
        m_nodeIndex.push_back(nodeIdx);
    }
    in >> m_elementLoad[0];
    in >> m_elementLoad[1];
    in >> m_elementLoad[2];
    in >> nProperties;
    clearProperties();
    for (i=0; i<nProperties; i++)
    {
        in >> prop;
        m_properties.push_back(prop);
    }
    m_internalDofs.clear();

    for (i=0; i<nNodes; i++)
    {
        CFemInternalDofsPtr intDof = new CFemInternalDofs();
        m_internalDofs.push_back(intDof);
        intDof->readFromStream(in);
    }
}

// ------------------------------------------------------------
long CFemElement::getElementIndex(unsigned int node)
{
    if (node<m_nodeIndex.size())
        return m_nodeIndex[node];
    else
        return -1;
}

// ------------------------------------------------------------
unsigned int CFemElement::getSize()
{
    return m_nodes.size();
}

// ------------------------------------------------------------
unsigned int CFemElement::getIndexSize()
{
    return m_nodeIndex.size();
}

// ------------------------------------------------------------
CFemNode* CFemElement::getNode(unsigned int index)
{
    if (index<m_nodes.size())
        return m_nodes[index];
    else
        return NULL;
}

// ------------------------------------------------------------
void CFemElement::addProperty(double value)
{
    m_properties.push_back(value);
}

// ------------------------------------------------------------
double CFemElement::getProperty(unsigned int idx)
{
    return m_properties[idx];
}

// ------------------------------------------------------------
void CFemElement::clearProperties()
{
    m_properties.clear();
}

// ------------------------------------------------------------
void CFemElement::setElementLoad(double fx, double fy, double fz)
{
    m_elementLoad[0] = fx;
    m_elementLoad[1] = fy;
    m_elementLoad[2] = fz;
}

// ------------------------------------------------------------
void CFemElement::getElementLoad(double &fx, double &fy, double &fz)
{
    fx = m_elementLoad[0];
    fy = m_elementLoad[1];
    fz = m_elementLoad[2];
}

// ------------------------------------------------------------
unsigned int CFemElement::getPropertySize()
{
    return m_properties.size();
}

// ------------------------------------------------------------
void CFemElement::setPropertySize(unsigned int size)
{
    m_properties.clear();
    for (unsigned int i=0; i<size; i++)
        m_properties.push_back(0.0);
}

// ------------------------------------------------------------
void CFemElement::setProperty(unsigned int idx, double value)
{
    if (idx<m_properties.size())
        m_properties[idx] = value;
}

// ------------------------------------------------------------
void CFemElement::addInternalDof(unsigned int localIdx, int localDof)
{
    if (localIdx<m_internalDofs.size())
    {
        m_internalDofs[localIdx]->add(localDof);
    }
}

// ------------------------------------------------------------
void CFemElement::clearInternalDof(unsigned int localIdx, int localDof)
{
    if (localIdx<m_internalDofs.size())
    {
        m_internalDofs[localIdx]->clear(localDof);
    }
}

// ------------------------------------------------------------
bool CFemElement::hasInternalDof(unsigned int localIdx, int localDof)
{
    if (localIdx<m_internalDofs.size())
    {
        return m_internalDofs[localIdx]->isAssigned(localDof);
    }
    else
        return false;
}

// ------------------------------------------------------------
long CFemElement::enumerateDofs(long count)
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
    {
        count = m_internalDofs[i]->enumerateDofs(count);
    }
    return count;
}

// ------------------------------------------------------------
long CFemElement::getInternalDof(unsigned int localIdx, int localDof)
{
    if (localIdx<m_internalDofs.size())
    {
        if (m_internalDofs[localIdx]->isAssigned(localDof))
            return m_internalDofs[localIdx]->getDof(localDof)->getNumber();
    }
    return -1;
}

// ------------------------------------------------------------
void CFemElement::setValueSize(int size)
{
    int i;
    m_values.resize(size);
    for (i=0; i<size; i++)
        m_values[i] = 0.0;
}

// ------------------------------------------------------------
double CFemElement::getValue(unsigned int idx)
{
    if (idx<m_values.size())
        return m_values[idx];
    else
        return 0.0;
}

// ------------------------------------------------------------
void CFemElement::setValue(unsigned int idx, double value)
{
    if (idx<m_values.size())
        m_values[idx] = value;
}

// ------------------------------------------------------------
int CFemElement::getValueSize()
{
    return m_values.size();
}
