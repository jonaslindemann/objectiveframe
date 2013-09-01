#include "StructureFactory.h"

#include <ivf/ivfconfig.h>

CStructureFactory::CStructureFactory()
{
    m_size[0] = 3;
    m_size[1] = 4;
    m_size[2] = 4;
    m_spacing[0] = 2.0;
    m_spacing[1] = 2.0;
    m_spacing[2] = 2.4;
    m_beamModel = NULL;
    m_currentMaterial = NULL;
}

CStructureFactory::~CStructureFactory()
{

}

void CStructureFactory::setSize(int rows, int cols, int stacks)
{
    m_size[0] = cols;
    m_size[1] = rows;
    m_size[2] = stacks;
}

void CStructureFactory::setSpacing(double xSpacing, double ySpacing, double zSpacing)
{
    m_spacing[0] = xSpacing;
    m_spacing[1] = ySpacing;
    m_spacing[2] = zSpacing;
}

void CStructureFactory::getSize(int &rows, int &cols, int &stacks)
{
    rows = m_size[0];
    cols = m_size[1];
    stacks = m_size[2];
}

void CStructureFactory::getSpacing(double &xSpacing, double &ySpacing, double &zSpacing)
{
    xSpacing = m_spacing[0];
    ySpacing = m_spacing[1];
    zSpacing = m_spacing[2];
}

