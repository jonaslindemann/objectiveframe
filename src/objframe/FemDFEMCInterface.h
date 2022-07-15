#ifndef _CFemDFEMCInterface_h_
#define _CFemDFEMCInterface_h_

#include <ofem/beam_model.h>

class FrameDFEMCInterface {
private:
    ofem::BeamModel* m_beamModel;
    int m_argc;
    char** m_argv;
    double m_maxNodeValue;
public:
    FrameDFEMCInterface();
    virtual ~FrameDFEMCInterface();

    void setBeamModel(ofem::BeamModel* model);
    void setArguments(int argc, char** argv);

    void execute();

    double getMaxNodeValue();
};

#endif
