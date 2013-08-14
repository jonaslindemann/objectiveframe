#ifndef _FemSLFFEAWriter_H_
#define _FemSLFFEAWriter_H_

#include "FemInputFileWriter.h"
#include "FemBeamModel.h"
#include "FemBeamLoad.h"

#include <string>

FemSmartPointer(CFemSLFFEAWriter);

class CFemSLFFEAWriter : public CFemInputFileWriter {
private:
	string m_fileName;
public:
	CFemSLFFEAWriter ();
	virtual ~CFemSLFFEAWriter ();

	FemClassInfo("CFemSLFFEAWriter",CFemInputFileWriter);

	void save();

	void setFileName(const char* fileName);

	void saveToStream(std::ostream &out);
};
#endif
