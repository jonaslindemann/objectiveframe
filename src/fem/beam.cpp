// Implementation of: public class CFemBeam

#include <ofem/beam.h>

#include <ofem/model_state_info.h>

#include <cmath>

using namespace ofem;
using namespace std;

Beam::Beam() : Element(), m_materialIndex{-1}, m_beamRotation{0.0}, m_evaluationPoints{8}, m_beamType{btBeam}
{
}

Beam::~Beam()
{
}

void Beam::print(std::ostream &out)
{
    Element::print(out);
}

void Beam::saveToStream(std::ostream &out)
{
    Element::saveToStream(out);
    out << m_beamRotation << " ";
    if (m_material != NULL)
        out << this->getMaterial()->getNumber() << endl;
    else
        out << -1 << endl;

    if (ModelStateInfo::getInstance().writeVersion() == "2") {
        if (m_beamType == btBeam)
            out << 0 << "\n";
        else
            out << 1 << "\n";
    }
}

void Beam::readFromStream(std::istream &in)
{
    Element::readFromStream(in);
    in >> m_beamRotation;
    in >> m_materialIndex;

    if (ModelStateInfo::getInstance().readVersion() == "2") {
        int beamType;
        in >> beamType;

        if (beamType == 0)
            m_beamType = btBeam;
        else
            m_beamType = btBar;
    }
}

void Beam::setMaterial(BeamMaterial *material)
{
    m_material = BeamMaterialPtr(material);
}

BeamMaterial *Beam::getMaterial()
{
    return m_material.get();
}

long Beam::getMaterialIndex()
{
    return m_materialIndex;
}

void Beam::setBeamRotation(double angle)
{
    m_beamRotation = angle;
}

double Beam::getBeamRotation()
{
    return m_beamRotation;
}

void ofem::Beam::addNode(Node *node)
{
    ofem::Element::addNode(node);

    switch (m_beamType) {
    case btBeam:
        if (node->getKind() == ofem::nkNotConnected)
            node->setKind(ofem::nk6Dof);
        if (node->getKind() == ofem::nk3Dof)
            node->setKind(ofem::nk6Dof);
        break;
    case btBar:
        if (node->getKind() == ofem::nkNotConnected)
            node->setKind(ofem::nk3Dof);
        break;
    default:
        break;
    }
}

void Beam::getOrientationZ(double &ex, double &ey, double &ez)
{
    double x1, y1, z1, x2, y2, z2;
    double dx, dy, dz;
    double l, alfa;
    double axis[3], u[3], v[3];

    auto n1 = this->getNode(0);
    auto n2 = this->getNode(1);

    n1->getCoord(x1, y1, z1);
    n2->getCoord(x2, y2, z2);

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    l = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

    dx = dx / l;
    dy = dy / l;
    dz = dz / l;

    if ((dx == 0.0) && (dz == 0.0))
        alfa = M_PI;
    else
        alfa = atan2(dy, dx) - M_PI / 2.0;

    axis[0] = 0.0;
    axis[1] = 0.0;
    axis[2] = 1.0;

    u[0] = 1.0;
    u[1] = 0.0;
    u[2] = 0.0;

    eulrot(axis, alfa, u, v);

    axis[0] = dx;
    axis[1] = dy;
    axis[2] = dz;

    u[0] = v[0];
    u[1] = v[1];
    u[2] = v[2];

    eulrot(axis, (-m_beamRotation + 90.0) * 2 * M_PI / 360.0, u, v);

    ex = v[0];
    ey = v[1];
    ez = v[2];
}

void Beam::getOrientationY(double &ex, double &ey, double &ez)
{
    double x1, y1, z1, x2, y2, z2;
    double dx, dy, dz;
    double l, alfa;
    double axis[3], u[3], v[3];

    auto n1 = this->getNode(0);
    auto n2 = this->getNode(1);

    n1->getCoord(x1, y1, z1);
    n2->getCoord(x2, y2, z2);

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    l = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

    dx = dx / l;
    dy = dy / l;
    dz = dz / l;

    if ((dx == 0.0) && (dz == 0.0))
        alfa = M_PI;
    else
        alfa = atan2(dy, dx) - M_PI / 2.0;

    axis[0] = 0.0;
    axis[1] = 0.0;
    axis[2] = 1.0;

    u[0] = 1.0;
    u[1] = 0.0;
    u[2] = 0.0;

    eulrot(axis, alfa, u, v);

    axis[0] = dx;
    axis[1] = dy;
    axis[2] = dz;

    u[0] = v[0];
    u[1] = v[1];
    u[2] = v[2];

    eulrot(axis, (-m_beamRotation) * 2 * M_PI / 360.0, u, v);

    ex = v[0];
    ey = v[1];
    ez = v[2];
}

void Beam::eulrot(double *axis, double angle, double *u, double *v)
{
    double c, s, a;
    double n1, n2, n3;

    c = cos(angle);
    s = sin(angle);
    a = 1 - c;

    n1 = axis[0];
    n2 = axis[1];
    n3 = axis[2];

    v[0] = u[0] * (a * pow(n1, 2) + c) + u[1] * (a * n1 * n2 - s * n3) + u[2] * (a * n1 * n3 + s * n2);
    v[1] = u[0] * (a * n1 * n2 + s * n3) + u[1] * (a * pow(n2, 2) + c) + u[2] * (a * n2 * n3 - s * n1);
    v[2] = u[0] * (a * n1 * n3 - s * n2) + u[1] * (a * n2 * n3 + s * n1) + u[2] * (a * pow(n3, 2) + c);
}

void Beam::setEvaluationPoints(int n)
{
    m_evaluationPoints = n;
}

int Beam::getEvaluationPoints()
{
    return m_evaluationPoints;
}

void ofem::Beam::setBeamType(BeamType type)
{
    m_beamType = type;
}

BeamType ofem::Beam::beamType()
{
    return m_beamType;
}
