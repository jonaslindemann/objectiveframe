#pragma once

#include <ofem/base.h>
#include <memory>

namespace ofem {

SmartPointer(CalfemReader);

class CalfemReader : public Base {
private:
    std::string m_filename;
    std::string m_base64String;
    std::string m_df3String;
    bool m_isValid;

public:
    CalfemReader(const std::string fname);

    ClassInfo("CalfemReader", Base);

    static CalfemReaderPtr create(const std::string fname)
    {
        return CalfemReaderPtr(new CalfemReader(fname));
    }

    bool parse();

    virtual void readFromStream(std::istream &in);

    std::string base64String();
    std::string df3String();
    bool isValid();


};

} // namespace ofem
