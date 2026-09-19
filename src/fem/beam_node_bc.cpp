#include <ofem/beam_node_bc.h>

#include <array>

using namespace ofem;

namespace {

// The default supports, in DefaultKind order. Keeping the names in one table
// means the persisted spelling and the pattern that produced it cannot drift
// apart - readFromStream() and applyDefault() both read this.
//
// "fixed pos/rot" and "fixed pos" are the two names older files already carry,
// so they must stay spelled exactly like that.

struct DefaultEntry {
    const char *name;
    bool dof[6];
};

constexpr std::array<DefaultEntry, 5> defaultTable = {{
    {"fixed pos/rot", {true, true, true, true, true, true}},
    {"fixed pos", {true, true, true, false, false, false}},
    {"roller x", {false, true, true, false, false, false}},
    {"roller y", {true, false, true, false, false, false}},
    {"roller z", {true, true, false, false, false, false}},
}};

} // namespace

const char *BeamNodeBC::defaultName(DefaultKind kind)
{
    return defaultTable[static_cast<size_t>(kind)].name;
}

bool BeamNodeBC::isDefaultName(const std::string &name)
{
    for (const auto &entry : defaultTable)
        if (name == entry.name)
            return true;

    return false;
}

void BeamNodeBC::applyDefault(DefaultKind kind)
{
    const auto &entry = defaultTable[static_cast<size_t>(kind)];

    this->release();

    for (int i = 0; i < 6; i++)
        if (entry.dof[i])
            this->prescribe(i + 1, 0.0);

    this->setName(entry.name);
}

BeamNodeBC::BeamNodeBC() : NodeBC()
{
    m_name = "Noname";
    m_color = 1;
}

BeamNodeBC::~BeamNodeBC()
{}

void BeamNodeBC::setName(const std::string &name)
{
    m_name = name;
}

const std::string BeamNodeBC::getName()
{
    return m_name;
}

void BeamNodeBC::saveToStream(std::ostream &out)
{
    using namespace std;
    NodeBC::saveToStream(out);
    if (m_name == "")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

void BeamNodeBC::readFromStream(std::istream &in)
{
    char buffer[255];
    NodeBC::readFromStream(in);
    in.getline(buffer, 255);
    in.getline(buffer, 255);
    this->setName(buffer);
    if (isDefaultName(this->getName()))
        this->setReadOnly(true);
    in >> m_color;
}

void BeamNodeBC::setColor(int color)
{
    m_color = color;
}

int BeamNodeBC::getColor()
{
    return m_color;
}
