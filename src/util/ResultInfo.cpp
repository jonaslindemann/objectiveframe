// ResultInfo.cpp: implementation of the CResultInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "ResultInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResultInfo::CResultInfo()
{
}

CResultInfo::~CResultInfo()
{
}

void CResultInfo::setMaxN(double maxN)
{
    m_maxN = maxN;
}

double CResultInfo::getMaxN()
{
    return m_maxN;
}

void CResultInfo::setMaxT(double maxT)
{
    m_maxT = maxT;
}

double CResultInfo::getMaxT()
{
    return m_maxT;
}

void CResultInfo::setMaxV(double maxV)
{
    m_maxV = maxV;
}

double CResultInfo::getMaxV()
{
    return m_maxV;
}

void CResultInfo::setMaxM(double maxM)
{
    m_maxM = maxM;
}

double CResultInfo::getMaxM()
{
    return m_maxM;
}

void CResultInfo::setMinN(double minN)
{
    m_minN = minN;
}

double CResultInfo::getMinN()
{
    return m_minN;
}

void CResultInfo::setMinT(double minT)
{
    m_minT = minT;
}

double CResultInfo::getMinT()
{
    return m_minT;
}

void CResultInfo::setMinV(double minV)
{
    m_minV = minV;
}

double CResultInfo::getMinV()
{
    return m_minV;
}

void CResultInfo::setMinM(double minM)
{
    m_minM = minM;
}

double CResultInfo::getMinM()
{
    return m_minM;
}

void CResultInfo::setMinNavier(double minNavier)
{
    m_minNavier = minNavier;
}

double CResultInfo::getMinNavier()
{
    return m_minNavier;
}

void CResultInfo::setMaxNavier(double maxNavier)
{
    m_maxNavier = maxNavier;
}

double CResultInfo::getMaxNavier()
{
    return m_maxNavier;
}
