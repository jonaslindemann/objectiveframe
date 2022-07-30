#include <vfem/beam_model.h>

#include <vfem/beam.h>
#include <vfem/beam_load.h>
#include <vfem/node.h>
#include <vfem/node_bc.h>
#include <vfem/node_load.h>

#include <ofem/beam_load.h>
#include <ofem/beam_set.h>
#include <ofem/element_load_set.h>
#include <ofem/node_bc_set.h>
#include <ofem/node_set.h>
#include <ofem/solid_pipe_section.h>

#include <ColorMap.h>
#include <ResultInfo.h>

using namespace ivf;
using namespace vfem;
using namespace std;

BeamModel::BeamModel()
    : ofem::BeamModel()
{
    m_nodeSize = 1.0;
    m_nodeType = ivf::Node::NT_CUBE;
    m_lineRadius = 1.0;
    m_loadSize = 1.0;
    m_beamLoadSize = 1.0;
    m_scaleFactor = 1.0;

    m_colorTable = new ColorTable();

    m_colorMapPos = new CColorMap();
    m_colorMapNeg = new CColorMap();
    m_colorMapStd = new CColorMap();

    m_resultInfo = new CResultInfo();

    m_beamType = IVF_BEAM_SOLID;
    m_resultType = IVF_BEAM_NO_RESULT;
    m_nodeType = ivf::Node::NT_CUBE;

    m_colorMapPath = "";

    m_textFont = nullptr;
    m_showNodeNumbers = false;
    m_showElementNumbers = false;
    m_camera = nullptr;
}

BeamModel::~BeamModel()
{
    delete m_colorMapPos;
    delete m_colorMapNeg;
    delete m_colorMapStd;
    delete m_resultInfo;
}

void BeamModel::onInitialised()
{
    ofem::BeamMaterialPtr material = new ofem::BeamMaterial();
    material->setProperties(2.1e9, 8.1e7, 1.0, 1.0, 1.0, 1.0);
    material->setSectionType(ofem::ST_SolidPipe);
    material->setName("default");
    material->setColor(52);
    this->materialSet()->addMaterial(material);
    this->materialSet()->setCurrentMaterial(0);

    // Create default Node BCs

    m_defaultNodeFixedBC = new ofem::BeamNodeBC();
    m_defaultNodeFixedBC->setName("fixed pos/rot");
    m_defaultNodeFixedBC->fixed();
    m_defaultNodeFixedBC->setReadOnly();
    this->nodeBCSet()->addBC(m_defaultNodeFixedBC);

    m_defaultNodePosBC = new ofem::BeamNodeBC();
    m_defaultNodePosBC->setName("fixed pos");
    m_defaultNodePosBC->fixedPosition();
    m_defaultNodePosBC->setReadOnly();
    this->nodeBCSet()->addBC(m_defaultNodePosBC);
}

void BeamModel::generateModel()
{

    // Open color maps

    m_colorMapPos->open("red.map");
    m_colorMapNeg->open("blue.map");
    m_colorMapStd->open("colormap11.map");

    // Temporary lists

    vector<Node*> ivfNodes;

    // Create nodes

    ofem::NodeSet* nodeSet = this->getNodeSet();

    for (int i = 0; i < nodeSet->getSize(); i++)
    {
        vfem::Node* ivfNode = new vfem::Node();
        ivfNode->setBeamModel(this);
        ivfNode->setFemNode(nodeSet->getNode(i));
        ivfNode->setMaterial(m_nodeMaterial);
        ivfNode->setDirectRefresh(true);
        ivfNode->nodeLabel()->setSize(m_nodeSize * 1.5);
        ivfNode->refresh();
        // if (m_pnodeSize!=nullptr)
        //	ivfNode->setNodeSize(*m_pnodeSize);
        m_scene->addChild(ivfNode);
        ivfNodes.push_back(ivfNode);
        nodeSet->getNode(i)->setNumber(i + 1);
    }

    // Generate elements

    ofem::BeamSet* beamSet = this->getElementSet();

    for (int i = 0; i < beamSet->getSize(); i++)
    {
        vfem::Beam* ivfBeam = new Beam();
        ofem::Beam* femBeam = (ofem::Beam*)beamSet->getElement(i);
        ivfBeam->setBeam(femBeam);
        // ivfBeam->setMaterial(m_beamMaterial);
        ivfBeam->setBeamModel(this);
        ofem::Node* femNode1 = femBeam->getNode(0);
        ofem::Node* femNode2 = femBeam->getNode(1);
        ivfBeam->setNodes(ivfNodes[femNode1->getNumber() - 1], ivfNodes[femNode2->getNumber() - 1]);
        ivfBeam->refresh();
        m_scene->addChild(ivfBeam);
    }

    // Generate beam loads

    ofem::ElementLoadSet* elementLoadSet = this->getElementLoadSet();

    for (int i = 0; i < elementLoadSet->getSize(); i++)
    {
        vfem::BeamLoad* ivfBeamLoad = new vfem::BeamLoad();
        ofem::BeamLoad* femBeamLoad = (ofem::BeamLoad*)elementLoadSet->getLoad(i);
        ivfBeamLoad->setBeamModel(this);
        ivfBeamLoad->setBeamLoad(femBeamLoad);
        femBeamLoad->setUser((void*)ivfBeamLoad);
        // ivfBeamLoad->setLoadHeight(*m_pbeamLoadSize);
        // ivfBeamLoad->setColorTable(this->getColorTable());
        ivfBeamLoad->refresh();
        m_scene->addChild(ivfBeamLoad);
    }

    // Generate node loads

    ofem::NodeLoadSet* nodeLoadSet = this->getNodeLoadSet();

    for (int i = 0; i < nodeLoadSet->getSize(); i++)
    {
        vfem::NodeLoad* ivfNodeLoad = new vfem::NodeLoad();
        ofem::BeamNodeLoad* femNodeLoad = (ofem::BeamNodeLoad*)nodeLoadSet->getLoad(i);
        ivfNodeLoad->setBeamModel(this);
        ivfNodeLoad->setNodeLoad(femNodeLoad);
        femNodeLoad->setUser((void*)ivfNodeLoad);
        // ivfNodeLoad->setColorTable(this->getColorTable());
        // ivfNodeLoad->setLoadHeight(*m_ploadSize);
        ivfNodeLoad->refresh();
        m_scene->addChild(static_cast<ivf::Shape*>(ivfNodeLoad));
    }

    // Generate node bcs

    ofem::NodeBCSet* nodeBCSet = this->getNodeBCSet();

    for (int i = 0; i < nodeBCSet->getSize(); i++)
    {
        vfem::NodeBC* ivfNodeBC = new vfem::NodeBC();
        ofem::BeamNodeBC* femNodeBC = (ofem::BeamNodeBC*)nodeBCSet->getBC(i);
        ivfNodeBC->setBeamModel(this);
        // ivfNodeBC->setColorTable(this->getColorTable());
        // ivfNodeBC->setNodeSize(*m_pnodeSize);
        ivfNodeBC->setNodeBC(femNodeBC);
        femNodeBC->setUser((void*)ivfNodeBC);
        ivfNodeBC->refresh();
        m_scene->addChild(static_cast<ivf::Shape*>(ivfNodeBC));
    }
}

