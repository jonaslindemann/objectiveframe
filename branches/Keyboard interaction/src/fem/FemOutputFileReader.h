#ifndef _CFemOutputFileReader_h_
#define _CFemOutputFileReader_h_

#include "FemObject.h"
#include "FemModel.h"

FemSmartPointer(CFemOutputFileReader);

class CFemOutputFileReader : public CFemObject {
private:
    CFemModel* m_femModel;
public:
    CFemOutputFileReader();
    virtual ~CFemOutputFileReader();

    FemClassInfo("CFemOutputFileReader",CFemObject);

    CFemModel* getFemModel();
    void setFemModel(CFemModel* model);

    virtual void readFromStream(istream &in);
};

#endif
