#ifndef _FemSLFFEAWriter_H_
#define _FemSLFFEAWriter_H_

#include "FemInputFileWriter.h"
#include "FemBeamModel.h"
#include "FemBeamLoad.h"

#include <string>

FemSmartPointer(FemSLFFEAWriter);

class FemSLFFEAWriter : public FemInputFileWriter {
private:
    string m_fileName;
public:
    FemSLFFEAWriter ();
    virtual ~FemSLFFEAWriter ();

    FemClassInfo("FemSLFFEAWriter",FemInputFileWriter);

    void save();

    void setFileName(const char* fileName);

    void saveToStream(std::ostream &out);
};
#endif
