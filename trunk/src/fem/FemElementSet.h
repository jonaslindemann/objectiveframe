#ifndef _CFemElementSet_H_
#define _CFemElementSet_H_

#include "FemObject.h"
#include "FemElement.h"
#include "FemNodeSet.h"

FemSmartPointer(CFemElementSet);

class CFemElementSet : public CFemObject {
private:
	std::vector<CFemElement*> m_elements;
public:
	CFemElementSet ();
	virtual ~CFemElementSet ();

	// Class info

	FemClassInfo("CFemElementSet",CFemObject);

	// Methods

	void deleteAll();
	CFemElement* removeElement(long i);
	bool deleteElement(long i);
	CFemElement* getElement(long i);
	void addElement(CFemElement* element);
	long getSize();
	void clear();
	long enumerateElements(long count=1);
	void connectNodes(CFemNodeSet* nodes);
	long enumerateDofs(long count=1);
	bool removeElement(CFemElement* element);

	// IO methods

	void readFromStream(std::istream &in);
	void saveToStream(std::ostream &out);
	virtual void print(std::ostream &out);
protected:
	virtual CFemElement* createElement();
};
#endif
