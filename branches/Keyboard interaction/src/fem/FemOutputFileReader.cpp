// FemOutputFileReader.cpp: implementation of the CFemOutputFileReader class.
//
//////////////////////////////////////////////////////////////////////

#include "FemOutputFileReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFemOutputFileReader::CFemOutputFileReader()
{
    m_femModel = NULL;
}

CFemOutputFileReader::~CFemOutputFileReader()
{

}

void CFemOutputFileReader::setFemModel(CFemModel *model)
{
    m_femModel = model;
}

CFemModel* CFemOutputFileReader::getFemModel()
{
    return m_femModel;
}

void CFemOutputFileReader::readFromStream(std::istream &in)
{

}
