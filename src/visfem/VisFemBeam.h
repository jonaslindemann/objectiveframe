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
    FemBeam*			m_femBeam;
    ivf::SimpleLineSetPtr	m_lineSet;
    ivf::SolidLinePtr		m_solidLine;
    VisFemNode*		    m_nodes[2];
    ivf::MaterialPtr		m_beamMaterial;
    ivf::ExtrusionPtr		m_extrusion;

    ivf::TexturePtr		m_beamTexture;
    ivf::ImagePtr			m_beamImage;

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

    void setBeam(FemBeam* beam);
    FemBeam* getBeam();
    void setNodes(VisFemNode* node1, VisFemNode* node2);

    void setBeamModel(VisBeamModel* beamModel);

protected:
    virtual void doCreateSelect();
    virtual void doCreateGeometry();
};
