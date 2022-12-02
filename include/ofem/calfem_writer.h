#pragma once
#include <ofem/input_file_writer.h>

#include <memory>

namespace ofem
{

SmartPointer(CalfemWriter);

class CalfemWriter : public ofem::InputFileWriter
{
private:
    std::string m_filename;

public:
    CalfemWriter(const std::string fname);

    ClassInfo("CalfemWriter", InputFileWriter);

    static CalfemWriterPtr create(const std::string fname)
    {
        return CalfemWriterPtr(new CalfemWriter(fname));
    }

    void writeHeader(std::ostream& out);
    void writeImports(std::ostream& out);
    void beginArr(std::ostream& out, std::string name);
    void arrRow(std::ostream& out, std::vector<float> v);
    void arrRow(std::ostream& out, std::vector<int> v);
    void arrRow(std::ostream& out, std::vector<double> v);
    void endArr(std::ostream& out);

    virtual void saveToStream(std::ostream& out) override;
};

}
