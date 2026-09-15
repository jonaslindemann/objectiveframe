#include <ofem/model_clip_board.h>

using namespace ofem;

ModelClipBoard::ModelClipBoard() : m_center{0.0, 0.0, 0.0}, m_offset{0.0, 0.0, 0.0}
{
}

void ModelClipBoard::calcCenter()
{
    m_center[0] = 0.0;
    m_center[1] = 0.0;
    m_center[2] = 0.0;

    if (m_nodes.empty())
        return;

    // X/Z use the centroid so a paste lands under the click point the way it
    // looks on screen. Y uses the lowest point of the copied structure instead
    // of its average height, so e.g. a column pastes standing on the point you
    // clicked rather than floating with its midpoint there.

    bool first = true;

    for (auto &node : m_nodes) {
        m_center[0] += node.x;
        m_center[2] += node.z;

        if (first || (node.y < m_center[1])) {
            m_center[1] = node.y;
            first = false;
        }
    }

    m_center[0] = m_center[0] / double(m_nodes.size());
    m_center[2] = m_center[2] / double(m_nodes.size());
}

void ModelClipBoard::setOffset(double x, double y, double z)
{
    m_offset[0] = x;
    m_offset[1] = y;
    m_offset[2] = z;
}

void ModelClipBoard::clear()
{
    m_nodes.clear();
    m_elements.clear();
    m_nodeIndex.clear();

    m_center[0] = 0.0;
    m_center[1] = 0.0;
    m_center[2] = 0.0;
}

void ModelClipBoard::addNode(ofem::Node *node)
{
    double x, y, z;
    node->getCoord(x, y, z);

    m_nodeIndex[node] = static_cast<int>(m_nodes.size());
    m_nodes.push_back({x, y, z});
}

void ModelClipBoard::addElement(ofem::Element *element)
{
    auto n0 = element->getNode(0);
    auto n1 = element->getNode(1);

    auto it0 = m_nodeIndex.find(n0);
    auto it1 = m_nodeIndex.find(n1);

    // Both endpoints must already be on the clipboard -- callers must
    // addNode() a beam's endpoints before addElement()'ing it.
    if ((it0 == m_nodeIndex.end()) || (it1 == m_nodeIndex.end()))
        return;

    m_elements.push_back({it0->second, it1->second});
}

void ModelClipBoard::paste(ofem::Model *model)
{
    if (m_nodes.empty())
        return;

    // Copied geometry is captured by value, independent of whatever the model
    // has done since (including an undo/redo, or a paste that got cancelled
    // and reverted a snapshot) -- so this only ever reads m_nodes/m_elements,
    // never anything belonging to the live model.

    this->calcCenter();

    // m_onCreateNode() always succeeds and appends in order (see addNode() in
    // FemView.cpp), so the pasted node for clipboard index i lands at this
    // model index + i - no need to track created nodes to work that out.

    long baseIndex = static_cast<long>(model->getNodeSet()->getSize());

    for (auto &node : m_nodes) {
        double nx = (node.x - m_center[0]) + m_offset[0];
        double ny = (node.y - m_center[1]) + m_offset[1];
        double nz = (node.z - m_center[2]) + m_offset[2];

        if (m_onCreateNode)
            m_onCreateNode(nx, ny, nz);
    }

    for (auto &element : m_elements) {
        if (m_onCreateElement)
            m_onCreateElement(static_cast<int>(baseIndex + element.i0), static_cast<int>(baseIndex + element.i1));
    }
}

void ModelClipBoard::assignOnCreateNode(std::function<void(double x, double y, double z)> &onCreateNode)
{
    m_onCreateNode = onCreateNode;
}

void ModelClipBoard::assignOnCreateElement(std::function<void(int i0, int i1)> &onCreateElement)
{
    m_onCreateElement = onCreateElement;
}
