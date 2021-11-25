#pragma once

#include <FemBeam.h>
#include <FemNode.h>

#include <ivf/Composite.h>
#include <ivf/SimpleLineSet.h>
#include <ivf/SolidLine.h>
#include <ivf/Material.h>
#include <ivf/Extrusion.h>
#include <ivf/LineSet.h>
#include <ivf/Image.h>
#include <ivf/Texture.h>

#include <ColorMap.h>
#include <ResultInfo.h>

#include "VisFemNode.h"
#include "ColorTable.h"

#include "VisBeamModel.h"

IvfSmartPointer(VisFemBeam);

class VisFemBeam : public ivf::Composite {
private:
    CFemBeam*			m_femBeam;
    ivf::SimpleLineSet*	m_lineSet;
    ivf::SolidLine*		m_solidLine;
    VisFemNode*		    m_nodes[2];
    ivf::Material*		m_beamMaterial;
    ivf::Extrusion*		m_extrusion;

    ivf::Texture*		m_beamTexture;
    ivf::Image*			m_beamImage;

    VisBeamModel*		m_beamModel;

    void initResults();
    double calcNavier(double N, double My, double Mz);
public:
    VisFemBeam ();
    virtual ~VisFemBeam ();

    IvfClassInfo("VisFemBeam",ivf::Composite);

    // Methods

    virtual void refresh();
    void initExtrusion();

    // Get/set methods

    void setBeam(CFemBeam* beam);
    CFemBeam* getBeam();
    void setNodes(VisFemNode* node1, VisFemNode* node2);

    void setBeamModel(VisBeamModel* beamModel);

protected:
    virtual void doCreateSelect();
    virtual void doCreateGeometry();
};
