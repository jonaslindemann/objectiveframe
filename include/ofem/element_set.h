#pragma once

#include <ofem/base.h>
#include <ofem/element.h>
#include <ofem/node_set.h>

namespace ofem {

    SmartPointer(ElementSet);

    class ElementSet : public Base {
    private:
        std::vector<ElementPtr> m_elements;
    public:
        ElementSet();
        virtual ~ElementSet();

        // Class info

        ClassInfo("ElementSet", Base);

        // Methods

        void deleteAll();
        Element* removeElement(long i);
        bool deleteElement(long i);
        Element* getElement(long i);
        void addElement(Element* element);
        size_t getSize();
        void clear();
        long enumerateElements(long count = 1);
        void connectNodes(NodeSet* nodes);
        long enumerateDofs(long count = 1);
        bool removeElement(Element* element);

        // IO methods

        void readFromStream(std::istream& in);
        void saveToStream(std::ostream& out);
        virtual void print(std::ostream& out);
    protected:
        virtual Element* createElement();
    };
}
