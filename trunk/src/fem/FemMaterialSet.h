#ifndef _CFemMaterialSet_H_
#define _CFemMaterialSet_H_

#include "FemObject.h"
#include "FemMaterial.h"

#include <vector>

FemSmartPointer(CFemMaterialSet);

class CFemMaterialSet : public CFemObject {
private:
	std::vector<CFemMaterial*> m_materials;
protected:

	// Allocators

	virtual CFemMaterial* createMaterial();
public:
	CFemMaterialSet ();
	virtual ~CFemMaterialSet ();

	// Class info

	FemClassInfo("CFemMaterialSet",CFemObject);

	// Methods

	void deleteAll();
	CFemMaterial* removeMaterial(long i);
	bool deleteMaterial(long i);
	CFemMaterial* getMaterial(long i);
	void addMaterial(CFemMaterial* material);
	long getSize();
	void clear();
	long enumerateMaterials(long count=1);
	bool removeMaterial(CFemMaterial* material);

	// IO methods

	void readFromStream(std::istream &in);
	void saveToStream(std::ostream &out);
	virtual void print(std::ostream &out);
};
#endif
