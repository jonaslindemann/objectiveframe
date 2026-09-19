#include <ofem/beam_node_load.h>

#include <cmath>
#include <sstream>

using namespace ofem;
using namespace std;

namespace {

// How close a direction has to be to an axis before the name says "Fy" rather
// than spelling the whole vector out, and how close two loads have to be before
// they are considered the same load. Direction components are already
// normalised when compared, so an absolute tolerance is the right kind here;
// the magnitude gets a relative one, because it carries the model's units.

constexpr double dirTolerance = 1.0e-6;
// Loose enough that two loads whose names print identically also compare
// identical - a load whose name is already taken but which refuses to merge
// would be indistinguishable in the load list.
constexpr double valueTolerance = 1.0e-6;

// Numbers go into names, where trailing zeros are noise: 5 rather than
// 5.000000, 2.5 rather than 2.500000.
//
// Default float formatting rather than fixed: it drops the trailing zeros by
// itself, and it keeps small magnitudes legible. A model worked in meganewtons
// has loads of 5e-06, which fixed notation would print as 0.000000 - every such
// load would then be named the same thing.

std::string trimNumber(double value)
{
    std::ostringstream ss;
    ss.precision(10);
    ss << value;
    std::string text = ss.str();

    // -0 reads as a mistake rather than as zero.

    if (text == "-0")
        text = "0";

    return text;
}

bool normalize(double &dx, double &dy, double &dz)
{
    double length = std::sqrt(dx * dx + dy * dy + dz * dz);

    if (length < dirTolerance)
        return false;

    dx /= length;
    dy /= length;
    dz /= length;

    return true;
}

} // namespace

std::string BeamNodeLoad::autoName(double dx, double dy, double dz, double value)
{
    if (!normalize(dx, dy, dz))
        return "F " + trimNumber(value);

    // An axis aligned load is named after its axis, with the direction's sign
    // folded into the magnitude - "Fy -5" is how the user asked for it, and
    // "Fy 5 (0, -1, 0)" would be the same thing said twice.

    const double component[3] = {dx, dy, dz};
    const char *axisName[3] = {"Fx", "Fy", "Fz"};

    for (int axis = 0; axis < 3; axis++)
    {
        if (std::abs(std::abs(component[axis]) - 1.0) > dirTolerance)
            continue;

        return std::string(axisName[axis]) + " " + trimNumber(std::copysign(value, component[axis]));
    }

    return "F " + trimNumber(value) + " (" + trimNumber(dx) + ", " + trimNumber(dy) + ", " + trimNumber(dz) + ")";
}

bool BeamNodeLoad::matches(double dx, double dy, double dz, double value)
{
    if (!normalize(dx, dy, dz))
        return false;

    double ox, oy, oz;
    this->getDirection(ox, oy, oz);

    if (!normalize(ox, oy, oz))
        return false;

    if (std::abs(ox - dx) > dirTolerance || std::abs(oy - dy) > dirTolerance || std::abs(oz - dz) > dirTolerance)
        return false;

    // Relative, so that two 5 kN loads match whether the model is worked in
    // newtons or meganewtons. An absolute tolerance would merge every load in a
    // model written in newtons and none in one written in meganewtons.

    // Purely relative, with no floor: a floor would be an absolute tolerance in
    // disguise, and would merge every load in a model whose units put the loads
    // below it. Two loads of exactly zero still compare equal, since the
    // difference is zero too.

    double own = this->getValue();
    double scale = std::abs(own);

    if (std::abs(value) > scale)
        scale = std::abs(value);

    return std::abs(own - value) <= valueTolerance * scale;
}

BeamNodeLoad::BeamNodeLoad() : NodeLoad(), m_name{"Noname"}, m_color{1}, m_scale{1.0}
{}

BeamNodeLoad::~BeamNodeLoad()
{}

void BeamNodeLoad::setName(const std::string &name)
{
    m_name = name;
}

const std::string BeamNodeLoad::getName()
{
    return m_name;
}

void ofem::BeamNodeLoad::setScale(double scale)
{
    m_scale = scale;
}

double ofem::BeamNodeLoad::getScale()
{
    return m_scale;
}

void BeamNodeLoad::saveToStream(std::ostream &out)
{
    NodeLoad::saveToStream(out);
    if (m_name == "")
        out << "Noname" << endl;
    else
        out << m_name << endl;
    out << m_color << endl;
}

void BeamNodeLoad::readFromStream(std::istream &in)
{
    char buffer[255];
    NodeLoad::readFromStream(in);
    in.getline(buffer, 255);
    in.getline(buffer, 255);
    this->setName(buffer);
    in >> m_color;
}

void BeamNodeLoad::setColor(int color)
{
    m_color = color;
}

int BeamNodeLoad::getColor()
{
    return m_color;
}
