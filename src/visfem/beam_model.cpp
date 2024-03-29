#include <vfem/beam_model.h>

#include <vfem/beam.h>
#include <vfem/beam_load.h>
#include <vfem/node.h>
#include <vfem/node_bc.h>
#include <vfem/node_load.h>
#include <vfem/preferences.h>

#include <ofem/beam_load.h>
#include <ofem/beam_set.h>
#include <ofem/element_load_set.h>
#include <ofem/node_bc_set.h>
#include <ofem/node_set.h>
#include <ofem/solid_pipe_section.h>

#include <ofmath/ray_cylinder.h>

#include <ColorMap.h>
#include <ResultInfo.h>

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

using namespace vfem;

BeamModel::BeamModel() : ofem::BeamModel()
{
    m_nodeSize = 1.0;
    m_nodeType = IVF_NODE_GEOMETRY;
    m_lineRadius = 1.0;
    m_loadSize = 1.0;
    m_beamLoadSize = 1.0;
    m_scaleFactor = 1.0;
    m_lineSides = 6;
    m_nodeRepr = ivf::Node::NT_CUBE;

    m_colorTable = ColorTable::create();

    m_colorMapPos = ColorMap::create();
    m_colorMapNeg = ColorMap::create();
    m_colorMapStd = ColorMap::create();

    m_colorMapPosBlack = ColorMap::create();
    m_colorMapNegBlack = ColorMap::create();

    m_beamType = IVF_BEAM_SOLID;
    m_resultType = IVF_BEAM_NO_RESULT;

    if (vfem::Preferences::instance().useSphereNodes())
        m_nodeType = ivf::Node::NT_SPHERE;
    else
        m_nodeType = ivf::Node::NT_CUBE;

    m_colorMapPath = "";

    m_textFont = nullptr;
    m_showNodeNumbers = vfem::Preferences::instance().showNodeNumbers();
    m_showElementNumbers = false;
    m_camera = nullptr;

    m_useBlending = false;
    m_maxReactionForce = 0.0;
    m_maxReactionMoment = 0.0;
}

BeamModel::~BeamModel()
{}

vfem::BeamModelPtr vfem::BeamModel::create()
{
    return vfem::BeamModelPtr(new vfem::BeamModel());
}

void BeamModel::onInitialised()
{
    ofem::BeamMaterial *material = new ofem::BeamMaterial();
    material->setProperties(2.1e9, 8.1e7, 1.0, 1.0, 1.0, 1.0);
    material->setSectionType(ofem::ST_SolidPipe);
    material->setName("default");
    material->setColor(52);
    this->materialSet()->addMaterial(material);
    this->materialSet()->setCurrentMaterial(0);

    // Create default Node BCs

    m_defaultNodeFixedBC = ofem::BeamNodeBC::create();
    m_defaultNodeFixedBC->setName("fixed pos/rot");
    m_defaultNodeFixedBC->fixed();
    m_defaultNodeFixedBC->setReadOnly();
    this->nodeBCSet()->addBC(m_defaultNodeFixedBC.get());

    m_defaultNodePosBC = ofem::BeamNodeBC::create();
    m_defaultNodePosBC->setName("fixed pos");
    m_defaultNodePosBC->fixedPosition();
    m_defaultNodePosBC->setReadOnly();
    this->nodeBCSet()->addBC(m_defaultNodePosBC.get());
}

void vfem::BeamModel::onReadComplete()
{
    // Set the default node BCs after load

    for (auto i = 0; i < this->nodeBCSet()->getSize(); i++)
    {
        auto nodeBC = static_cast<ofem::BeamNodeBC *>(this->nodeBCSet()->getBC(i));
        if (nodeBC->getName() == "fixed pos/rot")
            m_defaultNodeFixedBC = ofem::BeamNodeBCPtr(nodeBC);
        if (nodeBC->getName() == "fixed pos")
            m_defaultNodePosBC = ofem::BeamNodeBCPtr(nodeBC);
    }
}

