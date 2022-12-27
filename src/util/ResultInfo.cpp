// ResultInfo.cpp: implementation of the CResultInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "ResultInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResultInfo::ResultInfo()
{
}

ResultInfo::~ResultInfo()
{
}

void ResultInfo::setMaxN(double maxN)
{
    m_maxN = maxN;
}

double ResultInfo::getMaxN()
{
    return m_maxN;
}

void ResultInfo::setMaxT(double maxT)
{
    m_maxT = maxT;
}

double ResultInfo::getMaxT()
{
    return m_maxT;
}

void ResultInfo::setMaxV(double maxV)
{
    m_maxV = maxV;
}

double ResultInfo::getMaxV()
{
    return m_maxV;
}

void ResultInfo::setMaxM(double maxM)
{
    m_maxM = maxM;
}

double ResultInfo::getMaxM()
{
    return m_maxM;
}

void ResultInfo::setMinN(double minN)
{
    m_minN = minN;
}

double ResultInfo::getMinN()
{
    return m_minN;
}

void ResultInfo::setMinT(double minT)
{
    m_minT = minT;
}

double ResultInfo::getMinT()
{
    return m_minT;
}

void ResultInfo::setMinV(double minV)
{
    m_minV = minV;
}

double ResultInfo::getMinV()
{
    return m_minV;
}

void ResultInfo::setMinM(double minM)
{
    m_minM = minM;
}

double ResultInfo::getMinM()
{
    return m_minM;
}

void ResultInfo::setMinNavier(double minNavier)
{
    m_minNavier = minNavier;
}

double ResultInfo::getMinNavier()
{
    return m_minNavier;
}

void ResultInfo::setMaxNavier(double maxNavier)
{
    m_maxNavier = maxNavier;
}

double ResultInfo::getMaxNavier()
{
    return m_maxNavier;
}
