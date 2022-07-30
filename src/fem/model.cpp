#include <ofem/model.h>

using namespace ofem;
using namespace std;

// ------------------------------------------------------------
Model::Model()
    : Base()
{
}

// ------------------------------------------------------------
Model::~Model()
{
}

// ------------------------------------------------------------
NodeSet* Model::getNodeSet()
{
    return m_nodeSet;
}

NodeSet* Model::nodeSet()
{
    return m_nodeSet;
}

// ------------------------------------------------------------
ElementSet* Model::getElementSet()
{
    return m_elementSet;
}

ElementSet* Model::elementSet()
{
    return m_elementSet;
}

// ------------------------------------------------------------
MaterialSet* Model::getMaterialSet()
{
    return m_materialSet;
}

MaterialSet* Model::materialSet()
{
    return m_materialSet;
}

// ------------------------------------------------------------
NodeLoadSet* Model::getNodeLoadSet()
{
    return m_nodeLoadSet;
}

NodeLoadSet* Model::nodeLoadSet()
{
    return m_nodeLoadSet;
}

// ------------------------------------------------------------
ElementLoadSet* Model::getElementLoadSet()
{
    return m_elementLoadSet;
}

ElementLoadSet* Model::elementLoadSet()
{
    return m_elementLoadSet;
}

// ------------------------------------------------------------
NodeBCSet* Model::getBCSet()
{
    return m_bcSet;
}

NodeBCSet* Model::BCSet()
{
    return m_bcSet;
}

// ------------------------------------------------------------
NodeSet* Model::createNodeSet()
{
    return new NodeSet();
}

// ------------------------------------------------------------
ElementSet* Model::createElementSet()
{
    return new ElementSet();
}

// ------------------------------------------------------------
MaterialSet* Model::createMaterialSet()
{
    return new MaterialSet();
}

// ------------------------------------------------------------
NodeLoadSet* Model::createNodeLoadSet()
{
    return new NodeLoadSet();
}

// ------------------------------------------------------------
ElementLoadSet* Model::createElementLoadSet()
{
    return new ElementLoadSet();
}

// ------------------------------------------------------------
NodeBCSet* Model::createBCSet()
{
    return new NodeBCSet();
}

// ------------------------------------------------------------
void Model::print(ostream& out)
{
    m_materialSet->print(out);
    m_nodeSet->print(out);
    m_elementSet->print(out);
}

// ------------------------------------------------------------
void Model::saveToStream(std::ostream& out)
{
    // Enumerate model

    m_materialSet->enumerateMaterials();
    m_nodeSet->enumerateNodes();
    m_nodeSet->enumerateDofs();
    m_elementSet->enumerateElements();
    m_nodeLoadSet->enumerateLoads();
    m_elementLoadSet->enumerateLoads();
    m_bcSet->enumerateBCs();

    // Save to stream

    m_materialSet->saveToStream(out);
    out << endl
        << endl;
    m_nodeSet->saveToStream(out);
    out << endl
        << endl;
    m_elementSet->saveToStream(out);
    out << endl
        << endl;
    m_nodeLoadSet->saveToStream(out);
    out << endl
        << endl;
    m_elementLoadSet->saveToStream(out);
    out << endl
        << endl;
    m_bcSet->saveToStream(out);
}

// ------------------------------------------------------------
void Model::readFromStream(std::istream& in)
{
    initialize();
    deleteAll();
    m_materialSet->readFromStream(in);
    m_nodeSet->readFromStream(in);
    m_elementSet->readFromStream(in);
    m_nodeLoadSet->readFromStream(in);
    m_elementLoadSet->readFromStream(in);
    m_bcSet->readFromStream(in);
    this->connectElements();
    this->connectMaterials();
    this->connectNodeLoads();
    this->connectElementLoads();
    this->connectNodeBCs();
}

// ------------------------------------------------------------
void Model::connectElements()
{
    m_elementSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void Model::connectMaterials()
{
}

// ------------------------------------------------------------
void Model::connectNodeLoads()
{
    m_nodeLoadSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void Model::connectElementLoads()
{
    m_elementLoadSet->connectElements(m_elementSet);
}

// ------------------------------------------------------------
void Model::connectNodeBCs()
{
    m_bcSet->connectNodes(m_nodeSet);
}

// ------------------------------------------------------------
void Model::deleteAll()
{
    m_materialSet->deleteAll();
    m_elementSet->deleteAll();
    m_nodeSet->deleteAll();
    m_nodeLoadSet->deleteAll();
    m_elementLoadSet->deleteAll();
    m_bcSet->deleteAll();
}

// ------------------------------------------------------------
void Model::setFileName(const std::string& fname)
{
    m_fileName = fname;
}

// ------------------------------------------------------------
const std::string Model::getFileName()
{
    return m_fileName;
}

// ------------------------------------------------------------
bool Model::open()
{
    if (m_fileName != "")
    {
        fstream inputFile;
        inputFile.open(m_fileName.c_str(), ios::in);
        if (inputFile.is_open())
        {
            this->readFromStream(inputFile);
        }
        else
            return false;

        inputFile.close();
        return true;
    }
    else
        return false;
}

// ------------------------------------------------------------
void Model::save()
{
    if (m_fileName != "")
    {
        fstream outputFile;
        outputFile.open(m_fileName.c_str(), ios::out);
        this->saveToStream(outputFile);
        outputFile.close();
    }
}

// ------------------------------------------------------------
void Model::initialize()
{
    // Create new sets

    m_elementLoadSet = this->createElementLoadSet();
    m_nodeSet = this->createNodeSet();
    m_elementSet = this->createElementSet();
    m_materialSet = this->createMaterialSet();
    m_nodeLoadSet = this->createNodeLoadSet();
    m_bcSet = this->createBCSet();

    this->onInitialised();
}

// ------------------------------------------------------------
void Model::onInitialised()
{
}

// ------------------------------------------------------------
void Model::clearNodeValues()
{
    m_nodeSet->clearNodeValues();
}

// ------------------------------------------------------------
NodeBCSet* Model::getNodeBCSet()
{
    return m_bcSet;
}

NodeBCSet* Model::nodeBCSet()
{
    return m_bcSet;
}
