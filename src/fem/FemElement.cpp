// Implementation of: public class CFemElement

#include "FemElement.h"

// ------------------------------------------------------------
FemElement::FemElement ()
    :FemObject()
{
    m_number = -1;
    m_elementLoad[0] = 0.0;
    m_elementLoad[1] = 0.0;
    m_elementLoad[2] = 0.0;
}

// ------------------------------------------------------------
FemElement::~FemElement ()
{
    deleteAll();
}

// ------------------------------------------------------------
void FemElement::print(std::ostream &out)
{
    using namespace std;
    out << "Element " << this->getNumber() << endl;
    out << "   Nodes: ";
    for (unsigned int i=0; i<m_nodes.size(); i++)
        cout << m_nodes[i]->getNumber() << " ";
    out << endl;
}

// ------------------------------------------------------------
void FemElement::addNode(FemNode *node)
{
    m_nodes.push_back(FemNodePtr(node));
    FemInternalDofsPtr iDof = new FemInternalDofs();
    m_internalDofs.push_back(iDof);
}

// ------------------------------------------------------------
void FemElement::clear()
{
    m_nodes.clear();
    m_internalDofs.clear();
}

// ------------------------------------------------------------
void FemElement::deleteAll()
{
    m_nodes.clear();
    m_internalDofs.clear();
}

// ------------------------------------------------------------
void FemElement::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long FemElement::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void FemElement::saveToStream(std::ostream &out)
{
    using namespace std;
    FemObject::saveToStream(out);
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
void FemElement::readFromStream(std::istream &in)
{
    long nNodes, nProperties;
    double prop;
    long i;
    FemObject::readFromStream(in);
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
        FemInternalDofsPtr intDof = new FemInternalDofs();
        m_internalDofs.push_back(intDof);
        intDof->readFromStream(in);
    }
}

// ------------------------------------------------------------
long FemElement::getElementIndex(unsigned int node)
{
    if (node<m_nodeIndex.size())
        return m_nodeIndex[node]; 
    else
        return -1;
}

// ------------------------------------------------------------
size_t FemElement::getSize()
{
    return m_nodes.size();
}

// ------------------------------------------------------------
size_t FemElement::getIndexSize()
{
    return m_nodeIndex.size();
}

// ------------------------------------------------------------
FemNode* FemElement::getNode(unsigned int index)
{
    if (index<m_nodes.size())
        return m_nodes[index];
    else
        return NULL;
}

// ------------------------------------------------------------
void FemElement::addProperty(double value)
{
    m_properties.push_back(value);
}

// ------------------------------------------------------------
double FemElement::getProperty(unsigned int idx)
{
    return m_properties[idx];
}

// ------------------------------------------------------------
void FemElement::clearProperties()
{
    m_properties.clear();
}

// ------------------------------------------------------------
void FemElement::setElementLoad(double fx, double fy, double fz)
{
    m_elementLoad[0] = fx;
    m_elementLoad[1] = fy;
    m_elementLoad[2] = fz;
}

// ------------------------------------------------------------
void FemElement::getElementLoad(double &fx, double &fy, double &fz)
{
    fx = m_elementLoad[0];
    fy = m_elementLoad[1];
    fz = m_elementLoad[2];
}

// ------------------------------------------------------------
size_t FemElement::getPropertySize()
{
    return m_properties.size();
}

// ------------------------------------------------------------
void FemElement::setPropertySize(unsigned int size)
{
    m_properties.clear();
    for (unsigned int i=0; i<size; i++)
        m_properties.push_back(0.0);
}

// ------------------------------------------------------------
void FemElement::setProperty(unsigned int idx, double value)
{
    if (idx<m_properties.size())
        m_properties[idx] = value;
}

// ------------------------------------------------------------
void FemElement::addInternalDof(unsigned int localIdx, int localDof)
{
    if (localIdx<m_internalDofs.size())
    {
        m_internalDofs[localIdx]->add(localDof);
    }
}

// ------------------------------------------------------------
void FemElement::clearInternalDof(unsigned int localIdx, int localDof)
{
    if (localIdx<m_internalDofs.size())
    {
        m_internalDofs[localIdx]->clear(localDof);
    }
}

// ------------------------------------------------------------
bool FemElement::hasInternalDof(unsigned int localIdx, int localDof)
{
    if (localIdx<m_internalDofs.size())
    {
        return m_internalDofs[localIdx]->isAssigned(localDof);
    }
    else
        return false;
}

// ------------------------------------------------------------
long FemElement::enumerateDofs(long count)
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
    {
        count = m_internalDofs[i]->enumerateDofs(count);
    }
    return count;
}

// ------------------------------------------------------------
long FemElement::getInternalDof(unsigned int localIdx, int localDof)
{
    if (localIdx<m_internalDofs.size())
    {
        if (m_internalDofs[localIdx]->isAssigned(localDof))
            return m_internalDofs[localIdx]->getDof(localDof)->getNumber();
    }
    return -1;
}

// ------------------------------------------------------------
void FemElement::setValueSize(int size)
{
    int i;
    m_values.resize(size);
    for (i=0; i<size; i++)
        m_values[i] = 0.0;
}

// ------------------------------------------------------------
double FemElement::getValue(unsigned int idx)
{
    if (idx<m_values.size())
        return m_values[idx];
    else
        return 0.0;
}

// ------------------------------------------------------------
void FemElement::setValue(unsigned int idx, double value)
{
    if (idx<m_values.size())
        m_values[idx] = value;
}

// ------------------------------------------------------------
size_t FemElement::getValueSize()
{
    return m_values.size();
}
