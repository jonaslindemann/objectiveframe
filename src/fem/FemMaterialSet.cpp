// Implementation of: public class CFemMaterialSet

#include "FemMaterialSet.h"

// ------------------------------------------------------------
CFemMaterialSet::CFemMaterialSet ()
    :CFemObject()
{
    m_currentMaterialIdx = -1;
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
    m_materials.push_back(CFemMaterialPtr(material));
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
    if ( (i>=0)&&(i<(long)m_materials.size()) )
    {
        if (m_materials[i]->getRefCount()==1)
        {
            m_materials.erase(m_materials.begin()+i);
            return true;
        }
        else
            return false;
    }
    return false;
}

// ------------------------------------------------------------
CFemMaterial* CFemMaterialSet::removeMaterial(long i)
{
    if ( (i>=0)&&(i<(long)m_materials.size()) )
    {
        CFemMaterial* material = m_materials[i];
        
        if (material->getRefCount()==1)
        {
            material->addReference();
            m_materials.erase(m_materials.begin()+i);
            material->deleteReference();
            return material;
        }
        else
            return NULL;
    }
    else
        return NULL;
}

// ------------------------------------------------------------
void CFemMaterialSet::deleteAll()
{
    m_materials.clear();
}

// ------------------------------------------------------------
void CFemMaterialSet::clear()
{
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
        CFemMaterialPtr material = createMaterial();
        material->readFromStream(in);
        m_materials.push_back(material);
        std::cout << "material ref count = " << material->getRefCount() << std::endl;        
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
    std::vector<CFemMaterialPtr>::iterator p = m_materials.begin();

    while ( (p!=m_materials.end())&&(*p!=material) )
        p++;

    if (p!=m_materials.end())
    {
        CFemMaterial* material = *p;
        
        if (material->getRefCount()==1)
        {
            m_materials.erase(p);
            return true;
        }
        else
            return false;
    }
    return false;
}

// ------------------------------------------------------------
void CFemMaterialSet::setCurrentMaterial(long i)
{
    if ( (i>=0)&&(i<(long)m_materials.size()) )
        m_currentMaterialIdx = i;
    else
        m_currentMaterialIdx = -1;
}

// ------------------------------------------------------------
CFemMaterial* CFemMaterialSet::currentMaterial()
{
    if (m_currentMaterialIdx!=-1)
    {
        if ( (m_currentMaterialIdx>=0)&&(m_currentMaterialIdx<(long)m_materials.size()) )
            return m_materials[m_currentMaterialIdx];
        else
            return 0;
    }
    else
        return 0;
}

