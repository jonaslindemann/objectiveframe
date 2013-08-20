// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _FemInputFileWriter_H_
#define _FemInputFileWriter_H_

#include "FemObject.h"
#include "FemModel.h"

FemSmartPointer(CFemInputFileWriter);

class CFemInputFileWriter : public CFemObject {
private:
    CFemModel* m_femModel;
public:
    CFemInputFileWriter ();
    virtual ~CFemInputFileWriter ();

    FemClassInfo("CFemInputFileWriter",CFemObject);

    void setFemModel(CFemModel* model);
    CFemModel* getFemModel();
    virtual void saveToStream (std::ostream &out);
};
#endif
