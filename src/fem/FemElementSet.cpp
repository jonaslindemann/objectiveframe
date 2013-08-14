// Implementation of: public class CFemElementSet

#include "FemElementSet.h"

// ------------------------------------------------------------
CFemElementSet::CFemElementSet ()
		:CFemObject()
{
}

// ------------------------------------------------------------
CFemElementSet::~CFemElementSet ()
{
	deleteAll();
}

// ------------------------------------------------------------
void CFemElementSet::print(std::ostream &out)
{
	using namespace std;
	for (unsigned int i=0; i<m_elements.size(); i++)
		out << m_elements[i];
	out << endl;
}

// ------------------------------------------------------------
void CFemElementSet::addElement(CFemElement *element)
{
	element->addReference();
	m_elements.push_back(element);
}

// ------------------------------------------------------------
CFemElement* CFemElementSet::getElement(long i)
{
	if ( (i>=0)&&(i<(long)m_elements.size()) )
		return m_elements[i];
	else
		return NULL;
}

// ------------------------------------------------------------
bool CFemElementSet::deleteElement(long i)
{
	std::vector<CFemElement*>::iterator p = m_elements.begin();

	if ( (i>=0)&&(i<(long)m_elements.size()) )
	{
		CFemElement* element = m_elements[i];

		element->deleteReference();
		if (!element->isReferenced())
		{		
			p += i;
			m_elements.erase(p);
			delete element;
			return true;
		}
		element->addReference();
	}
	return false;
}

// ------------------------------------------------------------
CFemElement* CFemElementSet::removeElement(long i)
{
	std::vector<CFemElement*>::iterator p = m_elements.begin();

	if ( (i>=0)&&(i<(long)m_elements.size()) )
	{
		CFemElement* element = m_elements[i];
		element->deleteReference();
		p += i;
		m_elements.erase(p);
		return element;
	}
	else
		return NULL;
}

// ------------------------------------------------------------
void CFemElementSet::deleteAll()
{
	for (unsigned int i=0; i<m_elements.size(); i++)
	{
		CFemElement* element = m_elements[i];
		element->deleteReference();
		if (!element->isReferenced())
			delete element;
	}
	m_elements.clear();
}

// ------------------------------------------------------------
void CFemElementSet::clear()
{
	for (unsigned int i=0; i<m_elements.size(); i++)
	{
		CFemElement* element = m_elements[i];
		element->deleteReference();
	}
	m_elements.clear();
}

// ------------------------------------------------------------
long CFemElementSet::enumerateElements(long count)
{
	for (unsigned int i=0; i<m_elements.size(); i++)
		m_elements[i]->setNumber(count++);
	return count;
}

// ------------------------------------------------------------
long CFemElementSet::getSize()
{
	return m_elements.size();
}

// ------------------------------------------------------------
void CFemElementSet::saveToStream(std::ostream &out)
{
	using namespace std;
	CFemObject::saveToStream(out);
	out << m_elements.size() << endl;
	for (unsigned int i=0; i<m_elements.size(); i++)
		m_elements[i]->saveToStream(out);
}

// ------------------------------------------------------------
void CFemElementSet::readFromStream(std::istream &in)
{
	long nElements;
	CFemObject::readFromStream(in);
	in >> nElements;
	deleteAll();
	for (int i=0; i<nElements; i++)
	{
		CFemElement* element = createElement();
		element->addReference();
		element->readFromStream(in);
		m_elements.push_back(element);
	}
}

// ------------------------------------------------------------
void CFemElementSet::connectNodes(CFemNodeSet *nodes)
{
	for (unsigned int i=0; i<m_elements.size(); i++)
	{
		CFemElement* element = m_elements[i];
		for (unsigned int j=0; j<element->getIndexSize(); j++)
			element->addNode(nodes->getNode(element->getElementIndex(j)-1));
	}
}

// ------------------------------------------------------------
CFemElement* CFemElementSet::createElement()
{
	return new CFemElement();
}

// ------------------------------------------------------------
bool CFemElementSet::removeElement(CFemElement *element)
{
	std::vector<CFemElement*>::iterator p = m_elements.begin();

	for (unsigned int i=0; i<m_elements.size(); i++)
	{
		if (element==m_elements[i])
		{	
			element->deleteReference();
			if (!element->isReferenced())
			{
				p = m_elements.begin();
				p += i;
				m_elements.erase(p);
				delete element;
				return true;
			}
			else
			{
				element->addReference();
				return false;
			}
		}
	}
	return false;
}


// ------------------------------------------------------------
long CFemElementSet::enumerateDofs(long count)
{
	for (unsigned int i=0; i<m_elements.size(); i++)
		count = m_elements[i]->enumerateDofs(count);
	return count;	
}
