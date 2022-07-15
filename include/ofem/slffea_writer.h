#pragma once

#include <ofem/input_file_writer.h>
#include <ofem/beam_model.h>
#include <ofem/beam_load.h>

#include <string>

namespace ofem {

    SmartPointer(SLFFEAWriter);

    class SLFFEAWriter : public InputFileWriter {
    private:
        std::string m_fileName;
    public:
        SLFFEAWriter();
        virtual ~SLFFEAWriter();

        ClassInfo("SLFFEAWriter", InputFileWriter);

        void save();

        void setFileName(const char* fileName);

        void saveToStream(std::ostream& out);
    };
}
