#ifndef _CIvfFemBeamLoad_H_
#define _CIvfFemBeamLoad_H_

#include <FemBeamLoad.h>
#include <ivf/Shape.h>
#include <ivf/QuadSet.h>
#include <ivf/ExtrArrow.h>
#include <ivf/Material.h>

#include "ColorTable.h"
#include "VisBeamModel.h"

namespace std {};
using namespace std;

IvfSmartPointer(VisFemBeamLoad);

// public class: VisFemBeamLoad
class VisFemBeamLoad : public ivf::Shape {
private:
    vector<ivf::QuadSet*> m_q;
    CFemBeamLoad* m_beamLoad;
    vector<ivf::ExtrArrow*> m_arrow;
    ivf::Material* m_extrMaterial;
    ivf::Material* m_arrowMaterial;
    ColorTable* m_colorTable;

    VisBeamModel* m_beamModel;

    void initExtrusion();
public:
    VisFemBeamLoad ();
    virtual ~VisFemBeamLoad ();

    IvfClassInfo("VisFemBeamLoad",ivf::Shape);

    void setColorTable(ColorTable* colorTable);
    void setBeamModel(VisBeamModel* model);


    void setBeamLoad(CFemBeamLoad* load);

    virtual void refresh();
protected:
    virtual void createSelect();
    virtual void createGeometry();
};
#endif
