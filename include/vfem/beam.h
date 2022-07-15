#pragma once

#include <ofem/beam.h>
#include <ofem/node.h>

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

#include <vfem/node.h>
#include <vfem/color_table.h>
#include <vfem/beam_model.h>

namespace vfem {

IvfSmartPointer(VisBeam);

class Beam : public ivf::Composite {
private:
	ofem::Beam* m_femBeam;
	ivf::SimpleLineSetPtr	m_lineSet;
	ivf::SolidLinePtr		m_solidLine;
	vfem::Node* m_nodes[2];
	ivf::MaterialPtr		m_beamMaterial;
	ivf::ExtrusionPtr		m_extrusion;

	ivf::TexturePtr		m_beamTexture;
	ivf::ImagePtr			m_beamImage;

	vfem::BeamModel* m_beamModel;

	void initResults();
	double calcNavier(double N, double My, double Mz);
public:
	Beam();
	virtual ~Beam();

	IvfClassInfo("vfem::Beam", ivf::Composite);

	// Methods

	virtual void refresh();
	void initExtrusion();

	// Get/set methods

	void setBeam(ofem::Beam* beam);
	ofem::Beam* getBeam();
	void setNodes(vfem::Node* node1, vfem::Node* node2);

	void setBeamModel(vfem::BeamModel* beamModel);

protected:
	virtual void doCreateSelect();
	virtual void doCreateGeometry();
};
}