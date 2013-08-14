#ifndef _CFemElementLoad_H_
#define _CFemElementLoad_H_

#include "FemLoad.h"
#include "FemElement.h"

#include <vector>

namespace std {};
using namespace std;

FemSmartPointer(CFemElementLoad);

class CFemElementLoad : public CFemLoad {
private:
	std::vector<CFemElement*> m_elements;
	std::vector<long> m_elementIndex;
	double m_localDirection[3];
public:
	CFemElementLoad();
	virtual ~CFemElementLoad ();

	// Class info

	FemClassInfo("CFemElementLoad",CFemLoad);

	bool removeElement(CFemElement* element);
	void clearElements();
	void addElement(CFemElement* element);

	// Get/set methods

	void getLocalDirection(double *v);
	void setLocalDirection(double *v);
	void getLocalDirection(double &ex, double &ey, double &ez);
	void setLocalDirection(double ex, double ey, double ez);
	unsigned int getElementIndex(unsigned int idx);
	unsigned int getElementIndexSize();
	CFemElement* getElement(unsigned int idx);
	int getElementsSize();
	
	// IO Methods

	virtual void readFromStream(std::istream &in);
	virtual void saveToStream(std::ostream &out);
	virtual void print(std::ostream &out);
};
#endif
