#pragma once

#include <ofem/base.h>
#include <ofem/bc.h>
#include <ofem/element_load_set.h>
#include <ofem/element_set.h>
#include <ofem/load_set.h>
#include <ofem/material_set.h>
#include <ofem/node_bc_set.h>
#include <ofem/node_load_set.h>
#include <ofem/node_set.h>

#include <fstream>
#include <string>
#include <vector>
#include <string>

namespace ofem
{

SmartPointer(Model);

class Model : public Base
{
private:
    NodeSetPtr m_nodeSet;
    ElementSetPtr m_elementSet;
    MaterialSetPtr m_materialSet;
    NodeLoadSetPtr m_nodeLoadSet;
    ElementLoadSetPtr m_elementLoadSet;
    NodeBCSetPtr m_bcSet;
    std::string m_fileName;
    std::string m_version;

    std::string m_readVersion;
    std::string m_writeVersion;

    std::vector<std::string> m_snapShots;
    std::vector<std::string> m_restoredSnapShots;

protected:
    virtual NodeBCSet* createBCSet();
    virtual ElementLoadSet* createElementLoadSet();
    virtual NodeLoadSet* createNodeLoadSet();
    virtual MaterialSet* createMaterialSet();
    virtual NodeSet* createNodeSet();
    virtual ElementSet* createElementSet();
    virtual void connectElementLoads();
    virtual void connectNodeLoads();
    virtual void connectMaterials();
    virtual void connectElements();
    virtual void connectNodeBCs();
    virtual void onInitialised();
    virtual void onReadComplete();
    virtual void onSaveComplete();

public:
    Model();
    virtual ~Model();

    // Class info

    ClassInfo("Model", Base);
    StdFactory(Model);

    // Methods

    void initialize();
    void save();
    bool open();
    void deleteAll();

    std::string queryFileVersion(std::string filename);

    void snapShot();
    void restoreLastSnapShot();
    void revertLastSnapShot();

    size_t snapShotCount();

    void clearNodeValues();

    std::string version();

    // Get/set methods

    MaterialSet* getMaterialSet();
    MaterialSet* materialSet();

    ElementSet* getElementSet();
    ElementSet* elementSet();

    NodeSet* getNodeSet();
    NodeSet* nodeSet();

    NodeLoadSet* getNodeLoadSet();
    NodeLoadSet* nodeLoadSet();

    ElementLoadSet* getElementLoadSet();
    ElementLoadSet* elementLoadSet();

    NodeBCSet* getBCSet();
    NodeBCSet* BCSet();
    NodeBCSet* getNodeBCSet();
    NodeBCSet* nodeBCSet();

    const std::string getFileName();
    void setFileName(const std::string& fname);

    // IO Methods

    virtual void readFromStream(std::istream& in) override;
    virtual void saveToStream(std::ostream& out) override;
    virtual void print(std::ostream& out) override;
};
}
