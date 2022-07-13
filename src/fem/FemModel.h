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

FemSmartPointer(FemModel);

class FemModel : public FemObject {
private:
    FemNodeSetPtr m_nodeSet;
    FemElementSetPtr m_elementSet;
    FemMaterialSetPtr m_materialSet;
    FemNodeLoadSetPtr m_nodeLoadSet;
    FemElementLoadSetPtr m_elementLoadSet;
    FemNodeBCSetPtr m_bcSet;
    std::string m_fileName;
protected:
    virtual FemNodeBCSet* createBCSet();
    virtual FemElementLoadSet* createElementLoadSet();
    virtual FemNodeLoadSet* createNodeLoadSet();
    virtual FemMaterialSet* createMaterialSet();
    virtual FemNodeSet* createNodeSet();
    virtual FemElementSet* createElementSet();
    virtual void connectElementLoads();
    virtual void connectNodeLoads();
    virtual void connectMaterials();
    virtual void connectElements();
    virtual void connectNodeBCs();
    virtual void onInitialised();
public:
    FemModel ();
    virtual ~FemModel ();

    // Class info

    FemClassInfo("FemModel",FemObject);

    // Methods

    void initialize();
    void save();
    bool open();
    void deleteAll();

    void clearNodeValues();

    // Get/set methods

    FemMaterialSet* getMaterialSet();
	FemMaterialSet* materialSet();

	FemElementSet* getElementSet();
	FemElementSet* elementSet();

	FemNodeSet* getNodeSet();
	FemNodeSet* nodeSet();

	FemNodeLoadSet* getNodeLoadSet();
	FemNodeLoadSet* nodeLoadSet();

	FemElementLoadSet* getElementLoadSet();
	FemElementLoadSet* elementLoadSet();

	FemNodeBCSet* getBCSet();
	FemNodeBCSet* BCSet();
	FemNodeBCSet* getNodeBCSet();
	FemNodeBCSet* nodeBCSet();

    const std::string getFileName();
    void setFileName(const::string& fname);

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
