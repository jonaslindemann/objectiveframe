#ifndef _CFemModel_H_
#define _CFemModel_H_

#include "FemObject.h"
#include "FemMaterialSet.h"
#include "FemBC.h"
#include "FemNodeBCSet.h"
#include "FemLoadSet.h"
#include "FemElementSet.h"
#include "FemNodeSet.h"
#include "FemNodeLoadSet.h"
#include "FemElementLoadSet.h"

#include <vector>
#include <fstream>

namespace std {};
using namespace std;

FemSmartPointer(CFemModel);

class CFemModel : public CFemObject {
private:
    CFemNodeSet* m_nodeSet;
    CFemElementSet* m_elementSet;
    CFemMaterialSet* m_materialSet;
    CFemNodeLoadSet* m_nodeLoadSet;
    CFemElementLoadSet* m_elementLoadSet;
    CFemNodeBCSet* m_bcSet;
    std::string m_fileName;
protected:
    virtual CFemNodeBCSet* createBCSet();
    virtual CFemElementLoadSet* createElementLoadSet();
    virtual CFemNodeLoadSet* createNodeLoadSet();
    virtual CFemMaterialSet* createMaterialSet();
    virtual CFemNodeSet* createNodeSet();
    virtual CFemElementSet* createElementSet();
    virtual void connectElementLoads();
    virtual void connectNodeLoads();
    virtual void connectMaterials();
    virtual void connectElements();
    virtual void connectNodeBCs();
    virtual void onInitialised();
public:
    CFemModel ();
    virtual ~CFemModel ();

    // Class info

    FemClassInfo("CFemModel",CFemObject);

    // Methods

    void initialize();
    void save();
    void open();
    void deleteAll();

    void clearNodeValues();

    // Get/set methods

    CFemMaterialSet* getMaterialSet();
    CFemElementSet* getElementSet();
    CFemNodeSet* getNodeSet();
    CFemNodeLoadSet* getNodeLoadSet();
    CFemElementLoadSet* getElementLoadSet();
    CFemNodeBCSet* getBCSet();
    CFemNodeBCSet* getNodeBCSet();

    const std::string getFileName();
    void setFileName(const::string& fname);

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
