#ifndef _CFemBCSet_H_
#define _CFemBCSet_H_

#include "FemObject.h"
#include "FemBC.h"
#include "FemNodeSet.h"

#include <vector>

FemSmartPointer(CFemBCSet);

class CFemBCSet : public CFemObject {
private:
    std::vector<CFemBCPtr> m_bcs;
public:
    CFemBCSet ();
    virtual ~CFemBCSet ();

    FemClassInfo("CFemBCSet",CFemObject);

    // Methods

    void deleteAll();
    CFemBCPtr removeBC(long i);
    bool deleteBC(long i);
    void addBC(CFemBC* element);
    void clear();
    long enumerateBCs(long count=1);
    bool removeBC(CFemBC* bc);
    void connectNodes(CFemNodeSet* nodes);

    // Get/set methods

    CFemBC* getBC(long i);
    size_t getSize();

    // IO methods

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;
    virtual json toJSON() override;
protected:
    virtual CFemBC* createBC();
};
#endif
