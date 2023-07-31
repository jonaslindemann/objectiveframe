#pragma once

#include <ofem/base.h>
#include <ofem/element_set.h>
#include <ofem/load.h>
#include <ofem/node_set.h>

namespace ofem {

SmartPointer(LoadSet);

class LoadSet : public Base {
private:
    std::vector<Load *> m_loads;

public:
    bool removeLoad(Load *load);
    void connectElements(ElementSet *elements);
    void connectNodes(NodeSet *nodes);
    LoadSet();
    virtual ~LoadSet();

    // Class info

    ClassInfo("LoadSet", Base);
    StdFactory(LoadSet);

    // Methods

    void deleteAll();
    Load *removeLoad(long i);
    bool deleteLoad(long i);
    Load *getLoad(long i);
    void addLoad(Load *element);
    size_t getSize();
    void clear();
    long enumerateLoads(long count = 1);

    // IO methods

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;

protected:
    virtual Load *createLoad();
};
} // namespace ofem
