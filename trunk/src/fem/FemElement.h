#ifndef _CFemElement_H_
#define _CFemElement_H_

#include "FemObject.h"
#include "FemNode.h"
#include "FemInternalDofs.h"

#include <vector>

FemSmartPointer(CFemElement);

class CFemElement : public CFemObject {
private:
	std::vector<CFemNode*> m_nodes;
	std::vector<CFemInternalDofs*> m_internalDofs;
	std::vector<long> m_nodeIndex;
	std::vector<double> m_properties;
	std::vector<double> m_values;
	double m_elementLoad[3];
	long m_number;
public:
	CFemElement ();
	virtual ~CFemElement ();

	// Class info

	FemClassInfo("CFemElement",CFemObject);

	// Methods

	void addNode(CFemNode* node);
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
	int getValueSize();

	double getValue(unsigned int idx);
	void setValue(unsigned int idx, double value);

	// Get/set methods

	CFemNode* getNode(unsigned int index);
	unsigned int getIndexSize();
	unsigned int getSize();
	long getElementIndex(unsigned int node);
	long getNumber();
	void setNumber(long number);
	void getElementLoad(double &fx, double &fy, double &fz);
	void setElementLoad(double fx, double fy, double fz);
	double getProperty(unsigned int idx);
	void setPropertySize(unsigned int size);
	unsigned int getPropertySize();

	// IO Methods

	virtual void readFromStream(std::istream &in);
	virtual void saveToStream(std::ostream &out);
	virtual void print(std::ostream &out);
};
#endif
