#ifndef _CFemLoadSet_H_
#define _CFemLoadSet_H_

#include "FemObject.h"
#include "FemLoad.h"
#include "FemNodeSet.h"
#include "FemElementSet.h"

FemSmartPointer(CFemLoadSet);

class CFemLoadSet : public CFemObject {
private:
	std::vector<CFemLoad*> m_loads;
public:
	bool removeLoad(CFemLoad* load);
	void connectElements(CFemElementSet *elements);
	void connectNodes(CFemNodeSet* nodes);
	CFemLoadSet ();
	virtual ~CFemLoadSet ();

	// Class info

	FemClassInfo("CFemLoadSet",CFemObject);

	// Methods

	void deleteAll();
	CFemLoad* removeLoad(long i);
	bool deleteLoad(long i);
	CFemLoad* getLoad(long i);
	void addLoad(CFemLoad* element);
	long getSize();
	void clear();
	long enumerateLoads(long count=1);

	// IO methods

	void readFromStream(std::istream &in);
	void saveToStream(std::ostream &out);
	virtual void print(std::ostream &out);
protected:
	virtual CFemLoad* createLoad();
};
#endif
