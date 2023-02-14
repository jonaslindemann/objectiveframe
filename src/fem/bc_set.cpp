// Implementation of: public class CFemBCSet

#include <ofem/bc_set.h>
#include <ofem/node_bc.h>

using namespace ofem;

BCSet::BCSet()
    : Base()
{
}

BCSet::~BCSet()
{
    deleteAll();
}

void BCSet::print(std::ostream& out)
{
    using namespace std;
    for (unsigned int i = 0; i < m_bcs.size(); i++)
        out << m_bcs[i];
    out << endl;
}

json_nl BCSet::toJson()
{
    json_nl j = Base::toJson();

    json_nl bcList;

    for (auto& bc : m_bcs)
        bcList.push_back(bc->toJson());

    j["bcset"] = bcList;

    return j;
}

void BCSet::addBC(BC* bc)
{
    m_bcs.push_back(BCPtr(bc));
}

BC* BCSet::getBC(long i)
{
    if ((i >= 0) && (i < (long)m_bcs.size()))
        return m_bcs[i];
    else
        return NULL;
}

bool BCSet::deleteBC(long i)
{
    std::vector<BCPtr>::iterator p = m_bcs.begin();

    if ((i >= 0) && (i < (long)m_bcs.size()))
    {
        p += i;
        m_bcs.erase(p);
        return true;
    }
    return false;
}

BCPtr BCSet::removeBC(long i)
{
    std::vector<BCPtr>::iterator p = m_bcs.begin();

    if ((i >= 0) && (i < (long)m_bcs.size()))
    {
        BCPtr bc = m_bcs[i];
        if (!bc->isReadOnly())
        {
            p += i;
            m_bcs.erase(p);
            return bc;
        }
        else
            return nullptr;
    }
    else
        return nullptr;
}

void BCSet::deleteAll()
{
    m_bcs.clear();
}

void BCSet::clear()
{
    m_bcs.clear();
}

long BCSet::enumerateBCs(long count)
{
    for (auto i = 0; i < m_bcs.size(); i++)
        m_bcs[i]->setNumber(count++);
    return count;
}

size_t BCSet::getSize()
{
    return m_bcs.size();
}

void BCSet::saveToStream(std::ostream& out)
{
    using namespace std;
    Base::saveToStream(out);
    out << m_bcs.size() << endl;
    for (auto i = 0; i < m_bcs.size(); i++)
        m_bcs[i]->saveToStream(out);
}

void BCSet::readFromStream(std::istream& in)
{
    long nBCs;
    Base::readFromStream(in);
    in >> nBCs;
    deleteAll();
    for (auto i = 0; i < nBCs; i++)
    {
        BC* bc = createBC();
        bc->addReference();
        bc->readFromStream(in);
        m_bcs.push_back(bc);
    }
}

void BCSet::connectNodes(NodeSet* nodes)
{
    for (unsigned int i = 0; i < m_bcs.size(); i++)
    {
        BC* bc = m_bcs[i];
        if (bc->isClass("NodeBC"))
        {
            auto nodeBC = dynamic_cast<NodeBC*>(bc);
            // CFemNodeBC* nodeBC = (CFemNodeBC*) bc;
            nodeBC->clearNodes();
            for (auto j = 0; j < nodeBC->getNodeIndexSize(); j++)
            {
                Node* node = nodes->getNode(nodeBC->getNodeIndex(j) - 1);
                nodeBC->addNode(node);
            }
        }
    }
}

BC* BCSet::createBC()
{
    return new BC();
}

bool BCSet::removeBC(BC* bc)
{
    auto p = m_bcs.begin();

    while ((*p != bc) && (p != m_bcs.end()))
        p++;

    if (p != m_bcs.end())
    {
        if (!bc->isReadOnly())
        {
            m_bcs.erase(p);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}
