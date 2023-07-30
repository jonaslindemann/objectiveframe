#include <ofem/material_set.h>

using namespace ofem;


MaterialSet::MaterialSet()
    : Base()
{
    m_currentMaterialIdx = -1;
}


MaterialSet::~MaterialSet()
{
    deleteAll();
}


void MaterialSet::print(std::ostream& out)
{
    using namespace std;
    for (unsigned int i = 0; i < m_materials.size(); i++)
        out << m_materials[i];
    out << endl;
}


void MaterialSet::addMaterial(Material* material)
{
    m_materials.push_back(MaterialPtr(material));
}


Material* MaterialSet::getMaterial(long i)
{
    if ((i >= 0) && (i < (long)m_materials.size()))
        return m_materials[i].get();
    else
        return NULL;
}


bool MaterialSet::deleteMaterial(long i)
{
    if ((i >= 0) && (i < (long)m_materials.size()))
    {
        if (m_materials[i]->getRefCount() == 1)
        {
            m_materials.erase(m_materials.begin() + i);
            return true;
        }
        else
            return false;
    }
    return false;
}

void MaterialSet::deleteAll()
{
    m_materials.clear();
}


void MaterialSet::clear()
{
    m_materials.clear();
}


long MaterialSet::enumerateMaterials(long count)
{
    for (unsigned int i = 0; i < m_materials.size(); i++)
        m_materials[i]->setNumber(count++);
    return count;
}


size_t MaterialSet::getSize()
{
    return m_materials.size();
}


void MaterialSet::saveToStream(std::ostream& out)
{
    using namespace std;
    Base::saveToStream(out);
    out << m_materials.size() << endl;
    for (unsigned int i = 0; i < m_materials.size(); i++)
        m_materials[i]->saveToStream(out);
}


void MaterialSet::readFromStream(std::istream& in)
{
    long nMaterials;
    Base::readFromStream(in);
    in >> nMaterials;
    deleteAll();
    for (int i = 0; i < nMaterials; i++)
    {
        MaterialPtr material = MaterialPtr(createMaterial());
        material->readFromStream(in);
        m_materials.push_back(material);
        std::cout << "material ref count = " << material->getRefCount() << std::endl;
    }
}


Material* MaterialSet::createMaterial()
{
    return new Material();
}


bool MaterialSet::removeMaterial(Material* material)
{
    std::vector<MaterialPtr>::iterator p = m_materials.begin();

    while ((p != m_materials.end()) && ((*p).get() != material))
        p++;

    if (p != m_materials.end())
    {
        MaterialPtr material = *p;
        m_materials.erase(p);
        return true;

        /*
        if (material->getRefCount() == 2)
        {
            m_materials.erase(p);
            return true;
        }
        else
            return false;
        */
    }
    return false;
}


void MaterialSet::setCurrentMaterial(long i)
{
    if ((i >= 0) && (i < (long)m_materials.size()))
        m_currentMaterialIdx = i;
    else
        m_currentMaterialIdx = -1;
}


Material* MaterialSet::currentMaterial()
{
    if (m_currentMaterialIdx != -1)
    {
        if ((m_currentMaterialIdx >= 0) && (m_currentMaterialIdx < (long)m_materials.size()))
            return m_materials[m_currentMaterialIdx].get();
        else
            return 0;
    }
    else
        return 0;
}
