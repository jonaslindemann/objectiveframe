#ifndef _CFemMaterial_H_
#define _CFemMaterial_H_

#include "FemObject.h"

FemSmartPointer(CFemMaterial);

class CFemMaterial : public CFemObject {
private:
	long m_number;
public:
	CFemMaterial ();
	virtual ~CFemMaterial ();

	FemClassInfo("CFemMaterial",CFemObject);

	// Get/set methods
	
	long getNumber();
	void setNumber(long number);

	// IO Methods

	virtual void readFromStream(std::istream &in);
	virtual void saveToStream(std::ostream &out);
	virtual void print(std::ostream &out);
};
#endif
