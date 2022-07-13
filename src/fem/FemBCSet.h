#ifndef _CFemBCSet_H_
#define _CFemBCSet_H_

#include "FemObject.h"
#include "FemBC.h"
#include "FemNodeSet.h"

#include <vector>

FemSmartPointer(FemBCSet);

class FemBCSet : public FemObject {
private:
    std::vector<FemBCPtr> m_bcs;
public:
    FemBCSet ();
    virtual ~FemBCSet ();

    FemClassInfo("FemBCSet",FemObject);

    // Methods

    void deleteAll();
    FemBCPtr removeBC(long i);
    bool deleteBC(long i);
    void addBC(FemBC* element);
    void clear();
    long enumerateBCs(long count=1);
    bool removeBC(FemBC* bc);
    void connectNodes(FemNodeSet* nodes);

    // Get/set methods

    FemBC* getBC(long i);
    size_t getSize();

    // IO methods

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;
    virtual json toJSON() override;
protected:
    virtual FemBC* createBC();
};
#endif
