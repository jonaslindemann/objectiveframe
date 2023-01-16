#include <ofem/beam_model.h>
#include <ofem/beam_set.h>

using namespace ofem;


BeamModel::BeamModel()
    : Model()
// TODO: check and complete member initialisation list!
{
}


BeamModel::~BeamModel()
{
}

BeamModelPtr ofem::BeamModel::create()
{
    return BeamModelPtr(new BeamModel());
}


void BeamModel::connectMaterials()
{
    BeamSet* elementSet = this->getElementSet();
    elementSet->connectMaterials(this->getMaterialSet());
}


ElementSet* BeamModel::createElementSet()
{
    return new BeamSet();
}


MaterialSet* BeamModel::createMaterialSet()
{
    return new BeamMaterialSet();
}


BeamSet* BeamModel::getElementSet()
{
    return (BeamSet*)(Model::getElementSet());
}


BeamMaterialSet* BeamModel::getMaterialSet()
{
    return (BeamMaterialSet*)(Model::getMaterialSet());
}


ElementLoadSet* BeamModel::createElementLoadSet()
{
    return new BeamLoadSet();
}


NodeLoadSet* BeamModel::createNodeLoadSet()
{
    return new BeamNodeLoadSet();
}


NodeBCSet* BeamModel::createBCSet()
{
    return new BeamNodeBCSet();
}

void BeamModel::setMaxM(double maxM)
{
    m_maxM = maxM;
}

double BeamModel::maxM()
{
    return m_maxM;
}

double BeamModel::maxV()
{
    return m_maxV;
}

void BeamModel::setMaxV(double maxV)
{
    m_maxV = maxV;
}

double BeamModel::maxT()
{
    return m_maxT;
}

void BeamModel::setMaxT(double maxT)
{
    m_maxT = maxT;
}

double BeamModel::maxN()
{
    return m_maxN;
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
    return m_maxNavier;
}

void BeamModel::setMinM(double minM)
{
    m_minM = minM;
}

double BeamModel::minV()
{
    return m_minV;
}

void BeamModel::setMinV(double minV)
{
    m_minV = minV;
}

double BeamModel::minT()
{
    return m_minT;
}

void BeamModel::setMinT(double minT)
{
    m_minT = minT;
}

double BeamModel::minN()
{
    return m_minN;
}

void BeamModel::setMinN(double minN)
{
    m_minN = minN;
}

double BeamModel::minM()
{
    return m_minM;
}

void BeamModel::setMinNavier(double minNavier)
{
    m_minNavier = minNavier;
}

double BeamModel::minNavier()
{
    return m_minNavier;
}
