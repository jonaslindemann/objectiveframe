#pragma once

#include <ofem/base.h>

namespace ofem {

    SmartPointer(Load);

    class Load : public Base {
    private:
        long m_number;
        double m_value;
    public:
        Load();
        virtual ~Load();

        ClassInfo("Load", Base);

        // Get/set methods

        long getNumber();
        void setNumber(long number);
        double getValue();
        void setValue(double value);

        // IO Methods

        virtual void readFromStream(std::istream& in);
        virtual void saveToStream(std::ostream& out);
        virtual void print(std::ostream& out);
    };
}
