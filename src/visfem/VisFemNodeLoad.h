#ifndef _CIvfFemNodeLoad_H_
#define _CIvfFemNodeLoad_H_

#include <FemBeamNodeLoad.h>
#include <ivf/Shape.h>
#include <ivf/ExtrArrow.h>

#include "ColorTable.h"
#include "VisBeamModel.h"

IvfSmartPointer(VisFemNodeLoad);

// public class: VisFemNodeLoad
class VisFemNodeLoad : public ivf::Shape {
private:
    CFemBeamNodeLoad* m_nodeLoad;
    vector<ivf::ExtrArrowPtr> m_arrow;
    ivf::MaterialPtr m_arrowMaterial;
    VisBeamModel* m_beamModel;

    void initArrow();
public:
    VisFemNodeLoad ();
    virtual ~VisFemNodeLoad ();

    // Class info

    IvfClassInfo("VisFemNodeLoad",ivf::Shape);

    // Get/set methods

    void setBeamModel(VisBeamModel* model);
    void setNodeLoad(CFemBeamNodeLoad* nodeLoad);

    // Methods

    void refresh();

protected:
    virtual void doCreateSelect();
    virtual void doCreateGeometry();
};
#endif
