#ifndef _CFemMaterialSet_H_
#define _CFemMaterialSet_H_

#include "FemObject.h"
#include "FemMaterial.h"

#include <vector>

FemSmartPointer(FemMaterialSet);

class FemMaterialSet : public FemObject {
private:
    std::vector<FemMaterialPtr> m_materials;
    long m_currentMaterialIdx;
protected:

    // Allocators

    virtual FemMaterial* createMaterial();
public:
    FemMaterialSet ();
    virtual ~FemMaterialSet ();

    // Class info

    FemClassInfo("FemMaterialSet",FemObject);

    // Methods

    void deleteAll();
    FemMaterial* removeMaterial(long i);
    bool deleteMaterial(long i);
    FemMaterial* getMaterial(long i);
    void addMaterial(FemMaterial* material);
    size_t getSize();
    void clear();
    long enumerateMaterials(long count=1);
    bool removeMaterial(FemMaterial* material);
    
    void setCurrentMaterial(long i);
    FemMaterial* currentMaterial();

    // IO methods

    void readFromStream(std::istream &in);
    void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
