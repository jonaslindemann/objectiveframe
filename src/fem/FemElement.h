#ifndef _CFemElement_H_
#define _CFemElement_H_

#include "FemObject.h"
#include "FemNode.h"
#include "FemInternalDofs.h"

#include <vector>

FemSmartPointer(FemElement);

class FemElement : public FemObject {
private:
    std::vector<FemNodePtr> m_nodes;
    std::vector<FemInternalDofsPtr> m_internalDofs;
    std::vector<long> m_nodeIndex;
    std::vector<double> m_properties;
    std::vector<double> m_values;
    double m_elementLoad[3];
    long m_number;
public:
    FemElement ();
    virtual ~FemElement ();

    // Class info

    FemClassInfo("FemElement",FemObject);

    // Methods

    void addNode(FemNode* node);
    void addProperty(double value);
    void clearProperties();
    void deleteAll();
    void clear();
    long getInternalDof(unsigned int localIdx, int localDof);
    long enumerateDofs(long count=1);
    bool hasInternalDof(unsigned int localIdx, int localDof);
    void clearInternalDof(unsigned int localIdx, int localDof);
    void addInternalDof(unsigned int localIdx, int localDof);
    void setProperty(unsigned int idx, double value);

    void setValueSize(int size);
    size_t getValueSize();

    double getValue(unsigned int idx);
    void setValue(unsigned int idx, double value);

    // Get/set methods

    FemNode* getNode(unsigned int index);
    size_t getIndexSize();
    size_t getSize();
    long getElementIndex(unsigned int node);
    long getNumber();
    void setNumber(long number);
    void getElementLoad(double &fx, double &fy, double &fz);
    void setElementLoad(double fx, double fy, double fz);
    double getProperty(unsigned int idx);
    void setPropertySize(unsigned int size);
    size_t getPropertySize();

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
