#include <ofem/model_clip_board.h>

using namespace ofem;

ModelClipBoard::ModelClipBoard() : m_center{0.0, 0.0, 0.0}, m_offset{0.0, 0.0, 0.0}
{
}

void ModelClipBoard::calcCenter()
{
    double x, y, z;

    m_center[0] = 0.0;
    m_center[1] = 0.0;
    m_center[2] = 0.0;

    for (auto node : m_nodes) {
        node->getCoord(x, y, z);

        m_center[0] += x;
        m_center[1] += y;
        m_center[2] += z;
    }

    m_center[0] = m_center[0] / double(m_nodes.size());
    m_center[1] = m_center[1] / double(m_nodes.size());
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

    m_center[0] = 0.0;
    m_center[1] = 0.0;
    m_center[2] = 0.0;
}

void ModelClipBoard::addNode(ofem::Node *node)
{
    m_nodes.push_back(node);
}

void ModelClipBoard::addElement(ofem::Element *element)
{
    m_elements.push_back(element);
}

void ModelClipBoard::paste(ofem::Model *model)
{
    auto startNodeIdx = model->getNodeSet()->enumerateNodes();
    auto startElementIdx = model->getElementSet()->enumerateElements();

    auto nodeSet = model->getNodeSet();
    auto elementSet = model->getElementSet();

    double x, y, z;
    double nx, ny, nz;

    this->calcCenter();

    for (auto node : m_nodes) {
        node->getCoord(x, y, z);

        if (m_onCreateNode) {
            nx = (x - m_center[0]) + m_offset[0];
            ny = y + m_offset[1];
            nz = (z - m_center[2]) + m_offset[2];

            m_onCreateNode(nx, ny, nz);
        }
    }

    for (auto element : m_elements) {
        auto n0 = element->getNode(0);
        auto n1 = element->getNode(1);

        if (m_onCreateElement)
            m_onCreateElement(n0->getNumber() + startNodeIdx - 2, n1->getNumber() + startNodeIdx - 2);
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
