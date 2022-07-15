#pragma once

#include <ofem/base.h>
#include <ofem/node.h>
#include <ofem/internal_dofs.h>

#include <vector>

namespace ofem {

    SmartPointer(Element);

    class Element : public Base {
    private:
        std::vector<NodePtr> m_nodes;
        std::vector<InternalDofsPtr> m_internalDofs;
        std::vector<long> m_nodeIndex;
        std::vector<double> m_properties;
        std::vector<double> m_values;
        double m_elementLoad[3];
        long m_number;
    public:
        Element();
        virtual ~Element();

        // Class info

        ClassInfo("Element", Base);

        // Methods

        void addNode(Node* node);
        void addProperty(double value);
        void clearProperties();
        void deleteAll();
        void clear();
        long getInternalDof(unsigned int localIdx, int localDof);
        long enumerateDofs(long count = 1);
        bool hasInternalDof(unsigned int localIdx, int localDof);
        void clearInternalDof(unsigned int localIdx, int localDof);
        void addInternalDof(unsigned int localIdx, int localDof);
        void setProperty(unsigned int idx, double value);

        void setValueSize(int size);
        size_t getValueSize();

        double getValue(unsigned int idx);
        void setValue(unsigned int idx, double value);

        // Get/set methods

        Node* getNode(unsigned int index);
        size_t getIndexSize();
        size_t getSize();
        long getElementIndex(unsigned int node);
        long getNumber();
        void setNumber(long number);
        void getElementLoad(double& fx, double& fy, double& fz);
        void setElementLoad(double fx, double fy, double fz);
        double getProperty(unsigned int idx);
        void setPropertySize(unsigned int size);
        size_t getPropertySize();

        // IO Methods

        virtual void readFromStream(std::istream& in);
        virtual void saveToStream(std::ostream& out);
        virtual void print(std::ostream& out);
    };
}
