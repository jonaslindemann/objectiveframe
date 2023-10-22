#include <ofem/model.h>

#include <ofem/model_state_info.h>

#include <sstream>

using namespace ofem;
using namespace std;

Model::Model() : Base(), m_version{"2"}, m_readVersion{""}, m_writeVersion{""}
{
    ModelStateInfo::getInstance().setVersion(m_version);
}

Model::~Model()
{
}

NodeSet *Model::getNodeSet()
{
    return m_nodeSet.get();
}

NodeSet *Model::nodeSet()
{
    return m_nodeSet.get();
}

ElementSet *Model::getElementSet()
{
    return m_elementSet.get();
}

ElementSet *Model::elementSet()
{
    return m_elementSet.get();
}

MaterialSet *Model::getMaterialSet()
{
    return m_materialSet.get();
}

MaterialSet *Model::materialSet()
{
    return m_materialSet.get();
}

NodeLoadSet *Model::getNodeLoadSet()
{
    return m_nodeLoadSet.get();
}

NodeLoadSet *Model::nodeLoadSet()
{
    return m_nodeLoadSet.get();
}

ElementLoadSet *Model::getElementLoadSet()
{
    return m_elementLoadSet.get();
}

ElementLoadSet *Model::elementLoadSet()
{
    return m_elementLoadSet.get();
}

NodeBCSet *Model::getBCSet()
{
    return m_bcSet.get();
}

NodeBCSet *Model::BCSet()
{
    return m_bcSet.get();
}

NodeSet *Model::createNodeSet()
{
    return new NodeSet();
}

ElementSet *Model::createElementSet()
{
    return new ElementSet();
}

MaterialSet *Model::createMaterialSet()
{
    return new MaterialSet();
}

NodeLoadSet *Model::createNodeLoadSet()
{
    return new NodeLoadSet();
}

ElementLoadSet *Model::createElementLoadSet()
{
    return new ElementLoadSet();
}

NodeBCSet *Model::createBCSet()
{
    return new NodeBCSet();
}

void Model::print(ostream &out)
{
    m_materialSet->print(out);
    m_nodeSet->print(out);
    m_elementSet->print(out);
}

void Model::saveToStream(std::ostream &out)
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
    out << endl << endl;
    m_nodeSet->saveToStream(out);
    out << endl << endl;
    m_elementSet->saveToStream(out);
    out << endl << endl;
    m_nodeLoadSet->saveToStream(out);
    out << endl << endl;
    m_elementLoadSet->saveToStream(out);
    out << endl << endl;
    m_bcSet->saveToStream(out);
    this->onSaveComplete();
}

void Model::readFromStream(std::istream &in)
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
    this->onReadComplete();
}

void Model::connectElements()
{
    m_elementSet->connectNodes(m_nodeSet.get());
}

void Model::connectMaterials()
{
}

void Model::connectNodeLoads()
{
    m_nodeLoadSet->connectNodes(m_nodeSet.get());
}

void Model::connectElementLoads()
{
    m_elementLoadSet->connectElements(m_elementSet.get());
}

void Model::connectNodeBCs()
{
    m_bcSet->connectNodes(m_nodeSet.get());
}

void Model::deleteAll()
{
    m_materialSet->deleteAll();
    m_elementSet->deleteAll();
    m_nodeSet->deleteAll();
    m_nodeLoadSet->deleteAll();
    m_elementLoadSet->deleteAll();
    m_bcSet->deleteAll();
}

void Model::setFileName(const std::string &fname)
{
    m_fileName = fname;
}

const std::string Model::getFileName()
{
    return m_fileName;
}

std::string ofem::Model::queryFileVersion(std::string filename)
{
    std::fstream f;
    f.open(filename.c_str(), ios::in);

    std::string line;
    if (f.is_open()) {
        std::getline(f, line);

        f.close();

        if (line.find("#OF_VERSION") != -1) {
            auto equalPos = line.find("=");
            auto versionStr = line.substr(equalPos + 1, line.length() - equalPos);
            return versionStr;
        }
        else
            return "0";
    }
    else
        return "";
}

bool Model::open()
{
    if (m_fileName != "") {
        std::string readVersion = queryFileVersion(m_fileName);
        ModelStateInfo::getInstance().setReadVersion(readVersion);

        fstream inputFile;

        inputFile.open(m_fileName.c_str(), ios::in);
        if (inputFile.is_open()) {
            // Skip version line
            if (readVersion != "0") {
                std::string line;
                std::getline(inputFile, line);
            }
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

void Model::save()
{
    if (m_fileName != "") {
        ofem::ModelStateInfo::getInstance().setWriteVersion(m_version);

        fstream outputFile;
        outputFile.open(m_fileName.c_str(), ios::out);
        outputFile << "#OF_VERSION=" << m_version << "\n";
        this->saveToStream(outputFile);
        outputFile.close();
    }
}

void Model::snapShot()
{
    std::stringstream ss;
    this->saveToStream(ss);
    m_snapShots.push_back(ss.str());
    m_restoredSnapShots.clear();
}

void Model::restoreLastSnapShot()
{
    if (m_snapShots.size() > 0) {
        // Snapshot current model

        std::stringstream cs;
        this->saveToStream(cs);
        m_restoredSnapShots.push_back(cs.str());

        // Revert to last snapshot

        auto snapShot = m_snapShots.back();
        m_snapShots.pop_back();

        std::stringstream ss(snapShot);
        this->readFromStream(ss);
    }
}

void Model::revertLastSnapShot()
{
    if (m_restoredSnapShots.size() > 0) {
        // Current model to snapshot

        std::stringstream ss;
        this->saveToStream(ss);
        m_snapShots.push_back(ss.str());

        // Go forward

        auto snapShot = m_restoredSnapShots.back();
        m_restoredSnapShots.pop_back();
        std::stringstream cs(snapShot);
        this->readFromStream(cs);
    }
}

size_t Model::snapShotCount()
{
    return m_snapShots.size();
}

const std::string ofem::Model::toString()
{
    std::stringstream ss;
    this->saveToStream(ss);

    return ss.str();
}

bool ofem::Model::fromString(const std::string &value)
{
    std::stringstream ss(value);
    this->readFromStream(ss);
    return true;
}

void Model::initialize()
{
    // Create new sets

    m_elementLoadSet = ElementLoadSetPtr(this->createElementLoadSet());
    m_nodeSet = NodeSetPtr(this->createNodeSet());
    m_elementSet = ElementSetPtr(this->createElementSet());
    m_materialSet = MaterialSetPtr(this->createMaterialSet());
    m_nodeLoadSet = NodeLoadSetPtr(this->createNodeLoadSet());
    m_bcSet = NodeBCSetPtr(this->createBCSet());

    this->onInitialised();
}

void Model::onInitialised()
{
}

void ofem::Model::onReadComplete()
{
}

void ofem::Model::onSaveComplete()
{
}

void Model::clearNodeValues()
{
    m_nodeSet->clearNodeValues();
}

std::string ofem::Model::version()
{
    return m_version;
}

NodeBCSet *Model::getNodeBCSet()
{
    return m_bcSet.get();
}

NodeBCSet *Model::nodeBCSet()
{
    return m_bcSet.get();
}
