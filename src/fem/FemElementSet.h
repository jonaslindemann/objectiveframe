#ifndef _CFemElementSet_H_
#define _CFemElementSet_H_

#include "FemObject.h"
#include "FemElement.h"
#include "FemNodeSet.h"

FemSmartPointer(FemElementSet);

class FemElementSet : public FemObject {
private:
    std::vector<FemElementPtr> m_elements;
public:
    FemElementSet ();
    virtual ~FemElementSet ();

    // Class info

    FemClassInfo("FemElementSet",FemObject);

    // Methods

    void deleteAll();
    FemElement* removeElement(long i);
    bool deleteElement(long i);
    FemElement* getElement(long i);
    void addElement(FemElement* element);
    size_t getSize();
    void clear();
    long enumerateElements(long count=1);
    void connectNodes(FemNodeSet* nodes);
    long enumerateDofs(long count=1);
    bool removeElement(FemElement* element);

    // IO methods

    void readFromStream(std::istream &in);
    void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
protected:
    virtual FemElement* createElement();
};
#endif
