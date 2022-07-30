#pragma once

#include <ofem/base.h>
#include <ofem/model.h>

namespace ofem
{

SmartPointer(OutputFileReader);

class OutputFileReader : public Base
{
private:
    Model* m_femModel;

public:
    OutputFileReader();
    virtual ~OutputFileReader();

    ClassInfo("OutputFileReader", Base);

    Model* getFemModel();
    void setFemModel(Model* model);

    virtual void readFromStream(std::istream& in) override;
};
}