void vfem::BeamModel::enumerate()
{
    this->getNodeSet()->enumerateNodes();
    this->getElementSet()->enumerateElements();

    ofem::NodeSet* nodeSet = this->getNodeSet();

    for (int i = 0; i < nodeSet->getSize(); i++)
    {
        nodeSet->getNode(i)->setNumber(i + 1);
    }
}

void BeamModel::setScene(Composite* scene)
{
    m_scene = scene;
}

Composite* BeamModel::getScene()
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

void BeamModel::setNodeMaterial(ivf::Material* material)
{
    m_nodeMaterial = material;
}

void BeamModel::setBeamMaterial(ivf::Material* material)
{
    m_beamMaterial = material;
}

ColorTable* BeamModel::getColorTable()
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

ivf::Material* BeamModel::getNodeMaterial()
{
    return m_nodeMaterial;
}

double BeamModel::getLineRadius()
{
    return m_lineRadius;
}

void BeamModel::setColorMaps(CColorMap* pos, CColorMap* neg, CColorMap* std)
{
    m_colorMapPos = pos;
    m_colorMapNeg = neg;
    m_colorMapStd = std;
}

void BeamModel::setResultInfo(CResultInfo* resultInfo)
{
    m_resultInfo = resultInfo;
}

void BeamModel::setBeamType(int type)
{
    m_beamType = type;
}

int BeamModel::getBeamType()
{
    return m_beamType;
}

void vfem::BeamModel::setTextFont(ivf::BitmapFont* font)
{
    m_textFont = font;
}

ivf::BitmapFont* vfem::BeamModel::textFont()
{
    return m_textFont;
}

void vfem::BeamModel::setShowNodeNumbers(bool flag)
{
    m_showNodeNumbers = flag;
}

bool vfem::BeamModel::showNodeNumbers()
{
    return m_showNodeNumbers;
}

void vfem::BeamModel::setCamera(ivf::Camera* camera)
{
    m_camera = camera;
}

ivf::Camera* vfem::BeamModel::camera()
{
    return m_camera;
}

ofem::BeamNodeBC* BeamModel::defaultNodePosBC()
{
    return m_defaultNodePosBC;
}

ofem::BeamNodeBC* BeamModel::defaultNodeFixedBC()
{
    return m_defaultNodeFixedBC;
}

void BeamModel::setResultType(int type)
{
    m_resultType = type;
}

int BeamModel::getResultType()
{
    return m_resultType;
}

CResultInfo* BeamModel::getResultInfo()
{
    return m_resultInfo;
}

CColorMap* BeamModel::getColorMapPos()
{
    return m_colorMapPos;
}

CColorMap* BeamModel::getColorMapNeg()
{
    return m_colorMapNeg;
}

CColorMap* BeamModel::getColorMapStd()
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

double BeamModel::getBeamLoadSize()
{
    return m_beamLoadSize;
}

double BeamModel::getLoadSize()
{
    return m_loadSize;
}

void BeamModel::setPath(const std::string& path)
{
    m_colorMapPath = path;
    m_colorMapPos->setPath(m_colorMapPath);
    m_colorMapNeg->setPath(m_colorMapPath);
    m_colorMapStd->setPath(m_colorMapPath);
}
