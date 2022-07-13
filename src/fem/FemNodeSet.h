#ifndef _CFemNodeSet_H_
#define _CFemNodeSet_H_

#include "FemObject.h"
#include "FemNode.h"

#include <vector>

FemSmartPointer(FemNodeSet);

class FemNodeSet : public FemObject {
private:
    std::vector<FemNodePtr> m_nodes;
public:
    FemNodeSet ();
    virtual ~FemNodeSet ();

    // Class info

    FemClassInfo("FemNodeSet",FemObject);

    // Methods

    void deleteAll();
    FemNode* removeNode(long i);
    bool removeNode(FemNode* node);
    bool deleteNode(long i);
    FemNode* getNode(long i);
    void addNode(FemNode* node);
    void clear();

    void clearNodeValues();

    long enumerateNodes(long count=1);
    long enumerateDofs(long count=1);

    // Get/set methods

    size_t getSize();

    virtual void print(std::ostream &out);
    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);

    virtual json toJSON() override;
};
#endif
