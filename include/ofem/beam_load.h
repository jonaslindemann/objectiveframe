#pragma once

#include <ofem/element_load.h>
#include <ofem/beam.h>

namespace ofem {

	SmartPointer(BeamLoad);

	class BeamLoad : public ElementLoad {
	private:
		std::string m_name;
		int m_color;
	public:
		BeamLoad();
		virtual ~BeamLoad();

		ClassInfo("BeamLoad", ElementLoad);

		const std::string getName();
		void setName(const std::string& name);
		void setColor(int color);
		int getColor();

		virtual void readFromStream(std::istream& in);
		virtual void saveToStream(std::ostream& out);
	};
}
