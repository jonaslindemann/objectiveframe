#pragma once

#include <ofem/beam_load_set.h>
#include <ofem/beam_material_set.h>
#include <ofem/beam_node_bc_set.h>
#include <ofem/beam_node_load_set.h>
#include <ofem/beam_set.h>
#include <ofem/model.h>

namespace ofem {

SmartPointer(BeamModel);

// The numeric values are written to the model file and used by the REST API,
// so they must stay stable -- MassPerLength was added last and gets the next
// free value rather than a place in presentation order.
enum class SelfWeightMode {
    Density = 0,
    TotalLoad = 1,
    MassPerLength = 2
};

class BeamModel : public Model {
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
    double m_maxScale;
    double m_minScale;

    bool m_selfWeightEnabled;
    SelfWeightMode m_selfWeightMode;
    double m_gravity;
    double m_gravityScale;
    double m_totalWeight;
    double m_massPerLength;

protected:
    virtual NodeBCSet *createBCSet() override;
    virtual ElementLoadSet *createElementLoadSet() override;
    virtual NodeLoadSet *createNodeLoadSet() override;
    virtual MaterialSet *createMaterialSet() override;
    virtual ElementSet *createElementSet() override;
    virtual void connectMaterials() override;

public:
    BeamModel();
    virtual ~BeamModel();

    ClassInfo("BeamModel", Model);
    StdFactory(BeamModel);

    // static BeamModelPtr create();

    // Get/set methods

    BeamMaterialSet *getMaterialSet();
    BeamSet *getElementSet();

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

    void setMaxScale(double scale);
    void setMinScale(double scale);

    double maxScale();
    double minScale();

    bool selfWeightEnabled();
    void setSelfWeightEnabled(bool enabled);
    SelfWeightMode selfWeightMode();
    void setSelfWeightMode(SelfWeightMode mode);
    double gravity();
    void setGravity(double gravity);
    double gravityScale();
    void setGravityScale(double scale);
    double totalWeight();
    void setTotalWeight(double totalWeight);
    double massPerLength();
    void setMassPerLength(double massPerLength);

    // IO Methods

    virtual void saveToStream(std::ostream &out) override;
    virtual void readFromStream(std::istream &in) override;
};
} // namespace ofem
