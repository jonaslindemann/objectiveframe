#pragma once

#include <ofem/model.h>
#include <ofem/beam_material_set.h>
#include <ofem/beam_load_set.h>
#include <ofem/beam_set.h>
#include <ofem/beam_node_load_set.h>
#include <ofem/beam_node_bc_set.h>

namespace ofem {

	SmartPointer(BeamModel);

	class BeamModel : public Model {
	protected:
		virtual NodeBCSet* createBCSet();
		virtual ElementLoadSet* createElementLoadSet();
		virtual NodeLoadSet* createNodeLoadSet();
		virtual MaterialSet* createMaterialSet();
		virtual ElementSet* createElementSet();
		void connectMaterials();
	public:
		BeamModel();
		virtual ~BeamModel();

		ClassInfo("BeamModel", Model);

		// Get/set methods

		BeamMaterialSet* getMaterialSet();
		BeamSet* getElementSet();

	};
}