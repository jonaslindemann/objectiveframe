#include <ofem/beam_model.h>
#include <ofem/beam_set.h>

using namespace ofem;

BeamModel::BeamModel()
    : Model(), m_maxN{-1e300}, m_minN{1e300}, m_maxT{-1e300}, m_minT{1e300}, m_maxM{-1e300}, m_minM{1e300},
      m_maxV{-1e300}, m_minV{1e300}, m_maxNavier{-1e300}, m_minNavier{1e300}, m_maxScale{1.0}, m_minScale{1.0}
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
