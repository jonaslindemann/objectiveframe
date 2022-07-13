#ifndef _CFemElementLoad_H_
#define _CFemElementLoad_H_

#include "FemLoad.h"
#include "FemElement.h"

#include <vector>

namespace std {};
using namespace std;

FemSmartPointer(FemElementLoad);

class FemElementLoad : public FemLoad {
private:
    std::vector<FemElement*> m_elements;
    std::vector<long> m_elementIndex;
    double m_localDirection[3];
public:
    FemElementLoad();
    virtual ~FemElementLoad ();

    // Class info

    FemClassInfo("FemElementLoad",FemLoad);

    bool removeElement(FemElement* element);
    void clearElements();
    void addElement(FemElement* element);

    // Get/set methods

    void getLocalDirection(double *v);
    void setLocalDirection(double *v);
    void getLocalDirection(double &ex, double &ey, double &ez);
    void setLocalDirection(double ex, double ey, double ez);
    unsigned int getElementIndex(unsigned int idx);
    size_t getElementIndexSize();
    FemElement* getElement(unsigned int idx);
    size_t getElementsSize();

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
