#include <ofem/node_set.h>

using namespace ofem;

NodeSet::NodeSet() : Base()
{
}

NodeSet::~NodeSet()
{
    deleteAll();
}

void NodeSet::print(std::ostream &out)
{
    using namespace std;
    for (unsigned int i = 0; i < m_nodes.size(); i++)
        out << m_nodes[i];
    out << endl;
}

void NodeSet::addNode(Node *node)
{
    m_nodes.push_back(NodePtr(node));
}

Node *NodeSet::getNode(long i)
{
    if ((i >= 0) && (i < (long)m_nodes.size()))
        return m_nodes[i].get();
    else
        return nullptr;
}

bool NodeSet::deleteNode(long i)
{
    if ((i >= 0) && (i < (long)m_nodes.size())) {
        if (m_nodes[i]->getRefCount() == 1) {
            m_nodes.erase(m_nodes.begin() + i);
            return true;
        }
        else
            return false;
    }
    return false;
}

bool NodeSet::removeNode(long i)
{
    if ((i >= 0) && (i < (long)m_nodes.size())) {
        if (m_nodes[i]->getRefCount() == 1) {
            NodePtr node = m_nodes[i];
            m_nodes.erase(m_nodes.begin() + i);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool NodeSet::removeNode(Node *node)
{
    for (unsigned int i = 0; i < m_nodes.size(); i++) {
        if (node == m_nodes[i].get())
            return this->deleteNode(i);
    }
    return false;
}

void NodeSet::deleteAll()
{
    m_nodes.clear();
}

void NodeSet::clear()
{
    m_nodes.clear();
}

void NodeSet::clearNodeValues()
{
    for (unsigned int i = 0; i < m_nodes.size(); i++) {
        NodePtr node = m_nodes[i];
        node->clearValues();
    }
}

size_t NodeSet::getSize()
{
    return m_nodes.size();
}

long NodeSet::enumerateNodes(long count)
{
    for (unsigned int i = 0; i < m_nodes.size(); i++)
        m_nodes[i]->setNumber(count++);
    return count;
}

long NodeSet::enumerateDofs(long count)
{
    for (unsigned int i = 0; i < m_nodes.size(); i++)
        count = m_nodes[i]->enumerateDofs(count);
    return count;
}

void ofem::NodeSet::resetNodeKind(NodeKind newKind)
{
    for (auto &node : m_nodes)
        node->setKind(newKind);
}

void NodeSet::saveToStream(std::ostream &out)
{
    using namespace std;
    Base::saveToStream(out);
    out << m_nodes.size() << endl;
    for (unsigned int i = 0; i < m_nodes.size(); i++) {
        NodePtr node = m_nodes[i];
        node->saveToStream(out);
    }
}

json_nl NodeSet::toJson()
{
    json_nl j = Base::toJson();

    json_nl nodeList;

    for (auto &node : m_nodes)
        nodeList.push_back(node->toJson());

    j["nodeset"] = nodeList;

    return j;
}

void NodeSet::readFromStream(std::istream &in)
{
    long nNodes;

    Base::readFromStream(in);
    deleteAll();
    in >> nNodes;
    for (int i = 0; i < nNodes; i++) {
        NodePtr node = Node::create();
        node->readFromStream(in);
        m_nodes.push_back(node);
    }
}
