#ifndef _CStructureFactory_h_
#define _CStructureFactory_h_

#include <ivf/Scene.h>

#include <VisBeamModel.h>
#include <VisFemNode.h>
#include <VisFemBeam.h>

class CStructureFactory {
private:
    int m_size[3];
    double m_spacing[3];
    VisBeamModel* m_beamModel;
    CFemBeamMaterial* m_currentMaterial;
public:
    void setCurrentMaterial(CFemBeamMaterial* material);
    CStructureFactory();
    virtual ~CStructureFactory();

    // Methods

    void create();

    // Get/set methods

    void setSize(int rows, int cols, int stacks);
    void getSize(int &rows, int &cols, int &stacks);
    void setSpacing(double xSpacing, double ySpacing, double zSpacing);
    void getSpacing(double &xSpacing, double &ySpacing, double &zSpacing);
    void setBeamModel(VisBeamModel* model);
    VisBeamModel* getBeamModel();
};

#endif