void BeamModel::generateModel()
{
    // Open color maps

    m_colorMapPos->open("red.map");
    m_colorMapNeg->open("blue.map");
    m_colorMapStd->open("colormap11.map");

    m_colorMapPosBlack->open("red_black.map");
    m_colorMapNegBlack->open("blue_black.map");

    // Temporary lists

    std::vector<Node *> ivfNodes;

    // Create nodes

    auto nodeSet = this->getNodeSet();

    for (int i = 0; i < nodeSet->getSize(); i++)
    {
        auto ivfNode = new vfem::Node();
        ivfNode->setBeamModel(this);
        ivfNode->setFemNode(nodeSet->getNode(i));
        ivfNode->setMaterial(m_nodeMaterial);
        ivfNode->setDirectRefresh(true);
        ivfNode->nodeLabel()->setSize(float(m_nodeSize * 1.5f));
        ivfNode->refresh();
        m_scene->addChild(ivfNode);
        ivfNodes.push_back(ivfNode);
        nodeSet->getNode(i)->setNumber(i + 1);
        nodeSet->getNode(i)->setUser(static_cast<void *>(ivfNode));
    }

    // Generate elements

    auto beamSet = this->getElementSet();

    for (int i = 0; i < beamSet->getSize(); i++)
    {
        auto ivfBeam = new Beam();
        auto femBeam = (ofem::Beam *)beamSet->getElement(i);
        ivfBeam->setBeam(femBeam);
        ivfBeam->setBeamModel(this);
        auto femNode1 = femBeam->getNode(0);
        auto femNode2 = femBeam->getNode(1);
        ivfBeam->setNodes(ivfNodes[femNode1->getNumber() - 1], ivfNodes[femNode2->getNumber() - 1]);
        ivfBeam->refresh();
        femBeam->setUser(static_cast<void *>(ivfBeam));
        m_scene->addChild(ivfBeam);
    }

    // Generate beam loads

    auto elementLoadSet = this->getElementLoadSet();

    for (int i = 0; i < elementLoadSet->getSize(); i++)
    {
        auto ivfBeamLoad = new vfem::BeamLoad();
        auto femBeamLoad = (ofem::BeamLoad *)elementLoadSet->getLoad(i);
        ivfBeamLoad->setBeamModel(this);
        ivfBeamLoad->setBeamLoad(femBeamLoad);
        femBeamLoad->setUser((void *)ivfBeamLoad);
        ivfBeamLoad->refresh();
        m_scene->addChild(ivfBeamLoad);
    }

    // Generate node loads

    auto nodeLoadSet = this->getNodeLoadSet();

    for (int i = 0; i < nodeLoadSet->getSize(); i++)
    {
        auto ivfNodeLoad = new vfem::NodeLoad();
        auto femNodeLoad = (ofem::BeamNodeLoad *)nodeLoadSet->getLoad(i);
        ivfNodeLoad->setBeamModel(this);
        ivfNodeLoad->setNodeLoad(femNodeLoad);
        femNodeLoad->setUser((void *)ivfNodeLoad);
        ivfNodeLoad->refresh();
        m_scene->addChild(static_cast<ivf::Shape *>(ivfNodeLoad));
    }

    // Generate node bcs

    ofem::NodeBCSet *nodeBCSet = this->getNodeBCSet();

    for (int i = 0; i < nodeBCSet->getSize(); i++)
    {
        auto ivfNodeBC = new vfem::NodeBC();
        auto femNodeBC = (ofem::BeamNodeBC *)nodeBCSet->getBC(i);
        ivfNodeBC->setBeamModel(this);
        ivfNodeBC->setNodeBC(femNodeBC);
        femNodeBC->setUser((void *)ivfNodeBC);
        ivfNodeBC->refresh();
        m_scene->addChild(static_cast<ivf::Shape *>(ivfNodeBC));
    }
}

void vfem::BeamModel::enumerate()
{
    this->getNodeSet()->enumerateNodes();
    this->getElementSet()->enumerateElements();

    ofem::NodeSet *nodeSet = this->getNodeSet();

    for (int i = 0; i < nodeSet->getSize(); i++)
    {
        nodeSet->getNode(i)->setNumber(i + 1);
    }
}

void BeamModel::setScene(ivf::Composite *scene)
{
    m_scene = scene;
}

ivf::Composite *BeamModel::getScene()
{
    return m_scene;
}

void BeamModel::setNodeSize(double size)
{
    m_nodeSize = size;
}

void BeamModel::setNodeType(int type)
{
    m_nodeType = type;
}

