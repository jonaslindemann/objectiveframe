// Implementation of: public class CFemNodeBC

#include "FemNodeBC.h"

// ------------------------------------------------------------
FemNodeBC::FemNodeBC ()
{
    int i;
    for (i=0; i<6; i++)
    {
        m_prescribedDof[i] = true;
        m_prescribedValues[i] = 0.0;
    }
}

// ------------------------------------------------------------
FemNodeBC::~FemNodeBC ()
{
    this->clearNodes();
}

// ------------------------------------------------------------
void FemNodeBC::print(std::ostream &out)
{

}

// ------------------------------------------------------------
json FemNodeBC::toJSON()
{
    json j;

    json nodeNumbers;

    for (auto& node : m_nodes)
        nodeNumbers.push_back(node->getNumber());

    j["nodes"] = nodeNumbers;
    j["prescribed_dofs"] = m_prescribedDof;
    j["prescribed_values"] = m_prescribedValues;

    return j;
}

// ------------------------------------------------------------
void FemNodeBC::saveToStream(std::ostream &out)
{
    using namespace std;
    unsigned int i;
    //CFemBC::saveToStream(out);
    out << m_nodes.size() << endl;
    for (i=0; i<m_nodes.size(); i++)
        out << m_nodes[i]->getNumber() << endl;
    out << endl;
    for (i=0; i<6; i++)
    {
        if (m_prescribedDof[i])
            out << 1 << " " << m_prescribedValues[i] << endl;
        else
            out << 0 << " " << 0.0 << endl;
    }
}

// ------------------------------------------------------------
void FemNodeBC::readFromStream(std::istream &in)
{
    int nNodes, i;
    long idx;
    int flag;
    double value;
    //CFemBC::readFromStream(in);
    in >> nNodes;
    for (i=0; i<nNodes; i++)
    {
        in >> idx;
        m_nodeIndex.push_back(idx);
    }
    for (i=0; i<6; i++)
    {
        in >> flag >> value;
        if (flag==1)
            prescribe(i+1,value);
        else
            unprescribe(i+1);
    }
}

// ------------------------------------------------------------
void FemNodeBC::addNode(FemNode *node)
{
    node->addReference();
    m_nodes.push_back(node);
}

// ------------------------------------------------------------
void FemNodeBC::clearNodes()
{
    unsigned int i;

    for (i=0; i<m_nodes.size(); i++)
        m_nodes[i]->deleteReference();

    m_nodes.clear();
}

// ------------------------------------------------------------
bool FemNodeBC::removeNode(FemNode *node)
{
    std::vector<FemNode*>::iterator p = m_nodes.begin();

    while ( (p!=m_nodes.end())&&(*p!=node) )
        p++;

	if (p != m_nodes.end())
	{
		if (*p == node)
		{
			node->deleteReference();
			m_nodes.erase(p);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

// ------------------------------------------------------------
FemNode* FemNodeBC::getNode(unsigned int idx)
{
    if (idx<m_nodes.size())
        return m_nodes[idx];
    else
        return NULL;
}

// ------------------------------------------------------------
long FemNodeBC::getNodeIndex(unsigned int idx)
{
    if (idx<m_nodeIndex.size())
        return m_nodeIndex[idx];
    else
        return -1;
}

// ------------------------------------------------------------
size_t FemNodeBC::getNodeIndexSize()
{
    return m_nodeIndex.size();
}

// ------------------------------------------------------------
size_t FemNodeBC::getNodeSize()
{
    return m_nodes.size();
}

// ------------------------------------------------------------
void FemNodeBC::prescribe(int dof, double value)
{
    if ((dof>=1)&&(dof<=6))
    {
        m_prescribedDof[dof-1] = true;
        m_prescribedValues[dof-1] = value;
    }
}

void FemNodeBC::prescribePos(double value)
{
	for (int i = 1; i <= 3; i++)
		this->prescribe(i, value);
}

void FemNodeBC::prescribeRot(double value)
{
	for (int i = 4; i <= 6; i++)
		this->prescribe(i, value);
}

void FemNodeBC::fixed()
{
	this->release();
	this->prescribePos(0.0);
	this->prescribeRot(0.0);
}

void FemNodeBC::fixedPosition()
{
	this->release();
	this->prescribePos(0.0);
}

void FemNodeBC::release()
{
	for (int i = 1; i <= 6; i++)
		this->unprescribe(i);
}

// ------------------------------------------------------------
void FemNodeBC::unprescribe(int dof)
{
    if ((dof>=1)&&(dof<=6))
    {
        m_prescribedDof[dof-1] = false;
        m_prescribedValues[dof-1] = 0.0;
    }
}

// ------------------------------------------------------------
bool FemNodeBC::isPrescribed(int dof)
{
    if ((dof>=1)&&(dof<=6))
        return m_prescribedDof[dof-1];
    else
        return false;
}

// ------------------------------------------------------------
double FemNodeBC::getPrescribedValue(int dof)
{
    if ((dof>=1)&&(dof<=6))
        return m_prescribedValues[dof-1];
    else
        return 0.0;
}

bool* FemNodeBC::getPrescribedArr()
{
    return m_prescribedDof;
}

double* FemNodeBC::getPrescribedValueArr()
{
    return m_prescribedValues;
}
