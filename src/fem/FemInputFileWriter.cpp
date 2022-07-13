// Implementation of: public class CFemInputFileWriter

#include "FemInputFileWriter.h"

// ------------------------------------------------------------
FemInputFileWriter::FemInputFileWriter ()
{
    m_femModel = NULL;
}

// ------------------------------------------------------------
FemInputFileWriter::~FemInputFileWriter ()
{
}

// ------------------------------------------------------------
void FemInputFileWriter::saveToStream (std::ostream &out)
{
}

// ------------------------------------------------------------
void FemInputFileWriter::setFemModel(FemModel *model)
{
    m_femModel = model;
}

// ------------------------------------------------------------
FemModel* FemInputFileWriter::getFemModel()
{
    return m_femModel;
}
