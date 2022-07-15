#include <ofem/input_file_writer.h>

using namespace ofem;

// ------------------------------------------------------------
InputFileWriter::InputFileWriter ()
{
    m_femModel = NULL;
}

// ------------------------------------------------------------
InputFileWriter::~InputFileWriter ()
{
}

// ------------------------------------------------------------
void InputFileWriter::saveToStream (std::ostream &out)
{
}

// ------------------------------------------------------------
void InputFileWriter::setFemModel(Model *model)
{
    m_femModel = model;
}

// ------------------------------------------------------------
Model* InputFileWriter::getFemModel()
{
    return m_femModel;
}
