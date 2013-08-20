#ifndef _CFemDFEMCInterface_h_
#define _CFemDFEMCInterface_h_

#include <FemBeamModel.h>

class CFemDFEMCInterface {
private:
    CFemBeamModel* m_beamModel;
    int m_argc;
    char** m_argv;
    double m_maxNodeValue;
public:
    CFemDFEMCInterface();
    virtual ~CFemDFEMCInterface();

    void setBeamModel(CFemBeamModel* model);
    void setArguments(int argc, char** argv);

    void execute();

    double getMaxNodeValue();
};

#endif
