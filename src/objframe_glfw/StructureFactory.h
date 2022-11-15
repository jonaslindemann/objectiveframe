#ifndef _CStructureFactory_h_
#define _CStructureFactory_h_

#include <ivf/Scene.h>

#include <vfem/beam.h>
#include <vfem/beam_model.h>
#include <vfem/node.h>

class StructureFactory
{
private:
    int m_size[3];
    double m_spacing[3];
    vfem::BeamModel* m_beamModel;
    ofem::BeamMaterial* m_currentMaterial;

public:
    void setCurrentMaterial(ofem::BeamMaterial* material);
    StructureFactory();
    virtual ~StructureFactory();

    // Methods

    void create();

    // Get/set methods

    void setSize(int rows, int cols, int stacks);
    void getSize(int& rows, int& cols, int& stacks);
    void setSpacing(double xSpacing, double ySpacing, double zSpacing);
    void getSpacing(double& xSpacing, double& ySpacing, double& zSpacing);
    void setBeamModel(vfem::BeamModel* model);
    vfem::BeamModel* getBeamModel();
};

#endif
