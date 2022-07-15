#pragma once

#include <ofem/element.h>
#include <ofem/dof.h>
#include <ofem/beam_material.h>
#include <ofem/node.h>

#include <vector>

namespace ofem {

	SmartPointer(Beam);

	class Beam : public Element {
	private:
		void eulrot(double* axis, double angle, double* u, double* v);
		BeamMaterialPtr    m_material;
		long				  m_materialIndex;
		double                m_beamRotation;
		int					  m_evaluationPoints;
	public:
		Beam();
		virtual ~Beam();

		ClassInfo("Beam", Element);

		// Methods

		// Get/set methods

		void getOrientationZ(double& ex, double& ey, double& ez);
		void getOrientationY(double& ex, double& ey, double& ez);
		BeamMaterial* getMaterial();
		void setMaterial(BeamMaterial* material);
		long getMaterialIndex();
		double getBeamRotation();
		void setBeamRotation(double angle);
		void setEvaluationPoints(int n);
		int getEvaluationPoints();

		// IO Methods

		virtual void readFromStream(std::istream& in);
		virtual void saveToStream(std::ostream& out);
		virtual void print(std::ostream& out);
	};
}
