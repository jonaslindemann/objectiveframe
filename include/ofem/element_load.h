#pragma once

#include <ofem/element.h>
#include <ofem/load.h>

#include <vector>

namespace ofem {

SmartPointer(ElementLoad);

class ElementLoad : public Load {
private:
    std::vector<Element *> m_elements;
    std::vector<long> m_elementIndex;
    double m_localDirection[3];

public:
    ElementLoad();
    virtual ~ElementLoad();

    // Class info

    ClassInfo("ElementLoad", Load);
    StdFactory(ElementLoad);

    bool removeElement(Element *element);
    void clearElements();
    void addElement(Element *element);

    // Get/set methods

    void getLocalDirection(double *v);
    void setLocalDirection(double *v);
    void getLocalDirection(double &ex, double &ey, double &ez);
    void setLocalDirection(double ex, double ey, double ez);
    unsigned int getElementIndex(unsigned int idx);
    size_t getElementIndexSize();
    Element *getElement(unsigned int idx);
    size_t getElementsSize();

    // IO Methods

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;
};
} // namespace ofem
