#pragma once

#include <ofem/output_file_reader.h>

#include <string>

namespace ofem
{

SmartPointer(SLFFEAReader);

class SLFFEAReader : public OutputFileReader
{
private:
    std::string m_fileName;
    std::fstream m_outputFile;

public:
    SLFFEAReader();
    virtual ~SLFFEAReader();

    ClassInfo("SLFFEAReader", OutputFileReader);

    void setFileName(const char* fileName);
    void load();

    virtual void readFromStream(std::istream& in) override;
};
}
