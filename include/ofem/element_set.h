#pragma once

#include <ofem/base.h>
#include <ofem/element.h>
#include <ofem/node_set.h>

namespace ofem {

SmartPointer(ElementSet);

class ElementSet : public Base {
private:
    std::vector<ElementPtr> m_elements;

public:
    ElementSet();
    virtual ~ElementSet();

    // Class info

    ClassInfo("ElementSet", Base);
    StdFactory(ElementSet);

    // Methods

    void deleteAll();
    bool removeElement(long i);
    bool deleteElement(long i);
    Element *getElement(long i);
    void addElement(Element *element);
    size_t getSize();
    void clear();
    long enumerateElements(long count = 1);
    void connectNodes(NodeSet *nodes);
    long enumerateDofs(long count = 1);
    bool removeElement(Element *element);

    // IO methods

    void readFromStream(std::istream &in) override;
    void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;

protected:
    virtual Element *createElement();
};
} // namespace ofem
