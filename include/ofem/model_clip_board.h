#pragma once

#include <ofem/base.h>
#include <ofem/element.h>
#include <ofem/model.h>
#include <ofem/node.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace ofem {

typedef std::function<void(double x, double y, double z)> ModelClipboardCreateNodeFunc;
typedef std::function<void(int i0, int i1)> ModelClipboardCreateElementFunc;

SmartPointer(ModelClipBoard);

class ModelClipBoard : public ofem::Base {
private:
    // Copied geometry is captured by value at addNode()/addElement() time, not
    // kept as pointers into the live model. The clipboard has to survive things
    // that invalidate every ofem::Node/Element the model owns - an undo/redo,
    // or a cancelled paste reverting to a snapshot - without going stale.

    struct ClipNode {
        double x, y, z;
    };

    struct ClipElement {
        int i0, i1; // indices into m_nodes
    };

    std::vector<ClipNode> m_nodes;
    std::vector<ClipElement> m_elements;

    // Valid only while addNode()/addElement() are being called to build up a
    // copy (i.e. between clear() and the next clear()). Lets addElement()
    // resolve a live node pointer to its clipboard index immediately, while
    // the pointer is still good.
    std::map<ofem::Node *, int> m_nodeIndex;

    double m_center[3];
    double m_offset[3];

    void calcCenter();

public:
    ModelClipBoard();

    ClassInfo("ModelClipBoard", Base);
    StdFactory(ModelClipBoard);

    void clear();

    bool isEmpty() const
    {
        return m_nodes.empty();
    }

    /** Caller must addNode() both endpoints of a beam before addElement()'ing it. */
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
