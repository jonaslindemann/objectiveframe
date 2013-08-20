// Implementation of: public class CIvfBeamModel

#include "IvfBeamModel.h"

#include "IvfFemNode.h"
#include "IvfFemBeam.h"
#include "IvfFemBeamLoad.h"
#include "IvfFemNodeLoad.h"
#include "IvfFemNodeBC.h"
#include <FemNodeSet.h>
#include <FemBeamSet.h>
#include <FemBeamLoad.h>
#include <FemElementLoadSet.h>
#include <FemNodeBCSet.h>

#include <ColorMap.h>
#include <ResultInfo.h>


// ------------------------------------------------------------
CIvfBeamModel::CIvfBeamModel ()
    :CFemBeamModel()
{
    m_nodeSize = 1.0;
    m_nodeType = CIvfNode::NT_CUBE;
    m_lineRadius = 1.0;
    m_loadSize = 1.0;
    m_beamLoadSize = 1.0;

    m_colorTable = new CIvfColorTable();

    m_colorMapPos = new CColorMap();
    m_colorMapNeg = new CColorMap();
    m_colorMapStd = new CColorMap();

    m_resultInfo = new CResultInfo();

    m_beamType = IVF_BEAM_SOLID;
    m_resultType = IVF_BEAM_NO_RESULT;
    m_nodeType = CIvfNode::NT_CUBE;

    m_colorMapPath = "";
}

// ------------------------------------------------------------
CIvfBeamModel::~CIvfBeamModel ()
{
    delete m_colorMapPos;
    delete m_colorMapNeg;
    delete m_colorMapStd;
    delete m_resultInfo;
}

// ------------------------------------------------------------
void CIvfBeamModel::generateModel()
{

    // Open color maps

    m_colorMapPos->open("red.map");
    m_colorMapNeg->open("blue.map");
    m_colorMapStd->open("colormap11.map");

    // Temporary lists

    vector<CIvfFemNode*> ivfNodes;

    // Create nodes

    CFemNodeSet* nodeSet = this->getNodeSet();

    for (int i=0; i<nodeSet->getSize(); i++)
    {
        CIvfFemNode* ivfNode = new CIvfFemNode();
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
        CIvfFemBeam* ivfBeam = new CIvfFemBeam();
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
        CIvfFemBeamLoad* ivfBeamLoad = new CIvfFemBeamLoad();
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
        CIvfFemNodeLoad* ivfNodeLoad = new CIvfFemNodeLoad();
        CFemBeamNodeLoad* femNodeLoad = (CFemBeamNodeLoad*) nodeLoadSet->getLoad(i);
        ivfNodeLoad->setBeamModel(this);
        ivfNodeLoad->setNodeLoad(femNodeLoad);
        femNodeLoad->setUser((void*)ivfNodeLoad);
        //ivfNodeLoad->setColorTable(this->getColorTable());
        //ivfNodeLoad->setLoadHeight(*m_ploadSize);
        ivfNodeLoad->refresh();
        m_scene->addChild(ivfNodeLoad);
    }

    // Generate node bcs

    CFemNodeBCSet* nodeBCSet = this->getNodeBCSet();

    for (int i=0; i<nodeBCSet->getSize(); i++)
    {
        CIvfFemNodeBC* ivfNodeBC = new CIvfFemNodeBC();
        CFemBeamNodeBC* femNodeBC = (CFemBeamNodeBC*) nodeBCSet->getBC(i);
        ivfNodeBC->setBeamModel(this);
        //ivfNodeBC->setColorTable(this->getColorTable());
        //ivfNodeBC->setNodeSize(*m_pnodeSize);
        ivfNodeBC->setNodeBC(femNodeBC);
        femNodeBC->setUser((void*)ivfNodeBC);
        ivfNodeBC->refresh();
        m_scene->addChild(ivfNodeBC);
    }
}

// ------------------------------------------------------------
void CIvfBeamModel::setScene(CIvfComposite *scene)
{
    m_scene = scene;
}

// ------------------------------------------------------------
CIvfComposite* CIvfBeamModel::getScene()
{
    return m_scene;
}

void CIvfBeamModel::setNodeSize(double size)
{
    m_nodeSize = size;
}

void CIvfBeamModel::setNodeType(int type)
{
    m_nodeType = type;
}

void CIvfBeamModel::setLineRadius(double radius)
{
    m_lineRadius = radius;
}

void CIvfBeamModel::setLineSides(int sides)
{
    m_lineSides = sides;
}

void CIvfBeamModel::setNodeMaterial(CIvfMaterial *material)
{
    m_nodeMaterial = material;
}

void CIvfBeamModel::setBeamMaterial(CIvfMaterial *material)
{
    m_beamMaterial = material;
}

CIvfColorTable* CIvfBeamModel::getColorTable()
{
    return m_colorTable;
}

void CIvfBeamModel::setLoadSize(double size)
{
    m_loadSize = size;
}

void CIvfBeamModel::setBeamLoadSize(double size)
{
    m_beamLoadSize = size;
}


double CIvfBeamModel::getNodeSize()
{
    return m_nodeSize;
}

int CIvfBeamModel::getNodeType()
{
    return m_nodeType;
}

CIvfMaterial* CIvfBeamModel::getNodeMaterial()
{
    return m_nodeMaterial;
}

double CIvfBeamModel::getLineRadius()
{
    return m_lineRadius;
}

void CIvfBeamModel::setColorMaps(CColorMap *pos, CColorMap *neg, CColorMap *std)
{
    m_colorMapPos = pos;
    m_colorMapNeg = neg;
    m_colorMapStd = std;
}

void CIvfBeamModel::setResultInfo(CResultInfo *resultInfo)
{
    m_resultInfo = resultInfo;
}

void CIvfBeamModel::setBeamType(int type)
{
    m_beamType = type;
}

int CIvfBeamModel::getBeamType()
{
    return m_beamType;
}

void CIvfBeamModel::setResultType(int type)
{
    m_resultType = type;
}

int CIvfBeamModel::getResultType()
{
    return m_resultType;
}

CResultInfo* CIvfBeamModel::getResultInfo()
{
    return m_resultInfo;
}

CColorMap* CIvfBeamModel::getColorMapPos()
{
    return m_colorMapPos;
}

CColorMap* CIvfBeamModel::getColorMapNeg()
{
    return m_colorMapNeg;
}

CColorMap* CIvfBeamModel::getColorMapStd()
{
    return m_colorMapStd;
}

void CIvfBeamModel::setScaleFactor(double factor)
{
    m_scaleFactor = factor;
}

double CIvfBeamModel::getScaleFactor()
{
    return m_scaleFactor;
}

double CIvfBeamModel::getBeamLoadSize()
{
    return m_beamLoadSize;
}

double CIvfBeamModel::getLoadSize()
{
    return m_loadSize;
}

void CIvfBeamModel::setPath(const std::string& path)
{
    m_colorMapPath = path;
    m_colorMapPos->setPath(m_colorMapPath);
    m_colorMapNeg->setPath(m_colorMapPath);
    m_colorMapStd->setPath(m_colorMapPath);
}
