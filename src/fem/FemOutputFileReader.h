#ifndef _CFemOutputFileReader_h_
#define _CFemOutputFileReader_h_

#include "FemObject.h"
#include "FemModel.h"

FemSmartPointer(FemOutputFileReader);

class FemOutputFileReader : public FemObject {
private:
    FemModel* m_femModel;
public:
    FemOutputFileReader();
    virtual ~FemOutputFileReader();

    FemClassInfo("FemOutputFileReader",FemObject);

    FemModel* getFemModel();
    void setFemModel(FemModel* model);

    virtual void readFromStream(istream &in);
};

#endif
