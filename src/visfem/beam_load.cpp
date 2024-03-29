#include <ivf/Extrusion.h>
#include <ivfmath/Vec3d.h>
#include <vfem/beam_load.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

using namespace std;
using namespace ivf;
using namespace vfem;

BeamLoad::BeamLoad() : Shape()
{
    m_beamModel = nullptr;
    m_extrMaterial = ivf::Material::create();
    m_extrMaterial->setAmbientColor(0.3f, 0.0f, 0.0f, 1.0f);
    m_extrMaterial->setDiffuseColor(0.7f, 0.0f, 0.0f, 1.0f);
    m_extrMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    this->setMaterial(m_extrMaterial);

    m_arrowMaterial = ivf::Material::create();
    m_arrowMaterial->setAmbientColor(0.3f, 0.0f, 0.0f, 1.0f);
    m_arrowMaterial->setDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
    m_arrowMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_arrowMaterial->addReference();
    this->setUseName(true);
    this->setUseSelectShape(false);
}

BeamLoad::~BeamLoad()
{
    m_q.clear();
    m_arrow.clear();
}

void BeamLoad::refresh()
{
    ColorTable *colorTable;

    if (m_beamModel != nullptr)
        colorTable = m_beamModel->getColorTable();
    else
        colorTable = nullptr;

    if (m_colorTable == nullptr) {
        m_extrMaterial->setAmbientColor(0.3f, 0.0f, 0.0f, 1.0f);
        m_extrMaterial->setDiffuseColor(0.7f, 0.0f, 0.0f, 1.0f);
    }
    else {
        if (colorTable != nullptr) {
            colorTable->assignColor(m_beamLoad->getColor(), m_extrMaterial);
            colorTable->assignColor(m_beamLoad->getColor(), m_arrowMaterial);
        }
    }
    this->setBeamLoad(m_beamLoad);
    this->initExtrusion();
}

void BeamLoad::initExtrusion()
{
    Vec3d p1, p2;
    Vec3d v1;
    double x, y, z, ex, ey, ez;
    double lex, ley, lez;
    double loadHeight;
    double ax, ay, az;
    double bx, by, bz;

    if (m_beamLoad != nullptr) {

        for (int i = 0; i < m_q.size(); i++) {

            // Get beam

            ofem::Beam *femBeam = (ofem::Beam *)m_beamLoad->getElement(i);
            ofem::Node *node1 = femBeam->getNode(0);
            ofem::Node *node2 = femBeam->getNode(1);

            // Create section

            // m_q[i]->setUseTwist(true);

            m_beamLoad->getLocalDirection(lex, ley, lez);

            if (ley > 0)
                femBeam->getOrientationY(ex, ey, ez);
            else
                femBeam->getOrientationZ(ex, ey, ez);

            if (m_beamModel != nullptr)
                loadHeight = m_beamModel->getBeamLoadSize();
            else
                loadHeight = 1.0;

            // Set topolgy

            node1->getCoord(ax, ay, az);
            m_q[i]->setCoord(0, ax, ay, az);
            m_q[i]->setCoord(1, ax - ex * loadHeight, ay - ey * loadHeight, az - ez * loadHeight);
            node2->getCoord(bx, by, bz);
            m_q[i]->setCoord(2, bx - ex * loadHeight, by - ey * loadHeight, bz - ez * loadHeight);
            m_q[i]->setCoord(3, bx, by, bz);
            m_q[i]->refresh();

            // Define arrow

            m_arrow[i]->setSize(loadHeight, loadHeight * 0.20);
            m_arrow[i]->setRadius(loadHeight * 0.05, loadHeight * 0.03);
            if (m_beamLoad->getValue() < 0) {
                x = (ax + bx) / 2.0 - ex * loadHeight;
                y = (ay + by) / 2.0 - ey * loadHeight;
                z = (az + bz) / 2.0 - ez * loadHeight;
                m_arrow[i]->setPosition(x, y, z);
                m_arrow[i]->setDirection(ex, ey, ez);
            }
            else {
                x = (ax + bx) / 2.0;
                y = (ay + by) / 2.0;
                z = (az + bz) / 2.0;
                m_arrow[i]->setPosition(x, y, z);
                m_arrow[i]->setDirection(-ex, -ey, -ez);
            }
        }
    }
}

void BeamLoad::setBeamLoad(ofem::BeamLoad *load)
{
    int i;
    m_q.clear();
    m_arrow.clear();

    m_beamLoad = load;
    for (i = 0; i < m_beamLoad->getElementsSize(); i++) {
        QuadSetPtr q = QuadSet::create();
        m_q.push_back(q);
        IndexPtr idx = Index::create();
        idx->createLinear(4);
        q->addCoord(0.0, 0.0, 0.0);
        q->addCoord(0.0, 0.0, 0.0);
        q->addCoord(0.0, 0.0, 0.0);
        q->addCoord(0.0, 0.0, 0.0);
        q->addCoordIndex(idx);
        q->setMaterial(m_extrMaterial);

        auto arrow = ExtrArrow::create();
        m_arrow.push_back(arrow);
        arrow->setMaterial(m_arrowMaterial);
    }
}

void BeamLoad::doCreateGeometry()
{
    int old_style = ivfGetGLEJoinStyle();
    int i;
    // gleSetJoinStyle(TUBE_JN_ANGLE|TUBE_NORM_FACET|TUBE_JN_ANGLE);
    for (i = 0; i < m_q.size(); i++)
        m_q[i]->render();
    ivfSetGLEJoinStyle(TUBE_JN_ANGLE | TUBE_NORM_EDGE | TUBE_JN_ANGLE);
    for (i = 0; i < m_arrow.size(); i++)
        m_arrow[i]->render();
    ivfSetGLEJoinStyle(old_style);
}

void BeamLoad::doCreateSelect()
{
    int old_style = ivfGetGLEJoinStyle();
    int i;
    // gleSetJoinStyle(TUBE_JN_ANGLE|TUBE_NORM_FACET|TUBE_JN_ANGLE);
    for (i = 0; i < m_q.size(); i++) {
        m_q[i]->setSelect(Shape::SS_ON);
        m_q[i]->render();
        m_q[i]->setSelect(Shape::SS_OFF);
    }
    ivfSetGLEJoinStyle(TUBE_JN_ANGLE | TUBE_NORM_EDGE | TUBE_JN_ANGLE);
    for (i = 0; i < m_arrow.size(); i++) {
        m_arrow[i]->setSelect(Shape::SS_ON);
        m_arrow[i]->render();
        m_arrow[i]->setSelect(Shape::SS_OFF);
    }
    ivfSetGLEJoinStyle(old_style);
}

void BeamLoad::setBeamModel(BeamModel *model)
{
    m_beamModel = model;
}
