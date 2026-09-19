#include <ofem/beam_model.h>
#include <ofem/beam_set.h>
#include <ofem/model_state_info.h>

using namespace ofem;
using namespace std;

BeamModel::BeamModel()
    : Model(), m_maxN{-1e300}, m_minN{1e300}, m_maxT{-1e300}, m_minT{1e300}, m_maxM{-1e300}, m_minM{1e300},
      m_maxV{-1e300}, m_minV{1e300}, m_maxNavier{-1e300}, m_minNavier{1e300}, m_maxScale{1.0}, m_minScale{1.0},
      m_selfWeightEnabled{false}, m_selfWeightMode{SelfWeightMode::Density}, m_gravity{9.81}, m_gravityScale{1.0},
      m_totalWeight{0.0}, m_massPerLength{0.0}
{
}

BeamModel::~BeamModel()
{
}

// BeamModelPtr ofem::BeamModel::create()
//{
//     return BeamModelPtr(new BeamModel());
// }

void BeamModel::connectMaterials()
{
    BeamSet *elementSet = this->getElementSet();
    elementSet->connectMaterials(this->getMaterialSet());
}

ElementSet *BeamModel::createElementSet()
{
    return new BeamSet();
}

MaterialSet *BeamModel::createMaterialSet()
{
    return new BeamMaterialSet();
}

BeamSet *BeamModel::getElementSet()
{
    return (BeamSet *)(Model::getElementSet());
}

BeamMaterialSet *BeamModel::getMaterialSet()
{
    return (BeamMaterialSet *)(Model::getMaterialSet());
}

ElementLoadSet *BeamModel::createElementLoadSet()
{
    return new BeamLoadSet();
}

NodeLoadSet *BeamModel::createNodeLoadSet()
{
    return new BeamNodeLoadSet();
}

NodeBCSet *BeamModel::createBCSet()
{
    return new BeamNodeBCSet();
}

void BeamModel::setMaxM(double maxM)
{
    m_maxM = maxM;
}

double BeamModel::maxM()
{
    return m_maxM * m_maxScale;
}

double BeamModel::maxV()
{
    return m_maxV * m_maxScale;
}

void BeamModel::setMaxV(double maxV)
{
    m_maxV = maxV;
}

double BeamModel::maxT()
{
    return m_maxT * m_maxScale;
}

void BeamModel::setMaxT(double maxT)
{
    m_maxT = maxT;
}

double BeamModel::maxN()
{
    return m_maxN * m_maxScale;
}

void BeamModel::setMaxN(double maxN)
{
    m_maxN = maxN;
}

void BeamModel::setMaxNavier(double maxNavier)
{
    m_maxNavier = maxNavier;
}

double BeamModel::maxNavier()
{
    return m_maxNavier * m_maxScale;
}

void BeamModel::setMinM(double minM)
{
    m_minM = minM;
}

double BeamModel::minV()
{
    return m_minV * m_minScale;
}

void BeamModel::setMinV(double minV)
{
    m_minV = minV;
}

double BeamModel::minT()
{
    return m_minT * m_minScale;
}

void BeamModel::setMinT(double minT)
{
    m_minT = minT;
}

double BeamModel::minN()
{
    return m_minN * m_minScale;
}

void BeamModel::setMinN(double minN)
{
    m_minN = minN;
}

double BeamModel::minM()
{
    return m_minM * m_minScale;
}

void BeamModel::setMinNavier(double minNavier)
{
    m_minNavier = minNavier;
}

double BeamModel::minNavier()
{
    return m_minNavier * m_minScale;
}

void BeamModel::setMaxScale(double scale)
{
    m_maxScale = scale;
}

void BeamModel::setMinScale(double scale)
{
    m_minScale = scale;
}

double BeamModel::maxScale()
{
    return m_maxScale;
}

double BeamModel::minScale()
{
    return m_minScale;
}

bool BeamModel::selfWeightEnabled()
{
    return m_selfWeightEnabled;
}

void BeamModel::setSelfWeightEnabled(bool enabled)
{
    m_selfWeightEnabled = enabled;
}

SelfWeightMode BeamModel::selfWeightMode()
{
    return m_selfWeightMode;
}

void BeamModel::setSelfWeightMode(SelfWeightMode mode)
{
    m_selfWeightMode = mode;
}

double BeamModel::gravity()
{
    return m_gravity;
}

void BeamModel::setGravity(double gravity)
{
    m_gravity = gravity;
}

double BeamModel::gravityScale()
{
    return m_gravityScale;
}

void BeamModel::setGravityScale(double scale)
{
    m_gravityScale = scale;
}

double BeamModel::totalWeight()
{
    return m_totalWeight;
}

void BeamModel::setTotalWeight(double totalWeight)
{
    m_totalWeight = totalWeight;
}

double BeamModel::massPerLength()
{
    return m_massPerLength;
}

void BeamModel::setMassPerLength(double massPerLength)
{
    m_massPerLength = massPerLength;
}

void BeamModel::saveToStream(std::ostream &out)
{
    Model::saveToStream(out);

    if (ModelStateInfo::getInstance().writeVersion() == "3") {
        out << endl << endl;
        out << (m_selfWeightEnabled ? 1 : 0) << " ";
        out << static_cast<int>(m_selfWeightMode) << " ";
        out << m_gravity << " ";
        out << m_gravityScale << " ";
        out << m_totalWeight << " ";
        out << m_massPerLength << endl;
    }
}

void BeamModel::readFromStream(std::istream &in)
{
    Model::readFromStream(in);

    if (ModelStateInfo::getInstance().readVersion() == "3") {
        int enabled;
        int mode;
        in >> enabled;
        in >> mode;
        in >> m_gravity;
        in >> m_gravityScale;
        in >> m_totalWeight;

        // m_massPerLength was appended to the version 3 block after version 3
        // had already been written out, so a file from before that simply ends
        // here -- read it only if it is actually there.
        double massPerLength;
        if (in >> massPerLength)
            m_massPerLength = massPerLength;
        else
            in.clear();

        m_selfWeightEnabled = (enabled != 0);
        m_selfWeightMode = static_cast<SelfWeightMode>(mode);
    }
}
