#include <ofem/model_clip_board.h>

using namespace ofem;

ModelClipBoard::ModelClipBoard()
{
}

void ModelClipBoard::clear()
{
    m_nodes.clear();
    m_elements.clear();
}

void ModelClipBoard::addNode(ofem::Node* node)
{
    m_nodes.push_back(node);
}

void ModelClipBoard::addElement(ofem::Element* element)
{
    m_elements.push_back(element);
}

void ModelClipBoard::paste(ofem::Model* model)
{
    auto startNodeIdx = model->getNodeSet()->enumerateNodes();
    auto startElementIdx = model->getElementSet()->enumerateElements();

    auto nodeSet = model->getNodeSet();
    auto elementSet = model->getElementSet();

    double x, y, z;

    for (auto node : m_nodes)
    {
        node->getCoord(x, y, z);

        if (m_onCreateNode)
            m_onCreateNode(x, y, z);
    }

    for (auto element : m_elements)
    {
        auto n0 = element->getNode(0);
        auto n1 = element->getNode(1);

        if (m_onCreateElement)
            m_onCreateElement(n0->getNumber() + startNodeIdx, n1->getNumber() + startNodeIdx);
    }
}

void ModelClipBoard::assignOnCreateNode(std::function<void(double x, double y, double z)>& onCreateNode)
{
    m_onCreateNode = onCreateNode;
}

void ModelClipBoard::assignOnCreateElement(std::function<void(int i0, int i1)>& onCreateElement)
{
    m_onCreateElement = onCreateElement;
}
