#ifndef _CFemNodeSet_H_
#define _CFemNodeSet_H_

#include "FemObject.h"
#include "FemNode.h"

#include <vector>

FemSmartPointer(CFemNodeSet);

class CFemNodeSet : public CFemObject {
private:
    std::vector<CFemNodePtr> m_nodes;
public:
    CFemNodeSet ();
    virtual ~CFemNodeSet ();

    // Class info

    FemClassInfo("CFemNodeSet",CFemObject);

    // Methods

    void deleteAll();
    CFemNode* removeNode(long i);
    bool removeNode(CFemNode* node);
    bool deleteNode(long i);
    CFemNode* getNode(long i);
    void addNode(CFemNode* node);
    void clear();

    void clearNodeValues();

    long enumerateNodes(long count=1);
    long enumerateDofs(long count=1);

    // Get/set methods

    long getSize();

    virtual void print(std::ostream &out);
    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);

    virtual json toJSON() override;
};
#endif
