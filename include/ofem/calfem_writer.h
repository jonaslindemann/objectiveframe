#pragma once
#include <ofem/input_file_writer.h>

#include <filesystem>
#include <memory>

namespace ofem {

SmartPointer(CalfemWriter);

class CalfemWriter : public ofem::InputFileWriter {
private:
    std::string m_filename;
    std::filesystem::path m_pythonPath;
    bool m_flipYZ;

public:
    CalfemWriter(const std::string fname, bool flipYZ = true);

    ClassInfo("CalfemWriter", InputFileWriter);

    static CalfemWriterPtr create(const std::string fname)
    {
        return CalfemWriterPtr(new CalfemWriter(fname));
    }

    void setPythonPath(std::filesystem::path pythonPath);

    void writeHeader(std::ostream &out);
    void writeImports(std::ostream &out);
    void beginArr(std::ostream &out, std::string name);
    void beginArr1D(std::ostream &out, std::string name);
    void arrRow(std::ostream &out, std::vector<float> v);
    void arrRow(std::ostream &out, std::vector<int> v);
    void arrRow(std::ostream &out, std::vector<double> v);
    void endArr(std::ostream &out);
    void endArr1D(std::ostream &out);

    void writeString(std::ostream &out, std::string name, const std::string &value);

    std::string readTemplate(std::string filename);

    virtual void saveToStream(std::ostream &out) override;
    virtual void doAfterSave() override;
};

} // namespace ofem
