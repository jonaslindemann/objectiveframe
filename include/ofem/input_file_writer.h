#pragma once 

#include <ofem/base.h>
#include <ofem/model.h>

namespace ofem {

    SmartPointer(InputFileWriter);

    class InputFileWriter : public Base {
    private:
        Model* m_femModel;
    public:
        InputFileWriter();
        virtual ~InputFileWriter();

        ClassInfo("InputFileWriter", Base);

        void setFemModel(Model* model);
        Model* getFemModel();
        virtual void saveToStream(std::ostream& out);
    };
}
