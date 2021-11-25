// Implementation of: public class BeamModel

#include "VisBeamModel.h"

#include "VisFemNode.h"
#include "VisFemBeam.h"
#include "VisFemBeamLoad.h"
#include "VisFemNodeLoad.h"
#include "VisFemNodeBC.h"
#include <FemNodeSet.h>
#include <FemBeamSet.h>
#include <FemBeamLoad.h>
#include <FemElementLoadSet.h>
#include <FemNodeBCSet.h>
#include <FemSolidPipeSection.h>

#include <ColorMap.h>
#include <ResultInfo.h>

using namespace ivf;

// ------------------------------------------------------------
VisBeamModel::VisBeamModel()
    :CFemBeamModel()
{
    m_nodeSize = 1.0;
    m_nodeType = Node::NT_CUBE;
    m_lineRadius = 1.0;
    m_loadSize = 1.0;
    m_beamLoadSize = 1.0;

    m_colorTable = new ColorTable();

    m_colorMapPos = new CColorMap();
    m_colorMapNeg = new CColorMap();
    m_colorMapStd = new CColorMap();

    m_resultInfo = new CResultInfo();

    m_beamType = IVF_BEAM_SOLID;
    m_resultType = IVF_BEAM_NO_RESULT;
    m_nodeType = Node::NT_CUBE;

    m_colorMapPath = "";
}

// ------------------------------------------------------------
VisBeamModel::~VisBeamModel()
{
    delete m_colorMapPos;
    delete m_colorMapNeg;
    delete m_colorMapStd;
    delete m_resultInfo;
}

// ------------------------------------------------------------
void VisBeamModel::onInitialised()
{
    CFemBeamMaterialPtr material = new CFemBeamMaterial();
    CFemSolidPipeSectionPtr section = new CFemSolidPipeSection(0.05);
    material->setProperties(2.1e9, 8.1e7, 1.0, 1.0, 1.0, 1.0 );
    material->setSection(section);
    material->setName("default");
    material->setColor(52);
    this->materialSet()->addMaterial(material);
    this->materialSet()->setCurrentMaterial(0);

	// Create default Node BCs

	m_defaultNodeFixedBC = new CFemBeamNodeBC();
	m_defaultNodeFixedBC->setName("fixed pos/rot");
	m_defaultNodeFixedBC->fixed();
	m_defaultNodeFixedBC->setReadOnly();
	this->nodeBCSet()->addBC(m_defaultNodeFixedBC);

	m_defaultNodePosBC = new CFemBeamNodeBC();
	m_defaultNodePosBC->setName("fixed pos");
	m_defaultNodePosBC->fixedPosition();
	m_defaultNodePosBC->setReadOnly();
	this->nodeBCSet()->addBC(m_defaultNodePosBC);
}

// ------------------------------------------------------------
void VisBeamModel::generateModel()
{

    // Open color maps

    m_colorMapPos->open("red.map");
    m_colorMapNeg->open("blue.map");
    m_colorMapStd->open("colormap11.map");

    // Temporary lists

    vector<VisFemNode*> ivfNodes;

    // Create nodes

    CFemNodeSet* nodeSet = this->getNodeSet();

    for (int i=0; i<nodeSet->getSize(); i++)
    {
        VisFemNode* ivfNode = new VisFemNode();
        ivfNode->setFemNode(nodeSet->getNode(i));
        ivfNode->setMaterial(m_nodeMaterial);
        ivfNode->setBeamModel(this);
        ivfNode->setDirectRefresh(true);
        ivfNode->refresh();
        //if (m_pnodeSize!=NULL)
        //	ivfNode->setNodeSize(*m_pnodeSize);
        m_scene->addChild(ivfNode);
        ivfNodes.push_back(ivfNode);
        nodeSet->getNode(i)->setNumber(i+1);
    }

    // Generate elements

    CFemBeamSet* beamSet = this->getElementSet();

    for (int i=0; i<beamSet->getSize(); i++)
    {
        VisFemBeam* ivfBeam = new VisFemBeam();
        CFemBeam* femBeam = (CFemBeam*) beamSet->getElement(i);
        ivfBeam->setBeam(femBeam);
        //ivfBeam->setMaterial(m_beamMaterial);
        ivfBeam->setBeamModel(this);
        CFemNode* femNode1 = femBeam->getNode(0);
        CFemNode* femNode2 = femBeam->getNode(1);
        ivfBeam->setNodes(ivfNodes[femNode1->getNumber()-1],ivfNodes[femNode2->getNumber()-1]);
        ivfBeam->refresh();
        m_scene->addChild(ivfBeam);
    }

    // Generate beam loads

    CFemElementLoadSet* elementLoadSet = this->getElementLoadSet();

    for (int i=0; i<elementLoadSet->getSize(); i++)
    {
        VisFemBeamLoad* ivfBeamLoad = new VisFemBeamLoad();
        CFemBeamLoad* femBeamLoad = (CFemBeamLoad*) elementLoadSet->getLoad(i);
        ivfBeamLoad->setBeamModel(this);
        ivfBeamLoad->setBeamLoad(femBeamLoad);
        femBeamLoad->setUser((void*)ivfBeamLoad);
        //ivfBeamLoad->setLoadHeight(*m_pbeamLoadSize);
        //ivfBeamLoad->setColorTable(this->getColorTable());
        ivfBeamLoad->refresh();
        m_scene->addChild(ivfBeamLoad);
    }

    // Generate node loads

    CFemNodeLoadSet* nodeLoadSet = this->getNodeLoadSet();

    for (int i=0; i<nodeLoadSet->getSize(); i++)
    {
        VisFemNodeLoad* ivfNodeLoad = new VisFemNodeLoad();
        CFemBeamNodeLoad* femNodeLoad = (CFemBeamNodeLoad*) nodeLoadSet->getLoad(i);
        ivfNodeLoad->setBeamModel(this);
        ivfNodeLoad->setNodeLoad(femNodeLoad);
        femNodeLoad->setUser((void*)ivfNodeLoad);
        //ivfNodeLoad->setColorTable(this->getColorTable());
        //ivfNodeLoad->setLoadHeight(*m_ploadSize);
        ivfNodeLoad->refresh();
        m_scene->addChild(static_cast<ivf::Shape*>(ivfNodeLoad));
    }

    // Generate node bcs

    CFemNodeBCSet* nodeBCSet = this->getNodeBCSet();

    for (int i=0; i<nodeBCSet->getSize(); i++)
    {
        VisFemNodeBC* ivfNodeBC = new VisFemNodeBC();
        CFemBeamNodeBC* femNodeBC = (CFemBeamNodeBC*) nodeBCSet->getBC(i);
        ivfNodeBC->setBeamModel(this);
        //ivfNodeBC->setColorTable(this->getColorTable());
        //ivfNodeBC->setNodeSize(*m_pnodeSize);
        ivfNodeBC->setNodeBC(femNodeBC);
        femNodeBC->setUser((void*)ivfNodeBC);
        ivfNodeBC->refresh();
        m_scene->addChild(static_cast<ivf::Shape*>(ivfNodeBC));
    }
}

