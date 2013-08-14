#ifndef _CFemBC_H_
#define _CFemBC_H_

#include "FemObject.h"
#include "FemDof.h"

#include <vector>

FemSmartPointer(CFemBC);

class CFemBC : public CFemObject {
private:
	long m_number;
	std::vector<CFemDofPtr> m_prescribedDofs;
	std::vector<double>   m_prescribedValues;
public:
	CFemBC ();
	virtual ~CFemBC ();

	FemClassInfo("CFemBC",CFemObject);

	// Methods

	bool isPrescribed(CFemDof* dof);
	void prescribeDof(CFemDof* dof, double value);
	void unprescribeDof(CFemDof* dof);
	void clearDofs();

	// Get/set methods

	void setNumber(long number);
	long getNumber();

	int getSize();

	double getValue(unsigned int idx);

	CFemDof* getDof(unsigned int idx);

	// IO Methods

	virtual void readFromStream(std::istream &in);
	virtual void saveToStream(std::ostream &out);
	virtual void print(std::ostream &out);
};
#endif
