#pragma once

#include <ofem/beam_load_set.h>
#include <ofem/beam_material_set.h>
#include <ofem/beam_node_bc_set.h>
#include <ofem/beam_node_load_set.h>
#include <ofem/beam_set.h>
#include <ofem/model.h>

namespace ofem
{

SmartPointer(BeamModel);

class BeamModel : public Model
{
private:
    double m_maxN;
    double m_minN;
    double m_maxT;
    double m_minT;
    double m_maxM;
    double m_minM;
    double m_maxV;
    double m_minV;
    double m_maxNavier;
    double m_minNavier;

protected:
    virtual NodeBCSet* createBCSet() override;
    virtual ElementLoadSet* createElementLoadSet() override;
    virtual NodeLoadSet* createNodeLoadSet() override;
    virtual MaterialSet* createMaterialSet() override;
    virtual ElementSet* createElementSet() override;
    virtual void connectMaterials() override;

public:
    BeamModel();
    virtual ~BeamModel();

    ClassInfo("BeamModel", Model);

    static BeamModelPtr create();

    // Get/set methods

    BeamMaterialSet* getMaterialSet();
    BeamSet* getElementSet();

    void setMaxM(double maxM);
    double maxV();
    void setMaxV(double maxV);
    double maxT();
    void setMaxT(double maxT);
    double maxN();
    void setMaxN(double maxN);
    double maxM();
    void setMaxNavier(double maxNavier);
    double maxNavier();

    void setMinM(double minM);
    double minV();
    void setMinV(double minV);
    double minT();
    void setMinT(double minT);
    double minN();
    void setMinN(double minN);
    double minM();
    void setMinNavier(double minNavier);
    double minNavier();
};
}
