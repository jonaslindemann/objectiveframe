// Implementation of: public class CIvfFemBeam

#include "IvfFemBeam.h"

#include <ivfmath/IvfVec3d.h>

// ------------------------------------------------------------
CIvfFemBeam::CIvfFemBeam ()
    :CIvfComposite()
{
    m_femBeam = NULL;

    m_nodes[0] = NULL;
    m_nodes[1] = NULL;

    m_beamModel = NULL;

    m_beamMaterial = new CIvfMaterial();
    m_beamMaterial->setDiffuseColor(0.0f, 0.5f, 0.75f, 1.0f);
    m_beamMaterial->setSpecularColor(1.0f, 1.0f, 1.0f,1.0f);

    // Set up the solid line

    m_solidLine = new CIvfSolidLine();
    m_solidLine->setMaterial(m_beamMaterial);
    m_solidLine->setUseName(false);
    m_solidLine->setUseSelectShape(false);
    this->addChild(m_solidLine);

    // Set up line set

    m_lineSet = new CIvfSimpleLineSet();
    m_lineSet->setCoordSize(2);
    m_lineSet->setLineSize(3);
    m_lineSet->setColorSize(2);
    m_lineSet->setLineIndex(0, 0);
    m_lineSet->setLineIndex(1, 1);
    m_lineSet->setLineIndex(2, -1);
    m_lineSet->setColor(0, 1.0, 1.0, 0.0);
    m_lineSet->setColor(1, 1.0, 1.0, 0.0);
    m_lineSet->setState(CIvfShape::OS_OFF);
    m_lineSet->setUseName(false);
    m_lineSet->setUseSelectShape(false);
    this->addChild(m_lineSet);

    // Set up extrusion

    m_extrusion = new CIvfExtrusion();
    m_extrusion->setState(CIvfShape::OS_OFF);
    m_extrusion->setMaterial(m_beamMaterial);
    m_extrusion->setUseName(false);
    m_extrusion->setUseSelectShape(false);
    this->addChild(m_extrusion);

    // Create texture for visualising section forces

    m_beamImage = new CIvfImage();
    m_beamImage->setSize(8,8);

    int i;

    for (i=0; i<8; i++)
    {
        /*
        m_beamImage->setPixel(0,i,255,255,255);
        m_beamImage->setPixel(1,i,0,0,0);
        m_beamImage->setPixel(2,i,255,255,255);
        m_beamImage->setPixel(3,i,0,0,0);
        m_beamImage->setPixel(4,i,255,255,255);
        m_beamImage->setPixel(5,i,0,0,0);
        m_beamImage->setPixel(6,i,255,255,255);
        m_beamImage->setPixel(7,i,0,0,0);
        */

        m_beamImage->setPixel(i,0,255,255,255);
        m_beamImage->setPixel(i,1,0,0,0);
        m_beamImage->setPixel(i,2,255,255,255);
        m_beamImage->setPixel(i,3,0,0,0);
        m_beamImage->setPixel(i,4,255,255,255);
        m_beamImage->setPixel(i,5,0,0,0);
        m_beamImage->setPixel(i,6,255,255,255);
        m_beamImage->setPixel(i,7,0,0,0);
    }

    m_beamTexture = new CIvfTexture();
    m_beamTexture->setImage(m_beamImage);
    m_beamTexture->setTextureMode(GL_MODULATE);
    m_beamTexture->setFilters(GL_LINEAR, GL_LINEAR);
    m_beamTexture->deactivate();

    m_solidLine->setTexture(m_beamTexture);
    m_solidLine->setTextureMode(GLE_TEXTURE_ENABLE | GLE_TEXTURE_NORMAL_CYL);
    //m_solidLine->setTextureMode(GLE_TEXTURE_ENABLE | GLE_TEXTURE_VERTEX_MODEL_CYL);


    // Create a line indicating z-axis

    this->setUseName(true);
    this->setUseSelectShape(true);
}

// ------------------------------------------------------------
CIvfFemBeam::~CIvfFemBeam ()
{
    for (int i=0; i<2; i++)
    {
        if (m_nodes[i]!=NULL)
        {
            m_nodes[i]->deleteReference();
            if (!m_nodes[i]->referenced())
                delete m_nodes[i];
        }
    }
}

// ------------------------------------------------------------
void CIvfFemBeam::setBeam(CFemBeam *beam)
{
    m_femBeam = beam;
}

// ------------------------------------------------------------
CFemBeam* CIvfFemBeam::getBeam()
{
    return m_femBeam;
}

