#pragma once

#include <ofem/base.h>
#include <ofem/material.h>

#include <vector>

namespace ofem
{

SmartPointer(MaterialSet);

class MaterialSet : public Base
{
private:
    std::vector<MaterialPtr> m_materials;
    long m_currentMaterialIdx;

protected:
    // Allocators

    virtual Material* createMaterial();

public:
    MaterialSet();
    virtual ~MaterialSet();

    // Class info

    ClassInfo("MaterialSet", Base);

    // Methods

    void deleteAll();
    Material* removeMaterial(long i);
    bool deleteMaterial(long i);
    Material* getMaterial(long i);
    void addMaterial(Material* material);
    size_t getSize();
    void clear();
    long enumerateMaterials(long count = 1);
    bool removeMaterial(Material* material);

    void setCurrentMaterial(long i);
    Material* currentMaterial();

    // IO methods

    void readFromStream(std::istream& in) override;
    void saveToStream(std::ostream& out) override;
    virtual void print(std::ostream& out) override;
};
}
