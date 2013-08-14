// Implementation of: public class CFemInputFileWriter

#include "FemInputFileWriter.h"

// ------------------------------------------------------------
CFemInputFileWriter::CFemInputFileWriter ()
{
	m_femModel = NULL;
}

// ------------------------------------------------------------
CFemInputFileWriter::~CFemInputFileWriter ()
{
}

// ------------------------------------------------------------
void CFemInputFileWriter::saveToStream (std::ostream &out)
{
}

// ------------------------------------------------------------
void CFemInputFileWriter::setFemModel(CFemModel *model)
{
	m_femModel = model;
}

// ------------------------------------------------------------
CFemModel* CFemInputFileWriter::getFemModel()
{
	return m_femModel;
}
