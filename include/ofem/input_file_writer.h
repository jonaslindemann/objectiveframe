#pragma once

#include <ofem/base.h>
#include <ofem/model.h>

namespace ofem
{

SmartPointer(InputFileWriter);

class InputFileWriter : public Base
{
private:
    Model* m_femModel;
    std::string m_filename;

public:
    InputFileWriter(const std::string filename);
    virtual ~InputFileWriter();

    ClassInfo("InputFileWriter", Base);

    void setFemModel(Model* model);
    Model* getFemModel();

    const std::string filename();

    void save();

    virtual void doAfterSave();
    virtual void saveToStream(std::ostream& out) override;
};
}
