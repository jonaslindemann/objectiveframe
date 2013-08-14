#ifndef _CFemBeamLoad_H_
#define _CFemBeamLoad_H_

#include "FemElementLoad.h"
#include "FemBeam.h"

FemSmartPointer(CFemBeamLoad);

class CFemBeamLoad : public CFemElementLoad {
private:
	std::string m_name;
	int m_color;
public:
	CFemBeamLoad();
	virtual ~CFemBeamLoad ();

	FemClassInfo("CFemBeamLoad",CFemElementLoad);

	const std::string getName();
	void setName(const std::string& name);
	void setColor(int color);
	int getColor();

	virtual void readFromStream(std::istream &in);
	virtual void saveToStream(std::ostream &out);
};
#endif
