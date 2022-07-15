#pragma once

#include <ofem/material_set.h>
#include <ofem/beam_material.h>

namespace ofem {

	class BeamMaterialSet : public MaterialSet {
	public:
		BeamMaterialSet();
		virtual ~BeamMaterialSet();

	protected:
		virtual Material* createMaterial();
	};

}
