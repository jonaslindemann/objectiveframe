// FemOutputFileReader.cpp: implementation of the CFemOutputFileReader class.
//
//////////////////////////////////////////////////////////////////////

#include "FemOutputFileReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FemOutputFileReader::FemOutputFileReader()
{
    m_femModel = NULL;
}

FemOutputFileReader::~FemOutputFileReader()
{

}

void FemOutputFileReader::setFemModel(FemModel *model)
{
    m_femModel = model;
}

FemModel* FemOutputFileReader::getFemModel()
{
    return m_femModel;
}

void FemOutputFileReader::readFromStream(std::istream &in)
{

}
