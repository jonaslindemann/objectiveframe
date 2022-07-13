// Implementation of: public class CFemMaterialSet

#include "FemMaterialSet.h"

// ------------------------------------------------------------
FemMaterialSet::FemMaterialSet ()
    :FemObject()
{
    m_currentMaterialIdx = -1;
}

// ------------------------------------------------------------
FemMaterialSet::~FemMaterialSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void FemMaterialSet::print(std::ostream &out)
{
    using namespace std;
    for (unsigned int i=0; i<m_materials.size(); i++)
        out << m_materials[i];
    out << endl;
}

// ------------------------------------------------------------
void FemMaterialSet::addMaterial(FemMaterial *material)
{
    m_materials.push_back(FemMaterialPtr(material));
}

// ------------------------------------------------------------
FemMaterial* FemMaterialSet::getMaterial(long i)
{
    if ( (i>=0)&&(i<(long)m_materials.size()) )
        return m_materials[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool FemMaterialSet::deleteMaterial(long i)
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
FemMaterial* FemMaterialSet::removeMaterial(long i)
{
    if ( (i>=0)&&(i<(long)m_materials.size()) )
    {
        FemMaterial* material = m_materials[i];
        
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
void FemMaterialSet::deleteAll()
{
    m_materials.clear();
}

// ------------------------------------------------------------
void FemMaterialSet::clear()
{
    m_materials.clear();
}

// ------------------------------------------------------------
long FemMaterialSet::enumerateMaterials(long count)
{
    for (unsigned int i=0; i<m_materials.size(); i++)
        m_materials[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
size_t FemMaterialSet::getSize()
{
    return m_materials.size();
}

// ------------------------------------------------------------
void FemMaterialSet::saveToStream(std::ostream &out)
{
    using namespace std;
    FemObject::saveToStream(out);
    out << m_materials.size() << endl;
    for (unsigned int i=0; i<m_materials.size(); i++)
        m_materials[i]->saveToStream(out);
}

// ------------------------------------------------------------
void FemMaterialSet::readFromStream(std::istream &in)
{
    long nMaterials;
    FemObject::readFromStream(in);
    in >> nMaterials;
    deleteAll();
    for (int i=0; i<nMaterials; i++)
    {
        FemMaterialPtr material = createMaterial();
        material->readFromStream(in);
        m_materials.push_back(material);
        std::cout << "material ref count = " << material->getRefCount() << std::endl;        
    }
}

// ------------------------------------------------------------
FemMaterial* FemMaterialSet::createMaterial()
{
    return new FemMaterial();
}

// ------------------------------------------------------------
bool FemMaterialSet::removeMaterial(FemMaterial *material)
{
    std::vector<FemMaterialPtr>::iterator p = m_materials.begin();

    while ( (p!=m_materials.end())&&(*p!=material) )
        p++;

    if (p!=m_materials.end())
    {
        FemMaterial* material = *p;
        
        if (material->getRefCount()==2)
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
void FemMaterialSet::setCurrentMaterial(long i)
{
    if ( (i>=0)&&(i<(long)m_materials.size()) )
        m_currentMaterialIdx = i;
    else
        m_currentMaterialIdx = -1;
}

// ------------------------------------------------------------
FemMaterial* FemMaterialSet::currentMaterial()
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

