#include <ofem/input_file_writer.h>

using namespace ofem;
using namespace std;

#include <fstream>
#include <iostream>


InputFileWriter::InputFileWriter(const std::string filename)
{
    m_femModel = nullptr;
    m_filename = filename;
}

InputFileWriter::~InputFileWriter()
{
}

void ofem::InputFileWriter::doAfterSave()
{
}

void InputFileWriter::saveToStream(std::ostream& out)
{
}

InputFileWriterPtr ofem::InputFileWriter::create(const std::string filename)
{
    return InputFileWriterPtr(new InputFileWriter(filename));
}

void InputFileWriter::setFemModel(Model* model)
{
    m_femModel = model;
}

Model* InputFileWriter::getFemModel()
{
    return m_femModel;
}

const std::string ofem::InputFileWriter::filename()
{
    return m_filename;
}

void InputFileWriter::save()
{
    if (m_filename != "")
    {
        fstream inputFile;
        inputFile.open(m_filename, ios::out);
        this->saveToStream(inputFile);
        inputFile.close();

        this->doAfterSave();
    }
}
