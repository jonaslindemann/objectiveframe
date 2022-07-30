#pragma once

#include <ofem/beam_load.h>
#include <ofem/beam_model.h>
#include <ofem/input_file_writer.h>

#include <string>

namespace ofem
{

SmartPointer(SLFFEAWriter);

class SLFFEAWriter : public InputFileWriter
{
private:
public:
    SLFFEAWriter(const std::string filename);
    virtual ~SLFFEAWriter();

    ClassInfo("SLFFEAWriter", InputFileWriter);

    virtual void saveToStream(std::ostream& out) override;
};
}
