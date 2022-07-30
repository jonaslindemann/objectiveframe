#include <ofem/output_file_reader.h>

using namespace ofem;

OutputFileReader::OutputFileReader()
{
    m_femModel = NULL;
}

OutputFileReader::~OutputFileReader()
{
}

void OutputFileReader::setFemModel(Model* model)
{
    m_femModel = model;
}

Model* OutputFileReader::getFemModel()
{
    return m_femModel;
}

void OutputFileReader::readFromStream(std::istream& in)
{
}
