#ifndef _CFemLoadSet_H_
#define _CFemLoadSet_H_

#include "FemObject.h"
#include "FemLoad.h"
#include "FemNodeSet.h"
#include "FemElementSet.h"

FemSmartPointer(FemLoadSet);

class FemLoadSet : public FemObject {
private:
    std::vector<FemLoad*> m_loads;
public:
    bool removeLoad(FemLoad* load);
    void connectElements(FemElementSet *elements);
    void connectNodes(FemNodeSet* nodes);
    FemLoadSet ();
    virtual ~FemLoadSet ();

    // Class info

    FemClassInfo("FemLoadSet",FemObject);

    // Methods

    void deleteAll();
    FemLoad* removeLoad(long i);
    bool deleteLoad(long i);
    FemLoad* getLoad(long i);
    void addLoad(FemLoad* element);
    size_t getSize();
    void clear();
    long enumerateLoads(long count=1);

    // IO methods

    void readFromStream(std::istream &in);
    void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
protected:
    virtual FemLoad* createLoad();
};
#endif
