#pragma once

#include <ofem/base.h>
#include <ofem/element.h>
#include <ofem/model.h>
#include <ofem/node.h>

#include <functional>
#include <string>
#include <vector>

namespace ofem {

typedef std::function<void(double x, double y, double z)> ModelClipboardCreateNodeFunc;
typedef std::function<void(int i0, int i1)> ModelClipboardCreateElementFunc;

SmartPointer(ModelClipBoard);

class ModelClipBoard : public ofem::Base {
private:
    std::vector<ofem::Node *> m_nodes;
    std::vector<ofem::Element *> m_elements;

    double m_center[3];
    double m_offset[3];

    void calcCenter();

public:
    ModelClipBoard();

    ClassInfo("ModelClipBoard", Base);
    StdFactory(ModelClipBoard);

    void clear();

    void addNode(ofem::Node *node);
    void addElement(ofem::Element *element);

    void paste(ofem::Model *model);

    void setOffset(double x, double y, double z);

    void assignOnCreateNode(std::function<void(double x, double y, double z)> &onCreateNode);
    void assignOnCreateElement(std::function<void(int i0, int i1)> &onCreateElement);

protected:
    std::function<void(double x, double y, double z)> m_onCreateNode;
    std::function<void(int i0, int i1)> m_onCreateElement;
};

}; // namespace ofem