void BeamModel::setLineRadius(double radius)
{
    m_lineRadius = radius;
}

void BeamModel::setLineSides(int sides)
{
    m_lineSides = sides;
}

int vfem::BeamModel::getLineSides()
{
    return m_lineSides;
}

void BeamModel::setNodeMaterial(ivf::Material *material)
{
    m_nodeMaterial = material;
}

void BeamModel::setBeamMaterial(ivf::Material *material)
{
    m_beamMaterial = material;
}

ColorTable *BeamModel::getColorTable()
{
    return m_colorTable;
}

void BeamModel::setLoadSize(double size)
{
    m_loadSize = size;
}

void BeamModel::setBeamLoadSize(double size)
{
    m_beamLoadSize = size;
}

double BeamModel::getNodeSize()
{
    return m_nodeSize;
}

int BeamModel::getNodeType()
{
    return m_nodeType;
}

ivf::Material *BeamModel::getNodeMaterial()
{
    return m_nodeMaterial;
}

double BeamModel::getLineRadius()
{
    return m_lineRadius;
}

void BeamModel::setColorMaps(ColorMapPtr pos, ColorMapPtr neg, ColorMapPtr std)
{
    m_colorMapPos = pos;
    m_colorMapNeg = neg;
    m_colorMapStd = std;
}

void BeamModel::setBeamType(int type)
{
    m_beamType = type;
}

int BeamModel::getBeamType()
{
    return m_beamType;
}

void vfem::BeamModel::setTextFont(ivf::BitmapFont *font)
{
    m_textFont = font;
}

ivf::BitmapFont *vfem::BeamModel::textFont()
{
    return m_textFont;
}

void vfem::BeamModel::setShowNodeNumbers(bool flag)
{
    vfem::Preferences::instance().setShowNodeNumbers(flag);
    m_showNodeNumbers = flag;
}

bool vfem::BeamModel::showNodeNumbers()
{
    return vfem::Preferences::instance().showNodeNumbers();
}

void vfem::BeamModel::setCamera(ivf::Camera *camera)
{
    m_camera = camera;
}

ivf::Camera *vfem::BeamModel::camera()
{
    return m_camera;
}

void vfem::BeamModel::setMaxReactionForce(double force)
{
    m_maxReactionForce = force;
}

void vfem::BeamModel::setMaxReactionMoment(double moment)
{
    m_maxReactionMoment = moment;
}

double vfem::BeamModel::getMaxReactionForce()
{
    return m_maxReactionForce;
}

double vfem::BeamModel::getMaxReactionMoment()
{
    return m_maxReactionMoment;
}

ofem::BeamNodeBC *BeamModel::defaultNodePosBC()
{
    return m_defaultNodePosBC.get();
}

ofem::BeamNodeBC *BeamModel::defaultNodeFixedBC()
{
    return m_defaultNodeFixedBC.get();
}

void BeamModel::setResultType(int type)
{
    m_resultType = type;
}

int BeamModel::getResultType()
{
    return m_resultType;
}

ColorMapPtr BeamModel::getColorMapPos()
{
    return m_colorMapPos;
}

ColorMapPtr vfem::BeamModel::getColorMapNegBlack()
{
    return m_colorMapNegBlack;
}

ColorMapPtr vfem::BeamModel::getColorMapPosBlack()
{
    return m_colorMapPosBlack;
}

ColorMapPtr BeamModel::getColorMapNeg()
{
    return m_colorMapNeg;
}

ColorMapPtr BeamModel::getColorMapStd()
{
    return m_colorMapStd;
}

void BeamModel::setScaleFactor(double factor)
{
    m_scaleFactor = factor;
}

double BeamModel::getScaleFactor()
{
    return m_scaleFactor;
}

void vfem::BeamModel::setNodeRepr(ivf::Node::TNodeType type)
{
    if (type == ivf::Node::NT_SPHERE)
        vfem::Preferences::instance().setUseSphereNodes(true);
    else
        vfem::Preferences::instance().setUseSphereNodes(false);

    m_nodeRepr = type;
}

ivf::Node::TNodeType vfem::BeamModel::getNodeRepr()
{
    if (vfem::Preferences::instance().useSphereNodes())
        return ivf::Node::NT_SPHERE;
    else
        return ivf::Node::NT_CUBE;
}