void CStructureFactory::create()
{
    if (m_beamModel!=NULL)
    {
        //
        // Create nodes
        //
        
        m_currentMaterial = (CFemBeamMaterial*)m_beamModel->getMaterialSet()->currentMaterial();

        vector<CIvfFemNode*> nodes;

        int i, j, k;

        for (k=0; k<m_size[2]; k++)
        {
            for (i=0; i<m_size[1]; i++)
            {
                for (j=0; j<m_size[0]; j++)
                {
                    //
                    // Create a fem node
                    //

                    CFemNode* femNode = new CFemNode();

                    //
                    // Add it to the Fem model
                    //

                    m_beamModel->getNodeSet()->addNode(femNode);
                    femNode->setNumber(m_beamModel->getNodeSet()->getSize()-1);

                    //
                    // Create Ivf representation
                    //

                    double x = j*m_spacing[0] - m_spacing[0]*((double)m_size[0]-1.0)/2.0;
                    double z = i*m_spacing[2] - m_spacing[2]*((double)m_size[1]-1.0)/2.0;
                    double y = k*m_spacing[1];

                    CIvfFemNode* ivfNode = new CIvfFemNode();
                    ivfNode->setBeamModel(m_beamModel);
                    ivfNode->setFemNode(femNode);
                    ivfNode->setPosition(x, y, z);
                    //ivfNode->setNodeSize(*m_beamModel->getNodeSize());
                    //ivfNode->setNodeType(*m_beamModel->getNodeType());
                    ivfNode->setMaterial(m_beamModel->getNodeMaterial());
                    ivfNode->setDirectRefresh(true);

                    //
                    // Add to scene
                    //

                    m_beamModel->getScene()->addChild(ivfNode);
                    nodes.push_back(ivfNode);
                }
            }
        }

        //
        // Create risers
        //

        for (i=0; i<m_size[1]; i++)
        {
            for (j=0; j<m_size[0]; j++)
            {
                for (k=0; k<m_size[2]-1; k++)
                {
                    //
                    // Create visual representation
                    //

                    CIvfFemBeam* ivfBeam = new CIvfFemBeam();
                    ivfBeam->setBeamModel(m_beamModel);

                    //
                    // Create model representation
                    //

                    CFemBeam* femBeam =  new CFemBeam();

                    //
                    // Extract FemNode:s from the IvfNodes
                    //

                    CIvfFemNode* ivfNode1 =
                        (CIvfFemNode*) nodes[j+i*m_size[0]+m_size[0]*m_size[1]*k];
                    CIvfFemNode* ivfNode2 =
                        (CIvfFemNode*) nodes[j+i*m_size[0]+m_size[0]*m_size[1]*(k+1)];

                    //
                    // Add FemNodes to beam element
                    //

                    femBeam->addNode(ivfNode1->getFemNode());
                    femBeam->addNode(ivfNode2->getFemNode());

                    //
                    // Set the material
                    //

                    femBeam->setMaterial(m_currentMaterial);

                    //
                    // Add beam element to beam model
                    //

                    m_beamModel->getElementSet()->addElement(femBeam);

                    //
                    // Initialize the representation
                    //

                    ivfBeam->setBeam(femBeam);
                    ivfBeam->setNodes(ivfNode1, ivfNode2);
                    //ivfBeam->setBeamType(IVF_BEAM_SOLID);
                    //ivfBeam->setSolidRadius(*m_beamModel->getLineRadius());
                    //ivfBeam->setColorTable(m_beamModel->getColorTable());
                    ivfBeam->refresh();

                    //
                    // Add to scene
                    //

                    m_beamModel->getScene()->addChild(ivfBeam);
                }
            }
        }

        //
        // Create floors z direction
        //

        for (k=1; k<m_size[2]; k++)
        {
            for (j=0; j<m_size[0]; j++)
            {
                for (i=0; i<m_size[1]-1; i++)
                {
                    //
                    // Create visual representation
                    //

                    CIvfFemBeam* ivfBeam = new CIvfFemBeam();
                    ivfBeam->setBeamModel(m_beamModel);

                    //
                    // Create model representation
                    //

                    CFemBeam* femBeam =  new CFemBeam();

                    //
                    // Extract FemNode:s from the IvfNodes
                    //

                    CIvfFemNode* ivfNode1 =
                        (CIvfFemNode*) nodes[j+i*m_size[0]+m_size[0]*m_size[1]*k];
                    CIvfFemNode* ivfNode2 =
                        (CIvfFemNode*) nodes[j+(i+1)*m_size[0]+m_size[0]*m_size[1]*k];

                    //
                    // Add FemNodes to beam element
                    //

                    femBeam->addNode(ivfNode1->getFemNode());
                    femBeam->addNode(ivfNode2->getFemNode());

                    //
                    // Set the material
                    //

                    femBeam->setMaterial(m_currentMaterial);

                    //
                    // Add beam element to beam model
                    //

                    m_beamModel->getElementSet()->addElement(femBeam);

                    //
                    // Initialize the representation
                    //

                    ivfBeam->setBeam(femBeam);
                    ivfBeam->setNodes(ivfNode1, ivfNode2);
                    //ivfBeam->setBeamType(IVF_BEAM_SOLID);
                    //ivfBeam->setSolidRadius(*m_beamModel->getLineRadius());
                    //ivfBeam->setColorTable(m_beamModel->getColorTable());
                    ivfBeam->refresh();

                    //
                    // Add to scene
                    //

                    m_beamModel->getScene()->addChild(ivfBeam);
                }
            }
        }

        //
        // Create floors x direction
        //

        for (k=1; k<m_size[2]; k++)
        {
            for (i=0; i<m_size[1]; i++)
            {
                for (j=0; j<m_size[0]-1; j++)
                {
                    //
                    // Create visual representation
                    //

                    CIvfFemBeam* ivfBeam = new CIvfFemBeam();
                    ivfBeam->setBeamModel(m_beamModel);

                    //
                    // Create model representation
                    //

                    CFemBeam* femBeam =  new CFemBeam();

                    //
                    // Extract FemNode:s from the IvfNodes
                    //

                    CIvfFemNode* ivfNode1 =
                        (CIvfFemNode*) nodes[j+i*m_size[0]+m_size[0]*m_size[1]*k];
                    CIvfFemNode* ivfNode2 =
                        (CIvfFemNode*) nodes[j+1+i*m_size[0]+m_size[0]*m_size[1]*k];

                    //
                    // Add FemNodes to beam element
                    //

                    femBeam->addNode(ivfNode1->getFemNode());
                    femBeam->addNode(ivfNode2->getFemNode());

                    //
                    // Set the material
                    //

                    femBeam->setMaterial(m_currentMaterial);

                    //
                    // Add beam element to beam model
                    //

                    m_beamModel->getElementSet()->addElement(femBeam);

                    //
                    // Initialize the representation
                    //

                    ivfBeam->setBeam(femBeam);
                    ivfBeam->setNodes(ivfNode1, ivfNode2);
                    //ivfBeam->setBeamType(IVF_BEAM_SOLID);
                    //ivfBeam->setSolidRadius(*m_beamModel->getLineRadius());
                    //ivfBeam->setColorTable(m_beamModel->getColorTable());
                    ivfBeam->refresh();

                    //
                    // Add to scene
                    //

                    m_beamModel->getScene()->addChild(ivfBeam);
                }
            }
        }
    }
}

void CStructureFactory::setBeamModel(CIvfBeamModel *model)
{
    m_beamModel = model;
}

CIvfBeamModel* CStructureFactory::getBeamModel()
{
    return m_beamModel;
}

void CStructureFactory::setCurrentMaterial(CFemBeamMaterial *material)
{
    m_currentMaterial = material;
}
