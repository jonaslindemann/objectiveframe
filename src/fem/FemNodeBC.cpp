// Implementation of: public class CFemNodeBC

#include "FemNodeBC.h"

// ------------------------------------------------------------
CFemNodeBC::CFemNodeBC ()
{
	int i;
	for (i=0; i<6; i++)
	{
		m_prescribedDof[i] = true;
		m_prescribedValues[i] = 0.0;
	}
}	

// ------------------------------------------------------------
CFemNodeBC::~CFemNodeBC ()
{
	this->clearNodes();
}

// ------------------------------------------------------------
void CFemNodeBC::print(std::ostream &out)
{
	
}

// ------------------------------------------------------------
void CFemNodeBC::saveToStream(std::ostream &out)
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
void CFemNodeBC::readFromStream(std::istream &in)
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
void CFemNodeBC::addNode(CFemNode *node)
{
	node->addReference();
	m_nodes.push_back(node);
}

// ------------------------------------------------------------
void CFemNodeBC::clearNodes()
{
	unsigned int i;

	for (i=0; i<m_nodes.size(); i++)
		m_nodes[i]->deleteReference();

	m_nodes.clear();
}

// ------------------------------------------------------------
bool CFemNodeBC::removeNode(CFemNode *node)
{
	std::vector<CFemNode*>::iterator p = m_nodes.begin();

	while ( (p!=m_nodes.end())&&(*p!=node) )
		p++;

	if (*p==node)
	{
		node->deleteReference();
		m_nodes.erase(p);
		return true;
	}
	return false;
}

// ------------------------------------------------------------
CFemNode* CFemNodeBC::getNode(unsigned int idx)
{
	if ( (idx>=0)&&(idx<m_nodes.size()) )
		return m_nodes[idx];
	else
		return NULL;
}

// ------------------------------------------------------------
long CFemNodeBC::getNodeIndex(unsigned int idx)
{
	if ( (idx>=0)&&(idx<m_nodeIndex.size()) )
		return m_nodeIndex[idx];
	else
		return -1;
}

// ------------------------------------------------------------
int CFemNodeBC::getNodeIndexSize()
{
	return m_nodeIndex.size();
}

// ------------------------------------------------------------
int CFemNodeBC::getNodeSize()
{
	return m_nodes.size();
}

// ------------------------------------------------------------
void CFemNodeBC::prescribe(int dof, double value)
{
	if ((dof>=1)&&(dof<=6))
	{
		m_prescribedDof[dof-1] = true;
		m_prescribedValues[dof-1] = value;
	}
}

// ------------------------------------------------------------
void CFemNodeBC::unprescribe(int dof)
{
	if ((dof>=1)&&(dof<=6))
	{
		m_prescribedDof[dof-1] = false;
		m_prescribedValues[dof-1] = 0.0;
	}
}

// ------------------------------------------------------------
bool CFemNodeBC::isPrescribed(int dof)
{
	if ((dof>=1)&&(dof<=6))
		return m_prescribedDof[dof-1];
	else
		return false;
}

// ------------------------------------------------------------
double CFemNodeBC::getPrescribedValue(int dof)
{
	if ((dof>=1)&&(dof<=6))
		return m_prescribedValues[dof-1];
	else
		return 0.0;
}