void vfem::BeamModel::setUseBlending(bool flag)
{
    m_useBlending = flag;
}

bool vfem::BeamModel::getUseBlending()
{
    return m_useBlending;
}

double BeamModel::getBeamLoadSize()
{
    return m_beamLoadSize;
}

double BeamModel::getLoadSize()
{
    return m_loadSize;
}

void BeamModel::setPath(const std::string &path)
{
    m_colorMapPath = path;
    m_colorMapPos->setPath(m_colorMapPath);
    m_colorMapNeg->setPath(m_colorMapPath);
    m_colorMapStd->setPath(m_colorMapPath);
    m_colorMapPosBlack->setPath(m_colorMapPath);
    m_colorMapNegBlack->setPath(m_colorMapPath);
}

ivf::Shape *BeamModel::pick(int sx, int sy)
{
    if (m_camera != nullptr)
    {
        std::vector<ivf::Shape *> selectedShapes;
        double x, y, z, r;
        double vx, vy, vz;
        double d;
        double min_node_d = 1e300;
        double min_beam_d = 1e300;
        int selected_node_idx = -1;
        int selected_beam_idx = -1;
        ivf::Shape *min_node_shape = nullptr;
        ivf::Shape *min_beam_shape = nullptr;

        m_camera->getPosition(x, y, z);
        m_camera->pickVector(sx, sy).getComponents(vx, vy, vz);

        glm::vec3 p_orig{x, y, z};
        glm::vec3 v_dir{vx, vy, vz};
        v_dir = glm::normalize(v_dir);
        glm::vec3 p_intersect;
        glm::vec3 v_intersect_normal;

        r = this->getNodeSize();

        auto nodeSet = this->getNodeSet();

        for (int i = 0; i < nodeSet->getSize(); i++)
        {
            auto node = nodeSet->getNode(i);
            node->getCoord(x, y, z);

            if (glm::intersectRaySphere(p_orig, v_dir, glm::vec3(x, y, z), float(r), p_intersect, v_intersect_normal))
            {
                glm::vec3 v_orig_sphere = p_intersect - p_orig;
                d = v_orig_sphere.length();
                if (d < min_node_d)
                {
                    min_node_d = d;
                    selected_node_idx = i;
                    min_node_shape = static_cast<ivf::Shape *>(node->getUser());
                }
            }
        }
        if (min_node_shape != nullptr)
            std::cout << "node " << selected_node_idx << " found...\n";

        // Check beam hits

        r = this->getLineRadius();

        auto elementSet = this->getElementSet();

        double x0, y0, z0;
        double x1, y1, z1;

        for (int i = 0; i < elementSet->getSize(); i++)
        {
            auto beam = elementSet->getElement(i);
            auto n0 = beam->getNode(0);
            auto n1 = beam->getNode(1);

            n0->getCoord(x0, y0, z0);
            n1->getCoord(x1, y1, z1);

            glm::vec3 v_n0(x0, y0, z0);
            glm::vec3 v_n1(x1, y1, z1);
            glm::vec3 v_mid(0.5 * (x0 + x1), 0.5 * (y0 + y1), 0.5 * (z0 + z1));

            glm::vec3 v_cyl = v_n1 - v_n0;

            glm::vec3 v_normal;

            double d = glm::length(v_cyl);

            if (ofmath::intersectRayOrientedCylinder(p_orig, v_dir, float(r), float(d), v_n0, v_cyl, p_intersect,
                                                     v_normal))
            {
                glm::vec3 v_orig_sphere = v_mid - p_orig;

                d = glm::length(v_orig_sphere);

                if (d < min_beam_d)
                {
                    min_beam_d = d;
                    selected_beam_idx = i;
                    min_beam_shape = static_cast<ivf::Shape *>(beam->getUser());
                }
            }
        }
        if (min_beam_shape != nullptr)
            std::cout << "beam " << selected_beam_idx << " found...\n";

        if ((min_node_shape != nullptr) && (min_beam_shape != nullptr))
        {
            return min_node_shape;
        }

        if ((min_node_shape != nullptr) && (min_beam_shape == nullptr))
            return min_node_shape;

        if ((min_node_shape == nullptr) && (min_beam_shape != nullptr))
            return min_beam_shape;

        return nullptr;
    }
    else
        return nullptr;
}