// ------------------------------------------------------------
void CIvfFemBeam::refresh()
{
    if (m_femBeam!=NULL)
    {
        double x1, y1, z1;
        double x2, y2, z2;
        CFemNode* node1 = m_nodes[0]->getFemNode();
        CFemNode* node2 = m_nodes[1]->getFemNode();
        node1->getCoord(x1, y1, z1);
        node2->getCoord(x2, y2, z2);

        m_lineSet->setState(CIvfShape::OS_OFF);
        m_solidLine->setState(CIvfShape::OS_OFF);
        m_extrusion->setState(CIvfShape::OS_OFF);

        if (m_beamModel!=NULL)
        {

            if (m_beamModel->getColorTable()==NULL)
            {
                if (m_femBeam->getMaterial()!=NULL)
                {
                    m_beamMaterial->setDiffuseColor(1.0f, 1.0f, 0.0f, 1.0f);
                    m_beamMaterial->setSpecularColor(1.0f, 1.0f, 1.0f,1.0f);
                }
                else
                {
                    m_beamMaterial->setDiffuseColor(0.5f, 0.5f, 0.5f, 1.0f);
                    m_beamMaterial->setAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
                }
            }
            else
            {
                if (m_femBeam->getMaterial()!=NULL)
                {
                    m_beamModel->getColorTable()->assignColor(
                        m_femBeam->getMaterial()->getColor(), m_beamMaterial);
                }
                else
                {
                    m_beamMaterial->setDiffuseColor(0.5f, 0.5f, 0.5f, 1.0f);
                    m_beamMaterial->setAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
                }
            }

            m_lineSet->setState(CIvfShape::OS_OFF);
            m_extrusion->setState(CIvfShape::OS_OFF);
            m_solidLine->setState(CIvfShape::OS_OFF);
            initExtrusion();

            switch (m_beamModel->getBeamType()) {
            case IVF_BEAM_LINESET:
                m_lineSet->setCoord(0, x1, y1, z1);
                m_lineSet->setCoord(1, x2, y2, z2);
                m_lineSet->setState(CIvfShape::OS_ON);
                break;
            case IVF_BEAM_SOLID:
                if (m_beamModel!=NULL)
                    m_solidLine->setRadius(m_beamModel->getLineRadius());
                m_solidLine->setNodes(m_nodes[0],m_nodes[1]);
                m_solidLine->setState(CIvfShape::OS_ON);
                m_beamTexture->deactivate();
                m_beamTexture->setTextureModifier(1.0, 1.0/m_solidLine->getLength(), 0.0);
                m_solidLine->setTextureMode(0);
                break;
            case IVF_BEAM_RESULTS:
                if (m_beamModel!=NULL)
                {
                    if (m_beamModel->getResultType()!=IVF_BEAM_NO_RESULT)
                    {
                        m_solidLine->setRadius(m_beamModel->getLineRadius());
                        m_solidLine->setNodes(m_nodes[0],m_nodes[1]);
                        m_solidLine->setState(CIvfShape::OS_ON);
                        m_beamTexture->activate();
                        m_beamTexture->setTextureModifier(1.0, 1.0/m_solidLine->getLength(), 0.0);
                        m_beamMaterial->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
                        m_beamMaterial->setAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
                        m_solidLine->setTextureMode(GLE_TEXTURE_ENABLE | GLE_TEXTURE_VERTEX_MODEL_CYL);
                        //m_solidLine->setTextureMode(GLE_TEXTURE_ENABLE | GLE_TEXTURE_NORMAL_FLAT);
                        initResults();
                    } else {
                        m_solidLine->setRadius(m_beamModel->getLineRadius());
                        m_solidLine->setNodes(m_nodes[0],m_nodes[1]);
                        m_solidLine->setState(CIvfShape::OS_ON);
                        m_beamTexture->deactivate();
                        m_beamTexture->setTextureModifier(1.0, 1.0/m_solidLine->getLength(), 0.0);
                        m_solidLine->setTextureMode(0);
                    }
                }
                break;
            case IVF_BEAM_EXTRUSION:
                if (m_femBeam!=NULL)
                {
                    if (m_femBeam->getMaterial()!=NULL)
                    {
                        m_extrusion->setState(CIvfShape::OS_ON);
                    }
                }
                break;
            default:

                break;
            }
        }
    }
}

// ------------------------------------------------------------
void CIvfFemBeam::doCreateGeometry()
{
    if (m_femBeam!=NULL)
    {
        CIvfComposite::doCreateGeometry();
    }
}

// ------------------------------------------------------------
void CIvfFemBeam::setNodes(CIvfFemNode *node1, CIvfFemNode *node2)
{
    for (int i=0; i<2; i++)
    {
        if (m_nodes[i]!=NULL)
        {
            m_nodes[i]->deleteReference();
            if (!m_nodes[i]->referenced())
                delete m_nodes[i];
        }
    }
    m_nodes[0] = node1;
    m_nodes[1] = node2;
    m_nodes[0]->addReference();
    m_nodes[1]->addReference();

    m_beamTexture->setTextureModifier(1.0, 1.0/m_solidLine->getLength(), 0.0);
}

