#ifndef _CIvfFemNodeBC_H_
#define _CIvfFemNodeBC_H_

#include <ivf/ivfconfig.h>

namespace std {};
using namespace std;

#include "FemBeamNodeBC.h"
#include <ivf/Shape.h>
#include <ivf/Cone.h>
#include <ivf/Material.h>
#include <ivf/Transform.h>

#include "ColorTable.h"
#include "VisBC.h"
#include "VisBeamModel.h"

IvfSmartPointer(CIVfFemNodeBC);

// public class: VisFemNodeBC
class VisFemNodeBC : public ivf::Shape {
private:
    CFemBeamNodeBC* m_nodeBC;
    vector<VisBC*> m_bc;
    ivf::Material* m_bcMaterial;

    VisBeamModel* m_beamModel;
public:
    VisFemNodeBC();
    virtual ~VisFemNodeBC();

    IvfClassInfo("VisFemNodeBC",ivf::Shape);

    void refresh();

    void setNodeBC(CFemBeamNodeBC* bc);
    void setBeamModel(VisBeamModel* model);
protected:
    virtual void doCreateGeometry();
};
#endif
