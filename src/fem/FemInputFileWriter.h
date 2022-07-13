// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _FemInputFileWriter_H_
#define _FemInputFileWriter_H_

#include "FemObject.h"
#include "FemModel.h"

FemSmartPointer(FemInputFileWriter);

class FemInputFileWriter : public FemObject {
private:
    FemModel* m_femModel;
public:
    FemInputFileWriter ();
    virtual ~FemInputFileWriter ();

    FemClassInfo("FemInputFileWriter",FemObject);

    void setFemModel(FemModel* model);
    FemModel* getFemModel();
    virtual void saveToStream (std::ostream &out);
};
#endif
