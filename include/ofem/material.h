#pragma once

#include <ofem/base.h>

namespace ofem {

    SmartPointer(Material);

    class Material : public Base {
    private:
        long m_number;
    public:
        Material();
        virtual ~Material();

        ClassInfo("Material", Base);

        // Get/set methods

        long getNumber();
        void setNumber(long number);

        // IO Methods

        virtual void readFromStream(std::istream& in);
        virtual void saveToStream(std::ostream& out);
        virtual void print(std::ostream& out);
    };
}