// ------------------------------------------------------------
void CIvfFemBeam::doCreateSelect()
{
    if (m_beamModel!=NULL)
    {
        switch (m_beamModel->getBeamType()) {
        case IVF_BEAM_LINESET:

            break;
        case IVF_BEAM_SOLID:
        case IVF_BEAM_RESULTS:
            m_solidLine->setSelect(CIvfShape::SS_ON);
            m_solidLine->render();
            m_solidLine->setSelect(CIvfShape::SS_OFF);

            break;
        case IVF_BEAM_EXTRUSION:
            if (m_femBeam->getMaterial()!=NULL)
            {
                if (m_extrusion->getState()==CIvfShape::OS_ON)
                {
                    m_extrusion->setSelect(CIvfShape::SS_ON);
                    m_extrusion->render();
                    m_extrusion->setSelect(CIvfShape::SS_OFF);
                }
                else
                {
                    m_solidLine->setSelect(CIvfShape::SS_ON);
                    m_solidLine->render();
                    m_solidLine->setSelect(CIvfShape::SS_OFF);
                }
            }
            break;
        default:

            break;
        }
    }
}

// ------------------------------------------------------------
void CIvfFemBeam::initExtrusion()
{
    CIvfVec3d p1, p2;
    CIvfVec3d v1;
    double z, ex, ey, ez;

    // Create section

    m_extrusion->setUseTwist(true);

    if ( (m_femBeam!=NULL)&&
            (m_femBeam->getMaterial()!=NULL)&&
            (m_femBeam->getMaterial()->getSection()!=NULL) )
    {
        CFemSection* section = m_femBeam->getMaterial()->getSection();
        double x, y;
        m_extrusion->setSectionSize(section->getSize());
        for (unsigned int i=0; i<section->getSize(); i++)
        {
            section->getCoord(i, x, y);
            m_extrusion->setSectionCoord(i, x, y);
            if (i<section->getSize()-1)
            {
                section->getNormal(i, x, y);
                m_extrusion->setSectionNormal(i, x, y);
            }
        }
        // Set topolgy

        m_extrusion->setSpineSize(4);
        m_nodes[0]->getPosition(x,  y,  z);
        m_extrusion->setSpineCoord(1, x, y, z);
        m_nodes[1]->getPosition(x, y, z);
        m_extrusion->setSpineCoord(2, x, y, z);
        m_extrusion->setSpineTwist(1,m_femBeam->getBeamRotation());
        m_extrusion->setSpineTwist(2,m_femBeam->getBeamRotation());

        // Calculate start and endpoint

        m_extrusion->getSpineCoord(1, x, y, z);
        p1.setComponents(x, y, z);
        m_extrusion->getSpineCoord(2, x, y, z);
        p2.setComponents(x, y, z);

        v1.setFromPoints(p1, p2);
        v1.normalize();
        v1.getComponents(ex, ey, ez);

        if ( (ex==0)&&(ez==0)&&(ey!=0) )
            m_extrusion->setUpVector(1.0, 0.0, 0.0);
        else
            m_extrusion->setUpVector(0.0, 1.0, 0.0);

        // First point

        p1.getComponents(x, y, z);
        x = -0.1 * ex + x;
        y = -0.1 * ey + y;
        z = -0.1 * ez + z;
        m_extrusion->setSpineCoord(0, x, y, z);

        m_extrusion->getSpineCoord(m_extrusion->getSpineSize()-3, x, y, z);
        p1.setComponents(x, y, z);
        m_extrusion->getSpineCoord(m_extrusion->getSpineSize()-2, x, y, z);
        p2.setComponents(x, y, z);

        v1.setFromPoints(p1, p2);
        v1.normalize();
        v1.getComponents(ex, ey, ez);

        // First point

        p1.getComponents(x, y, z);
        x = 0.1 * ex + x;
        y = 0.1 * ey + y;
        z = 0.1 * ez + z;
        m_extrusion->setSpineCoord(m_extrusion->getSpineSize()-1, x, y, z);
    }
}

