// Implementation of: public class CFemMaterialSet

#include "FemMaterialSet.h"

// ------------------------------------------------------------
CFemMaterialSet::CFemMaterialSet ()
		:CFemObject()
{
}

// ------------------------------------------------------------
CFemMaterialSet::~CFemMaterialSet ()
{
	deleteAll();
}

// ------------------------------------------------------------
void CFemMaterialSet::print(std::ostream &out)
{
	using namespace std;
	for (unsigned int i=0; i<m_materials.size(); i++)
		out << m_materials[i];
	out << endl;
}

// ------------------------------------------------------------
void CFemMaterialSet::addMaterial(CFemMaterial *material)
{
	material->addReference();
	m_materials.push_back(material);
}

// ------------------------------------------------------------
CFemMaterial* CFemMaterialSet::getMaterial(long i)
{
	if ( (i>=0)&&(i<(long)m_materials.size()) )
		return m_materials[i];
	else
		return NULL;
}

// ------------------------------------------------------------
bool CFemMaterialSet::deleteMaterial(long i)
{
	std::vector<CFemMaterial*>::iterator p = m_materials.begin();

	if ( (i>=0)&&(i<(long)m_materials.size()) )
	{
		CFemMaterial* material = m_materials[i];

		material->deleteReference();
		if (!material->isReferenced())
		{		
			p += i;
			m_materials.erase(p);
			delete material;
			return true;
		}
		material->addReference();
	}
	return false;
}

// ------------------------------------------------------------
CFemMaterial* CFemMaterialSet::removeMaterial(long i)
{
	std::vector<CFemMaterial*>::iterator p = m_materials.begin();

	if ( (i>=0)&&(i<(long)m_materials.size()) )
	{
		CFemMaterial* material = m_materials[i];
		material->deleteReference();
		p += i;
		m_materials.erase(p);
		return material;
	}
	else
		return NULL;
}

// ------------------------------------------------------------
void CFemMaterialSet::deleteAll()
{
	for (unsigned int i=0; i<m_materials.size(); i++)
	{
		CFemMaterial* material = m_materials[i];
		material->deleteReference();
		if (!material->isReferenced())
			delete material;
	}
	m_materials.clear();
}

// ------------------------------------------------------------
void CFemMaterialSet::clear()
{
	for (unsigned int i=0; i<m_materials.size(); i++)
	{
		CFemMaterial* material = m_materials[i];
		material->deleteReference();
	}
	m_materials.clear();
}

// ------------------------------------------------------------
long CFemMaterialSet::enumerateMaterials(long count)
{
	for (unsigned int i=0; i<m_materials.size(); i++)
		m_materials[i]->setNumber(count++);
	return count;
}

// ------------------------------------------------------------
long CFemMaterialSet::getSize()
{
	return m_materials.size();
}

// ------------------------------------------------------------
void CFemMaterialSet::saveToStream(std::ostream &out)
{
	using namespace std;
	CFemObject::saveToStream(out);
	out << m_materials.size() << endl;
	for (unsigned int i=0; i<m_materials.size(); i++)
		m_materials[i]->saveToStream(out);
}

// ------------------------------------------------------------
void CFemMaterialSet::readFromStream(std::istream &in)
{
	long nMaterials;
	CFemObject::readFromStream(in);
	in >> nMaterials;
	deleteAll();
	for (int i=0; i<nMaterials; i++)
	{
		CFemMaterial* material = createMaterial();
		material->addReference();
		material->readFromStream(in);
		m_materials.push_back(material);
	}
}

// ------------------------------------------------------------
CFemMaterial* CFemMaterialSet::createMaterial()
{
	return new CFemMaterial();
}

// ------------------------------------------------------------
bool CFemMaterialSet::removeMaterial(CFemMaterial *material)
{
	std::vector<CFemMaterial*>::iterator p = m_materials.begin();

	while ( (p!=m_materials.end())&&(*p!=material) )
		p++;

	if (p!=m_materials.end())
	{
		CFemMaterial* material = *p;
		material->deleteReference();
		if (!material->isReferenced())
		{
			m_materials.erase(p);
			delete material;
			return true;
		}
		else
			material->addReference();
	}
	return false;
}
