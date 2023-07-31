#pragma once

#include <ofem/base.h>
#include <ofem/bc.h>
#include <ofem/node_set.h>

#include <vector>

namespace ofem {

SmartPointer(BCSet);

class BCSet : public Base {
private:
    std::vector<BCPtr> m_bcs;

public:
    BCSet();
    virtual ~BCSet();

    ClassInfo("BCSet", Base);
    StdFactory(BCSet);

    // Methods

    void deleteAll();
    BCPtr removeBC(long i);
    bool deleteBC(long i);
    void addBC(BC *element);
    void clear();
    long enumerateBCs(long count = 1);
    bool removeBC(BC *bc);
    void connectNodes(NodeSet *nodes);

    // Get/set methods

    BC *getBC(long i);
    size_t getSize();

    // IO methods

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;
    virtual json_nl toJson() override;

protected:
    virtual BC *createBC();
};
} // namespace ofem