void CIvfFemBeam::initResults()
{
    if (m_beamModel!=NULL)
    {
        CColorMap* colorMapPos = m_beamModel->getColorMapPos();
        CColorMap* colorMapNeg = m_beamModel->getColorMapNeg();
        CColorMap* colorMapStd = m_beamModel->getColorMapStd();
        CResultInfo* resultInfo = m_beamModel->getResultInfo();

        if (m_femBeam!=NULL)
        {
            int n = m_femBeam->getEvaluationPoints();
            int k;
            float red, green, blue;
            GLubyte r, g, b;
            double value, v1, v2;
            double N, My, Mz;

            red = green = blue = 0.0;

            for (k=0; k<n; k++)
            {
                switch (m_beamModel->getResultType()) {
                case IVF_BEAM_N:
                    value = m_femBeam->getValue(0+6*k);
                    if (value>0)
                    {
                        value = fabs(value)/resultInfo->getMaxN();
                        colorMapPos->getColor(value, red, green, blue);
                    }
                    else
                    {
                        value = fabs(value)/resultInfo->getMinN();
                        colorMapNeg->getColor(fabs(value), red, green, blue);
                    }

                    break;
                case IVF_BEAM_T:
                    value = m_femBeam->getValue(3+6*k);
                    value = (fabs(value)-resultInfo->getMinT())/resultInfo->getMaxT();
                    break;
                case IVF_BEAM_V:
                    v1 = m_femBeam->getValue(1+6*k);
                    v2 = m_femBeam->getValue(2+6*k);
                    value = sqrt(pow(v1,2)+pow(v2,2));
                    value = (value-resultInfo->getMinV())/resultInfo->getMaxV();
                    break;
                case IVF_BEAM_M:
                    v1 = m_femBeam->getValue(4+6*k);
                    v2 = m_femBeam->getValue(5+6*k);
                    value = sqrt(pow(v1,2)+pow(v2,2));
                    value = (value-resultInfo->getMinM())/resultInfo->getMaxM();
                    break;
                case IVF_BEAM_NAVIER:
                    N = m_femBeam->getValue(0+6*k);
                    My = m_femBeam->getValue(4+6*k);
                    Mz = m_femBeam->getValue(5+6*k);
                    value = calcNavier(N, My, Mz);
                    value = (fabs(value)-resultInfo->getMinNavier())/resultInfo->getMaxNavier();
                    break;
                default:
                    value = 0.0;
                    break;
                }

                if (m_beamModel->getResultType()!=IVF_BEAM_N)
                    colorMapStd->getColor(value, red, green, blue);

                r = (GLubyte)(255.0f*red);
                g = (GLubyte)(255.0f*green);
                b = (GLubyte)(255.0f*blue);

                /*
                m_beamImage->setPixel(k, 0, r, g, b);
                m_beamImage->setPixel(k, 1, r, g, b);
                m_beamImage->setPixel(k, 2, r, g, b);
                m_beamImage->setPixel(k, 3, r, g, b);
                m_beamImage->setPixel(k, 4, r, g, b);
                m_beamImage->setPixel(k, 5, r, g, b);
                m_beamImage->setPixel(k, 6, r, g, b);
                m_beamImage->setPixel(k, 7, r, g, b);
                */
                m_beamImage->setPixel(0, k, r, g, b);
                m_beamImage->setPixel(1, k, r, g, b);
                m_beamImage->setPixel(2, k, r, g, b);
                m_beamImage->setPixel(3, k, r, g, b);
                m_beamImage->setPixel(4, k, r, g, b);
                m_beamImage->setPixel(5, k, r, g, b);
                m_beamImage->setPixel(6, k, r, g, b);
                m_beamImage->setPixel(7, k, r, g, b);
            }
            m_beamTexture->bind();
        }
    }
}

void CIvfFemBeam::setBeamModel(CIvfBeamModel *beamModel)
{
    m_beamModel = beamModel;
}

double CIvfFemBeam::calcNavier(double N, double My, double Mz)
{
    double E, G, A, Iy, Iz, Kv;
    double eyMax, eyMin, ezMax, ezMin;
    double sig[4];
    double sigN;
    double maxSig;
    int i;

    m_femBeam->getMaterial()->getProperties(E, G, A, Iy, Iz, Kv);
    m_femBeam->getMaterial()->getSection()->getExcY(eyMax, eyMin);
    m_femBeam->getMaterial()->getSection()->getExcZ(ezMax, ezMin);

    sigN = N/A;

    for (i=0; i<4; i++)
        sig[i] = sigN;

    sig[0]+=Mz*ezMax/Iz;
    sig[1]+=Mz*ezMax/Iz;
    sig[2]-=Mz*ezMin/Iz;
    sig[3]-=Mz*ezMin/Iz;

    sig[0]+=My*eyMax/Iy;
    sig[1]-=My*eyMin/Iy;
    sig[2]+=My*eyMax/Iy;
    sig[3]-=My*eyMax/Iy;

    maxSig = -1.0e300;

    for (i=0; i<4; i++)
        if (fabs(sig[i])>maxSig)
            maxSig = fabs(sig[i]);

    return maxSig;
}