// ------------------------------------------------------------
void VisBeamModel::setScene(Composite *scene)
{
    m_scene = scene;
}

// ------------------------------------------------------------
Composite* VisBeamModel::getScene()
{
    return m_scene;
}

void VisBeamModel::setNodeSize(double size)
{
    m_nodeSize = size;
}

void VisBeamModel::setNodeType(int type)
{
    m_nodeType = type;
}

void VisBeamModel::setLineRadius(double radius)
{
    m_lineRadius = radius;
}

void VisBeamModel::setLineSides(int sides)
{
    m_lineSides = sides;
}

void VisBeamModel::setNodeMaterial(Material *material)
{
    m_nodeMaterial = material;
}

void VisBeamModel::setBeamMaterial(Material *material)
{
    m_beamMaterial = material;
}

ColorTable* VisBeamModel::getColorTable()
{
    return m_colorTable;
}

void VisBeamModel::setLoadSize(double size)
{
    m_loadSize = size;
}

void VisBeamModel::setBeamLoadSize(double size)
{
    m_beamLoadSize = size;
}


double VisBeamModel::getNodeSize()
{
    return m_nodeSize;
}

int VisBeamModel::getNodeType()
{
    return m_nodeType;
}

Material* VisBeamModel::getNodeMaterial()
{
    return m_nodeMaterial;
}

double VisBeamModel::getLineRadius()
{
    return m_lineRadius;
}

void VisBeamModel::setColorMaps(CColorMap *pos, CColorMap *neg, CColorMap *std)
{
    m_colorMapPos = pos;
    m_colorMapNeg = neg;
    m_colorMapStd = std;
}

void VisBeamModel::setResultInfo(CResultInfo *resultInfo)
{
    m_resultInfo = resultInfo;
}

void VisBeamModel::setBeamType(int type)
{
    m_beamType = type;
}

int VisBeamModel::getBeamType()
{
    return m_beamType;
}

CFemBeamNodeBC * VisBeamModel::defaultNodePosBC()
{
	return m_defaultNodePosBC;
}

CFemBeamNodeBC * VisBeamModel::defaultNodeFixedBC()
{
	return m_defaultNodeFixedBC;
}

void VisBeamModel::setResultType(int type)
{
    m_resultType = type;
}

int VisBeamModel::getResultType()
{
    return m_resultType;
}

CResultInfo* VisBeamModel::getResultInfo()
{
    return m_resultInfo;
}

CColorMap* VisBeamModel::getColorMapPos()
{
    return m_colorMapPos;
}

CColorMap* VisBeamModel::getColorMapNeg()
{
    return m_colorMapNeg;
}

CColorMap* VisBeamModel::getColorMapStd()
{
    return m_colorMapStd;
}

void VisBeamModel::setScaleFactor(double factor)
{
    m_scaleFactor = factor;
}

double VisBeamModel::getScaleFactor()
{
    return m_scaleFactor;
}

double VisBeamModel::getBeamLoadSize()
{
    return m_beamLoadSize;
}

double VisBeamModel::getLoadSize()
{
    return m_loadSize;
}

void VisBeamModel::setPath(const std::string& path)
{
    m_colorMapPath = path;
    m_colorMapPos->setPath(m_colorMapPath);
    m_colorMapNeg->setPath(m_colorMapPath);
    m_colorMapStd->setPath(m_colorMapPath);
}
