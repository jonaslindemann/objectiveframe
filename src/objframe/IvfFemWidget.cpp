#include "IvfFemWidget.h"

#include <sstream>

#include <FL/Fl_Native_File_Chooser.H>

#include <FL/fl_message.H>

#include <ivf/IvfCoordinateSystem.h>
#include <ivf/IvfTexture.h>
#include <ivfimage/IvfSgiImage.h>

#include <FemBeam.h>
#include <FemNode.h>
#include <FemBeamLoad.h>
#include <FemSLFFEAWriter.h>
#include <ProcessManager.h>

#include "NodePropDlg.h"
#include "BeamPropDlg.h"
#include "MaterialsDlg.h"
#include "ElementLoadsDlg.h"
#include "NodeLoadsDlg.h"
#include "NodeBCsDlg.h"
#include "ScalefactorDlg.h"
#include "StructureDlg.h"
#include "FeedbackDlg.h"
#include "StructureFactory.h"

#include "StatusOutput.h"

#ifdef HAVE_CORBA
#include "FemDFEMCInterface.h"
#endif

#define BTN_SELECT		  1001
#define BTN_MOVE          1002
#define BTN_CREATE_NODE   1003
#define BTN_CREATE_BEAM   1004
#define BTN_DELETE        1005
#define BTN_INSPECT       1006
#define BTN_NODE_BC       1007
#define BTN_NODE_LOAD     1008
#define BTN_BEAM_LOAD     1009
#define BTN_VIEW_ZOOM     1010
#define BTN_VIEW_PAN      1011
#define BTN_VIEW_RESET    1012
#define BTN_VIEW_CENTER   1013
#define BTN_MATERIALS     1014
#define BTN_FEEDBACK      1020

// ------------------------------------------------------------
// ------------------------------------------------------------
// IvfFemWidget feedback callback
// ------------------------------------------------------------
// ------------------------------------------------------------

void feedbackCallback(void* pointer)
{
    CIvfFemWidget* widget = (CIvfFemWidget*) pointer;
    widget->doFeedback();
    Fl::add_timeout(0.1f, feedbackCallback, widget);
}

#ifdef ADVANCED_GL
void hintCallback(void* pointer)
{
    CIvfFemWidget* widget = (CIvfFemWidget*) pointer;
    widget->doHint();
    if (!widget->isHintFinished())
        Fl::add_timeout(0.1f, hintCallback, widget);
}
#endif

// ------------------------------------------------------------
// ------------------------------------------------------------
// IvfFemWidget constructor
// ------------------------------------------------------------
// ------------------------------------------------------------

CIvfFemWidget::CIvfFemWidget(int X, int Y, int W, int H, const char *L) :
    CIvfFltkWidget(X, Y, W, H, L)
{
    m_width = W;
    m_height = H;
    so_print("FemWidget: Initializing variables.");
    m_tactileForce = NULL;
    m_internalSolver = NULL;
    m_relNodeSize = 0.004;
    m_relLineRadius = 0.0015;
    m_relLoadSize = 0.07;
    m_customMode = OF_NORMAL;
    m_customModeSet = false;
    m_alfa = 0.0;
    m_beta = 0.0;
    m_startAlfa = 0.0;
    m_startBeta = M_PI/2.0;;
    m_haveScaleFactor = false;
    m_needRecalc = true; 
    m_selectFilter = SF_ALL;
    m_deleteFilter = DF_ALL;
    m_highlightFilter = HF_ALL;
    m_overWorkspace = true;
    m_lastOverWorkspace = true;
    m_hintFinished = true;
    m_lockScaleFactor = false;
    m_saneModel = false;

    m_tactileForceValue = 1000.0;

    // Initialize GUI variables

    m_coordWidget = NULL;

    m_progPath = "";

}

void CIvfFemWidget::onInit()
{
    so_print("FemWidget: Initializing FemWidget.");

    // Intialize transparent workspace plane

#ifdef ADVANCED_GL
    CIvfMaterialPtr material = new CIvfMaterial();
    material->setDiffuseColor(1.0f, 1.0f, 1.0f, 0.8f);
    material->setSpecularColor(1.0f, 1.0f, 1.0f, 0.8f);
    material->setAmbientColor(0.3f, 0.3f, 0.3f, 0.8f);
#else
    CIvfMaterialPtr material;
#endif

#ifdef SIMPLE_GRID
    CIvfSgiImagePtr image = new CIvfSgiImage();
    image->setFileName("images/grid.rgb");
    image->read();

    CIvfTexturePtr texture = new CIvfTexture();
    texture->setImage(image);
    texture->setGenerateMipmaps(true);
    texture->setFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    texture->setTextureMode(GL_DECAL);
#endif

#ifdef SIMPLE_GRID
    m_plane = new CIvfQuadPlane();
    m_plane->setMaterial(material);
    m_plane->setSize(20.0, 20.0);
    m_plane->setRotationQuat(1.0, 0.0, 0.0, 90.0);
    m_plane->setTexture(texture);
    m_plane->setTextureCoord(0,0.0,0.0);
    m_plane->setTextureCoord(1,20.0,0.0);
    m_plane->setTextureCoord(2,20.0,20.0);
    m_plane->setTextureCoord(3,0.0,20.0);
    m_plane->addReference();
    m_plane->setUseName(true);
    this->getScene()->getPostComposite()->addChild(m_plane);
    this->getScene()->getCurrentPlane()->getGrid()->setUseGrid(false);
#endif
    this->getScene()->getCurrentPlane()->getCursor()->setThickness(0.02);
    this->getScene()->setRenderFlatShadow(true);
    this->getScene()->setShadowColor(0.3, 0.3, 0.3);

    // Common 3D gui state variables

    so_print("FemWidget: Initializing variables.");
    this->setWorkspace(20.0);

    // Initialize Ivf++ variables
#ifdef ADVANCED_GL
    m_selectedShape = NULL;
    m_selectedButton = NULL;
#endif
    // Initialize scene

    so_print("FemWidget: Initializing scene.");

    // Define node material

    so_print("FemWidget: Defining node material.");
    m_nodeMaterial = new CIvfMaterial();
    m_nodeMaterial->addReference();
    m_nodeMaterial->setDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
    m_nodeMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_nodeMaterial->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);

    // Define line material

    so_print("FemWidget: Defining line material.");
    m_lineMaterial = new CIvfMaterial();
    m_lineMaterial->addReference();
    m_lineMaterial->setDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
    m_lineMaterial->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
    m_lineMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Initialize beam model

    so_print("FemWidget: Setting color map path.");
    std::string colorPath = m_progPath;
    colorPath = colorPath + "maps/";

    std::cout << m_progPath << std::endl;

    so_print("FemWidget: Initializing beam model.");
    m_beamModel = new CIvfBeamModel();
    m_beamModel->initialize();
    m_beamModel->setPath(colorPath);
    m_beamModel->setScene(this->getScene()->getComposite());
    m_beamModel->setNodeSize(this->getWorkspace()*m_relNodeSize);
    m_beamModel->setNodeType(CIvfNode::NT_CUBE);
    m_beamModel->setLineRadius(this->getWorkspace()*m_relLineRadius);
    m_beamModel->setLoadSize(this->getWorkspace()*m_relLoadSize);
    m_beamModel->setBeamLoadSize(this->getWorkspace()*m_relLoadSize);
    m_beamModel->setNodeMaterial(m_nodeMaterial);
    m_beamModel->setBeamMaterial(m_lineMaterial);

    // Initialize color table

    so_print("FemWidget: Initializing color table.");
    CIvfColorTable* colorTable = m_beamModel->getColorTable();
    uchar r,g,b;

    for (int i=0; i<256; i++)
    {
        Fl::get_color((Fl_Color)i, r, g, b);
        colorTable->setColor(i,
                             (double)r / 255.0,
                             (double)g / 255.0,
                             (double)b / 255.0);
    }

    // Initialize gle library

    so_print("FemWidget: Initializing gle library.");
    ivfSetGLEJoinStyle(TUBE_JN_CAP|TUBE_NORM_EDGE|TUBE_JN_ANGLE);

    // Initialize model file name variables

    so_print("FemWidget: Setting initial file name.");
    m_fileName = "";
    this->setFileName("noname.df3");

    // Overlay management

    m_overlaySelected = false;

#ifdef ADVANCED_GL
    m_overlayScene = new CIvfSelectOrtho();
    m_overlayScene->setViewport(m_width, m_height);
    m_overlayScene->setUseCustomTransform(false);
    this->setUseOverlay(true);
    this->setupOverlay();
#endif

    m_coordText = "";

    // Create tactile Force icon

    so_print("FemWidget: Setting material for tactile force.");
    material = new CIvfMaterial();
    material->setDiffuseColor(1.0f, 1.0f, 0.0f, 1.0f);
    material->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    material->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);

    so_print("FemWidget: Creating tactile force.");

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce = new CIvfExtrArrow();
    m_tactileForce->setSize(loadSize*0.6, loadSize*0.6*0.20);
    m_tactileForce->setRadius(loadSize*0.055, loadSize*0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize*0.7);
    m_tactileForce->setMaterial(material);
    m_tactileForce->addReference();
    m_tactileForce->setState(CIvfShape::OS_OFF);
    this->getScene()->addChild(m_tactileForce);

    so_print("DialogMgr: Creating element load dialog.");
    m_dlgElementLoads = new CElementLoadsDlg();
    m_dlgElementLoads->setFemWidget((void*)this);

    so_print("DialogMgr: Creating materials dialog.");
    m_dlgMaterials = new CMaterialsDlg();
    m_dlgMaterials->setFemWidget((void*)this);

    so_print("DialogMgr: Creating node prop dialog.");
    m_dlgNodeProp = new CNodePropDlg();

    so_print("DialogMgr: Creating node load dialog.");
    m_dlgNodeLoads = new CNodeLoadsDlg();
    m_dlgNodeLoads->setFemWidget((void*)this);

    so_print("DialogMgr: Creating beam prop dialog.");
    m_dlgBeamProp = new CBeamPropDlg();
    m_dlgBeamProp->setFemWidget((void*)this);

    so_print("DialogMgr: Creating node bc dialog.");
    m_dlgNodeBCs = new CNodeBCsDlg();
    m_dlgNodeBCs->setFemWidget((void*)this);

    so_print("DialogMgr: Creating scale factor dialog.");
    m_dlgScalefactor = new CScalefactorDlg();
    m_dlgScalefactor->setFemWidget((void*)this);

    so_print("DialogMgr: Creating structure dialog.");
    m_dlgStructure = new CStructureDlg();

    // Set initial edit mode

    so_print("FemWidget: Setting initial edit mode.");
    this->setEditMode(IVF_VIEW_ZOOM);
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// Ivf FemWidget destructor
// ------------------------------------------------------------
// ------------------------------------------------------------

CIvfFemWidget::~CIvfFemWidget()
{
    so_print("FemWidget: Destructor.");

    // Do some cleanup

    delete m_internalSolver;

    so_print("FemWidget: Deleting node properties dialog.");
    delete m_dlgNodeProp;
    so_print("FemWidget: Deleting beam properties dialog.");
    delete m_dlgBeamProp;
    so_print("FemWidget: Deleting materials dialog.");
    delete m_dlgMaterials;
    so_print("FemWidget: Deleting element load dialog.");
    delete m_dlgElementLoads;
    so_print("FemWidget: Deleting node load dialog.");
    delete m_dlgNodeLoads;
    so_print("FemWidget: Deleting node bc dialog.");
    delete m_dlgNodeBCs;
    so_print("FemWidget: Deleting scale factor dialog.");
    delete m_dlgScalefactor;
    so_print("FemWidget: Deleting structure dialog.");
    delete m_dlgStructure;
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// Set/Get methods
// ------------------------------------------------------------
// ------------------------------------------------------------

// ------------------------------------------------------------
void CIvfFemWidget::setCoordWidget(Fl_Widget* widget)
{
    m_coordWidget = widget;
}

// ------------------------------------------------------------
Fl_Widget* CIvfFemWidget::getCoordWidget()
{
    return m_coordWidget;
}

// ------------------------------------------------------------
void CIvfFemWidget::setFileName(const std::string& name)
{
    m_fileName = name;
}

// ------------------------------------------------------------
const std::string CIvfFemWidget::getFileName()
{
    // Return current filename

    return m_fileName;
}

// ------------------------------------------------------------
void CIvfFemWidget::setWorkspace(double size)
{
    CIvfFltkWidget::setWorkspace(size);
#ifdef SIMPLE_GRID
    m_plane->setSize(size, size);
    m_plane->setTextureCoord(0,0.0,0.0);
    m_plane->setTextureCoord(1,20.0,0.0);
    m_plane->setTextureCoord(2,20.0,20.0);
    m_plane->setTextureCoord(3,0.0,20.0);
#endif

    if (m_beamModel!=NULL)
    {
        m_beamModel->setNodeSize(this->getWorkspace()*m_relNodeSize);
        m_beamModel->setNodeType(CIvfNode::NT_CUBE);
        m_beamModel->setLineRadius(this->getWorkspace()*m_relLineRadius);
        m_beamModel->setLoadSize(this->getWorkspace()*m_relLoadSize);
        m_beamModel->setBeamLoadSize(this->getWorkspace()*m_relLoadSize);
    }

    this->set_changed();
    this->resetView();
    this->getCamera()->setPerspective(45.0, size/50.0, size*4.0);

    if (m_tactileForce!=NULL)
    {
        double loadSize;
        if (m_beamModel!=NULL)
            loadSize = m_beamModel->getLoadSize();
        else
            loadSize = 1.0;

        m_tactileForce->setSize(loadSize*0.6, loadSize*0.6*0.20);
        m_tactileForce->setRadius(loadSize*0.055, loadSize*0.035);
        m_tactileForce->setDirection(0.0, -1.0, 0.0);
        m_tactileForce->setOffset(-loadSize*0.7);
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::setCurrentMaterial(CFemBeamMaterial *material)
{
    // Set current material

    m_currentMaterial = material;
}

// ------------------------------------------------------------
CFemBeamMaterial* CIvfFemWidget::getCurrentMaterial()
{
    // Return current material

    return m_currentMaterial;
}

// ------------------------------------------------------------
void CIvfFemWidget::setCurrentBeamLoad(CFemBeamLoad *elementLoad)
{
    // Set current elementload

    m_currentElementLoad = elementLoad;
}

// ------------------------------------------------------------
void CIvfFemWidget::setRepresentation(int repr)
{
    // Change model representation

    m_representation = repr;

    switch (m_representation) {
    case FRAME_FEM:
        so_print("FemWidget: Setting representation to FRAME_FEM.");
        ivfSetGLEJoinStyle(TUBE_JN_CAP|TUBE_NORM_EDGE|TUBE_JN_ANGLE);
        m_beamModel->setBeamType(IVF_BEAM_SOLID);
        m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
        break;
    case FRAME_GEOMETRY:
        so_print("FemWidget: Setting representation to FRAME_GEOMETRY.");
        ivfSetGLEJoinStyle(TUBE_JN_CAP|TUBE_NORM_FACET|TUBE_JN_ANGLE);
        m_beamModel->setBeamType(IVF_BEAM_EXTRUSION);
        m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
        break;
    case FRAME_DISPLACEMENTS:
        so_print("FemWidget: Setting representation to FRAME_DISPLACEMENTS.");
        ivfSetGLEJoinStyle(TUBE_JN_CAP|TUBE_NORM_EDGE|TUBE_JN_ANGLE);
        m_beamModel->setBeamType(IVF_BEAM_SOLID);
        m_beamModel->setNodeType(IVF_NODE_DISPLACEMENT);
        break;
    case FRAME_RESULTS:
        so_print("FemWidget: Setting representation to FRAME_RESULTS.");
        ivfSetGLEJoinStyle(TUBE_JN_CAP|TUBE_NORM_EDGE|TUBE_JN_ANGLE);
        m_beamModel->setBeamType(IVF_BEAM_RESULTS);
        m_beamModel->setNodeType(IVF_NODE_DISPLACEMENT);
        break;
    default:
        m_beamModel->setBeamType(IVF_BEAM_SOLID);
        m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
        break;
    }

    // Tell all beams/nodes in scene to change their appearance

    /*
     for (int i=0; i<this->getScene()->getSize(); i++)
     {
     CIvfShape* shape = this->getScene()->getChild(i);
     if (shape->isClass("CIvfFemBeam"))
     {
     CIvfFemBeam* ivfBeam = (CIvfFemBeam*) shape;
     ivfBeam->setBeamType(beamType);
     ivfBeam->setResultType(IVF_BEAM_M);
     }
     if (shape->isClass("CIvfFemNode"))
     {
     CIvfFemNode* ivfFemNode = (CIvfFemNode*) shape;
     ivfFemNode->setNodeType(nodeType);
     ivfFemNode->setScalefactor(m_beamModel->getScaleFactor());
     }
     }
     */

    // Shapes has to be refreshed to represent the
    // the changes

    this->set_changed();
    this->redraw();
}

// ------------------------------------------------------------
CIvfShape* CIvfFemWidget::getSelectedShape()
{
    // Return currently selected shape

    return m_selectedShape;
}

// ------------------------------------------------------------
CFemBeamLoad* CIvfFemWidget::getCurrentBeamLoad()
{
    return m_currentElementLoad;
}

// ------------------------------------------------------------
void CIvfFemWidget::setCurrentNodeLoad(CFemBeamNodeLoad *nodeLoad)
{
    m_currentNodeLoad = nodeLoad;
}

// ------------------------------------------------------------
CFemBeamNodeLoad* CIvfFemWidget::getCurrentNodeLoad()
{
    return m_currentNodeLoad;
}

// ------------------------------------------------------------
void CIvfFemWidget::setEditMode(int mode)
{
    // If setEditMode is not called in response to a setCustomMode call,
    // the custom mode is set to OF_NORMAL

    // set highlight filter

    switch (mode) {
    case IVF_SELECT:
        setHighlightFilter(HF_ALL);
        setSelectFilter(SF_ALL);
        setRepresentation(FRAME_FEM);
        break;
    case IVF_CREATE_NODE:
    case IVF_CREATE_LINE:
        setHighlightFilter(HF_NODES);
        setSelectFilter(SF_NODES);
        setRepresentation(FRAME_FEM);
        break;
    default:
        setHighlightFilter(HF_ALL);
        setSelectFilter(SF_ALL);
        break;
    }

    if ((m_customMode == OF_FEEDBACK)&&(m_customModeSet))
    {
        setHighlightFilter(HF_NODES);
        setSelectFilter(SF_NODES);
    }

    if (!m_customModeSet)
    {
        m_tactileForce->setState(CIvfShape::OS_OFF);
        m_interactionNode = NULL;
        m_customMode = OF_NORMAL;
        this->redraw();
    }
    else
        m_customModeSet = false;

    if (m_customMode==OF_FEEDBACK)
    {
        this->getScene()->getComposite()->setHighlightChildren(CIvfShape::HS_OFF);
        this->setRepresentation(FRAME_DISPLACEMENTS);
    }

    m_coordText = "";

    CIvfFltkWidget::setEditMode(mode);
}


// ------------------------------------------------------------
void CIvfFemWidget::setBeamRefreshMode(int mode)
{
    CIvfComposite* scene = this->getScene()->getComposite();
    int i;

    for (i=0; i<scene->getSize(); i++)
    {
        CIvfShape* shape = scene->getChild(i);
        if (shape->isClass("CIvfSolidLine"))
        {
            CIvfSolidLine* solidLine = (CIvfSolidLine*) shape;
            solidLine->setRefresh(mode);
        }
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::setArguments(int argc, char **argv)
{
    m_argc = argc;
    m_argv = argv;
}

// ------------------------------------------------------------
void CIvfFemWidget::setScalefactor(double scalefactor)
{
    m_beamModel->setScaleFactor(scalefactor);
    this->set_changed();
    this->redraw();
}

// ------------------------------------------------------------
double CIvfFemWidget::getScalefactor()
{
    return m_beamModel->getScaleFactor();
}

// ------------------------------------------------------------
void CIvfFemWidget::setCurrentNodeBC(CFemBeamNodeBC *bc)
{
    m_currentNodeBC = bc;
}

// ------------------------------------------------------------
void CIvfFemWidget::setCustomMode(int mode)
{
    if (m_customMode==OF_FEEDBACK)
    {
        //m_tactileForce->setState(IVF_OBJECT_ON);
        m_interactionNode = NULL;
    }
    m_customMode = mode;
    m_customModeSet = true;
    m_haveScaleFactor = false;
    this->setBeamRefreshMode(IVF_REFRESH_NODES);
    if (m_customMode!=OF_NORMAL)
    {
        m_tactileForce->setState(CIvfShape::OS_OFF);
        m_interactionNode = NULL;
        this->clearSelection();
        this->redraw();
        m_beamModel->clearNodeValues();
        this->setResultType(IVF_BEAM_NO_RESULT);
        this->setEditMode(IVF_SELECT);
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::setSelectFilter(int filter)
{
    m_selectFilter = filter;
}

// ------------------------------------------------------------
void CIvfFemWidget::setDeleteFilter(int filter)
{
    m_deleteFilter = filter;
}

// ------------------------------------------------------------
void CIvfFemWidget::setHighlightFilter(int filter)
{
    m_highlightFilter = filter;
}

// ------------------------------------------------------------
void CIvfFemWidget::setNeedRecalc(bool flag)
{
    m_needRecalc = flag;
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// Widget methods
// ------------------------------------------------------------
// ------------------------------------------------------------

// ------------------------------------------------------------
void CIvfFemWidget::save()
{
    // Save model

    if (m_fileName == "noname.df3")
    {
        Fl_Native_File_Chooser fnfc;
        fnfc.title("Save");
        fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
        fnfc.filter("ObjectiveFrame\t*.df3\n");
        fnfc.directory("");           // default directory to use

        int result = fnfc.show();

        if (result!=0)
            return;

        std::string filename = fnfc.filename();
        this->setFileName(filename);
    }
    m_beamModel->setFileName(m_fileName);
    m_beamModel->save();
}

// ------------------------------------------------------------
void CIvfFemWidget::saveAs()
{
    // Save model

    Fl_Native_File_Chooser fnfc;
    fnfc.title("Save as");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    fnfc.filter("ObjectiveFrame\t*.df3\n");
    fnfc.directory("");           // default directory to use

    int result = fnfc.show();

    if (result==0)
    {
        std::string filename = fnfc.filename();
        this->setFileName(filename);
        m_beamModel->setFileName(m_fileName);
        m_beamModel->save();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::open()
{
    // Open model

    // Disable callbacks

    disableHint();

    // Prompt for a filename

    Fl_Native_File_Chooser fnfc;
    fnfc.title("Open file");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fnfc.filter("ObjectiveFrame\t*.df3\n");
    fnfc.directory("");           // default directory to use

    int result = fnfc.show();

    //char* fname = fl_file_chooser("Open file", "*.df3", "");

    // If we have a filename we try to open.

    if (result==0)
    {
        // Change filename and erase previous model/scene

        std::string filename = fnfc.filename();
        this->setFileName(filename);
        this->deleteAll();
        //this->addToScene(m_plane);

        so_print("FemWidget: Setting color map path.");
        std::string colorPath = "";

        colorPath = colorPath + m_progPath;
        colorPath = colorPath + "maps/";

        // Initialize and open beam model

        m_beamModel->initialize();
        m_beamModel->setFileName(m_fileName);
        m_beamModel->setPath(colorPath);
        m_beamModel->open();

        // Generate a Ivf++ representation

        m_beamModel->generateModel();

        // Update dialogs


        m_dlgNodeProp->setNode(NULL);
        m_dlgBeamProp->setBeam(NULL);
        m_dlgMaterials->setMaterials(m_beamModel->getMaterialSet());
        m_dlgElementLoads->setLoadSet(m_beamModel->getElementLoadSet());
        m_dlgNodeLoads->setLoadSet(m_beamModel->getNodeLoadSet());
        m_dlgNodeBCs->setBCSet(m_beamModel->getNodeBCSet());

        // Add tactile force

        this->getScene()->addChild(m_tactileForce);

        double loadSize = m_beamModel->getLoadSize();

        m_tactileForce->setSize(loadSize*0.6, loadSize*0.6*0.20);
        m_tactileForce->setRadius(loadSize*0.055, loadSize*0.035);
        m_tactileForce->setDirection(0.0, -1.0, 0.0);
        m_tactileForce->setOffset(-loadSize*0.7);

        m_needRecalc = true;

        if (m_internalSolver!=NULL)
            delete m_internalSolver;

        m_internalSolver = NULL;

        this->setEditMode(IVF_VIEW_ZOOM);
    }

    // Enable hint callback

    initiateHint();
}


// ------------------------------------------------------------
void CIvfFemWidget::showProperties()
{
    // Properties for selected shape

    if (this->getSelectedShape()!=NULL)
    {
        if (this->getSelectedShape()->isClass("CIvfFemNode"))
            m_dlgNodeProp->show();

        if (this->getSelectedShape()->isClass("CIvfFemBeam"))
            m_dlgBeamProp->show();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::showMaterials()
{
    // Show materials dialog

    setRepresentation(FRAME_FEM);
    m_dlgMaterials->setMaterials(m_beamModel->getMaterialSet());
    m_dlgMaterials->show();
}

// ------------------------------------------------------------
void CIvfFemWidget::newModel()
{
    // Disable hint callback

    disableHint();

    // Delete all Ivf++ objects

    this->deleteAll();

    // Setup color map path

    std::string colorPath = m_progPath;
    colorPath = colorPath + "maps\\";

    // Setup new beam model

    m_beamModel = new CIvfBeamModel();
    m_beamModel->initialize();
    m_beamModel->setPath(colorPath);
    m_beamModel->setScene(this->getScene()->getComposite());
    m_beamModel->setNodeSize(this->getWorkspace()*m_relNodeSize);
    m_beamModel->setNodeType(CIvfNode::NT_CUBE);
    m_beamModel->setLineRadius(this->getWorkspace()*m_relLineRadius);
    m_beamModel->setLoadSize(this->getWorkspace()*m_relLoadSize);
    m_beamModel->setBeamLoadSize(this->getWorkspace()*m_relLoadSize);
    m_beamModel->setNodeMaterial(m_nodeMaterial);
    m_beamModel->setBeamMaterial(m_lineMaterial);

    m_currentMaterial = NULL;


    // Initialize color table

    CIvfColorTable* colorTable = m_beamModel->getColorTable();
    uchar r,g,b;

    for (int i=0; i<256; i++)
    {
        Fl::get_color((Fl_Color)i, r, g, b);
        colorTable->setColor(i,
                             (double)r / 255.0,
                             (double)g / 255.0,
                             (double)b / 255.0);
    }

    // Initialize dialogs

    m_dlgNodeProp->setNode(NULL);
    m_dlgBeamProp->setBeam(NULL);
    m_dlgMaterials->setMaterials(m_beamModel->getMaterialSet());

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize*0.6, loadSize*0.6*0.20);
    m_tactileForce->setRadius(loadSize*0.055, loadSize*0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize*0.7);

    m_needRecalc = true;

    if (m_internalSolver!=NULL)
        delete m_internalSolver;

    m_internalSolver = NULL;

    this->setEditMode(IVF_VIEW_ZOOM);
    //this->getScene()->getWorldSystem()->getGrid()->setAxisSize(this->getWorkspace()*0.05);

    // Enable hint callback

    initiateHint();

    // Update screen

    this->redraw();
}

// ------------------------------------------------------------
void CIvfFemWidget::assignMaterialToSelected()
{
    // Assigns a material to selected shapes

    if (this->getCurrentMaterial()!=NULL)
    {
        CIvfComposite* selected = this->getSelectedShapes();
        for (int i=0; i<selected->getSize(); i++)
        {
            CIvfShape* shape = selected->getChild(i);
            if (shape->isClass("CIvfFemBeam"))
            {
                CIvfFemBeam* ivfBeam = (CIvfFemBeam*)shape;
                ivfBeam->getBeam()->setMaterial(this->getCurrentMaterial());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::removeMaterialFromSelected()
{
    // Remove materials from selected shapes

    CIvfComposite* selected = this->getSelectedShapes();
    for (int i=0; i<selected->getSize(); i++)
    {
        CIvfShape* shape = selected->getChild(i);
        if (shape->isClass("CIvfFemBeam"))
        {
            CIvfFemBeam* ivfBeam = (CIvfFemBeam*)shape;
            ivfBeam->getBeam()->setMaterial(NULL);
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

// ------------------------------------------------------------
void CIvfFemWidget::deleteBeamLoad(CFemBeamLoad *elementLoad)
{
    // Delete a beam load


    // Get ivf representation from element load

    CIvfFemBeamLoad* ivfBeamLoad = (CIvfFemBeamLoad*) elementLoad->getUser();

    // Remove shape from scene and delete it

    this->getScene()->getComposite()->removeShape(ivfBeamLoad);
    delete ivfBeamLoad;

    // Remove load from beam model

    m_needRecalc = true;
    m_beamModel->getElementLoadSet()->removeLoad(elementLoad);
}

// ------------------------------------------------------------
void CIvfFemWidget::deleteSelected()
{
    // Delete things in an orderly fashion

    setDeleteFilter(DF_ELEMENTS);
    deleteSelectedKeep();
    setDeleteFilter(DF_NODES);
    deleteSelectedKeep();
    setDeleteFilter(DF_ALL);
}

// ------------------------------------------------------------
void CIvfFemWidget::addBeamLoad(CFemBeamLoad *elementLoad)
{
    // Add a beam load


    // Create ivf represenation

    CIvfFemBeamLoad* ivfLoad = new CIvfFemBeamLoad();
    ivfLoad->setBeamModel(m_beamModel);
    ivfLoad->setBeamLoad(elementLoad);

    // Set user property of element load to point to
    // our ivf representation

    elementLoad->setUser((void*)ivfLoad);

    // Initialize ivf representation

    ivfLoad->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(ivfLoad);
}

// ------------------------------------------------------------
void CIvfFemWidget::addNodeLoad(CFemBeamNodeLoad *nodeLoad)
{
    // Add a node load


    // Create ivf represenation

    CIvfFemNodeLoad* ivfNodeLoad = new CIvfFemNodeLoad();
    ivfNodeLoad->setBeamModel(m_beamModel);
    ivfNodeLoad->setNodeLoad(nodeLoad);

    // Set user property of element load to point to
    // our ivf representation

    nodeLoad->setUser((void*)ivfNodeLoad);

    // Initialize ivf representation

    ivfNodeLoad->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(ivfNodeLoad);
}

// ------------------------------------------------------------
void CIvfFemWidget::addNodeBC(CFemBeamNodeBC *bc)
{
    // Add a node load


    // Create ivf represenation

    CIvfFemNodeBC* ivfNodeBC = new CIvfFemNodeBC();
    ivfNodeBC->setBeamModel(m_beamModel);
    ivfNodeBC->setNodeBC(bc);

    // Set user property of element load to point to
    // our ivf representation

    bc->setUser((void*)ivfNodeBC);

    // Initialize ivf representation

    ivfNodeBC->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(ivfNodeBC);

}

// ------------------------------------------------------------
void CIvfFemWidget::showBeamLoads()
{
    // Show beam loads dialog

    setRepresentation(FRAME_FEM);
    m_dlgElementLoads->setLoadSet(m_beamModel->getElementLoadSet());
    m_dlgElementLoads->show();
}

// ------------------------------------------------------------
void CIvfFemWidget::assignBeamLoadSelected()
{
    // Assign a beam load to selected beams

    if (m_currentElementLoad!=NULL)
    {
        CIvfComposite* selected = this->getSelectedShapes();
        for (int i=0; i<selected->getSize(); i++)
        {
            CIvfShape* shape = selected->getChild(i);
            if (shape->isClass("CIvfFemBeam"))
            {
                CIvfFemBeam* ivfBeam = (CIvfFemBeam*)shape;
                m_currentElementLoad->addElement((CFemElement*)ivfBeam->getBeam());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::assignNodeLoadSelected()
{
    // Assign a node load to selected nodes

    if (m_currentNodeLoad!=NULL)
    {
        CIvfComposite* selected = this->getSelectedShapes();
        for (int i=0; i<selected->getSize(); i++)
        {
            CIvfShape* shape = selected->getChild(i);
            if (shape->isClass("CIvfFemNode"))
            {
                CIvfFemNode* ivfNode = (CIvfFemNode*)shape;
                m_currentNodeLoad->addNode((CFemNode*)ivfNode->getFemNode());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::deleteNodeLoad(CFemBeamNodeLoad *nodeLoad)
{
    // Delete a node load


    // Get ivf representation from element load

    CIvfFemNodeLoad* ivfNodeLoad = (CIvfFemNodeLoad*) nodeLoad->getUser();

    // Remove shape from scene and delete it

    this->getScene()->getComposite()->removeShape(ivfNodeLoad);
    delete ivfNodeLoad;

    // Remove load from beam model

    m_beamModel->getNodeLoadSet()->removeLoad(nodeLoad);
    setCurrentNodeLoad(NULL);
}

// ------------------------------------------------------------
void CIvfFemWidget::deleteNodeBC(CFemBeamNodeBC *bc)
{
    // Delete a node load


    // Get ivf representation from element load

    CIvfFemNodeBC* ivfNodeBC = (CIvfFemNodeBC*) bc->getUser();

    // Remove shape from scene and delete it

    this->getScene()->getComposite()->removeShape(ivfNodeBC);
    delete ivfNodeBC;

    // Remove load from beam model

    m_needRecalc = true;
    m_beamModel->getNodeBCSet()->removeBC(bc);
    setCurrentNodeBC(NULL);
}


// ------------------------------------------------------------
void CIvfFemWidget::showNodeLoads()
{
    // Show beam loads dialog

    setRepresentation(FRAME_FEM);
    m_dlgNodeLoads->setLoadSet(m_beamModel->getNodeLoadSet());
    m_dlgNodeLoads->show();
}

// ------------------------------------------------------------
void CIvfFemWidget::setRotationSelected(double rotation)
{
    // Assigns a material to selected shapes

    CIvfComposite* selected = this->getSelectedShapes();
    for (int i=0; i<selected->getSize(); i++)
    {
        CIvfShape* shape = selected->getChild(i);
        if (shape->isClass("CIvfFemBeam"))
        {
            CIvfFemBeam* ivfBeam = (CIvfFemBeam*)shape;
            ivfBeam->getBeam()->setBeamRotation(rotation);
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    this->set_changed();
    this->redraw();
}

// ------------------------------------------------------------
void CIvfFemWidget::setupOverlay()
{
#ifdef ADVANCED_GL
    CIvfPlaneButton* button;

    /*
     m_logoFace = new FTFace();
     m_logoFace->open("fonts/BATTLE3.TTF");
     m_logoFont = new GLTTPixmapFont(m_logoFace);
     m_logoFont->create(20);

     m_coordFace = new FTFace();
     m_coordFace->open("fonts/BATTLE3.TTF");
     m_coordFont = new GLTTPixmapFont(m_coordFace);
     m_coordFont->create(15);
     */

    m_editArea = new CIvfArea2D();
    m_editArea->add(0,0);
    m_editArea->add(65,0);
    m_editArea->add(65,400);
    m_editArea->add(0,400);
    m_editArea->setColor(0, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(1, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(2, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(3, 0.0f, 0.0f, 0.0f);
    m_areas.push_back(m_editArea);

    m_objectArea = new CIvfArea2D();
    m_objectArea->add(0,0);
    m_objectArea->add(0,0);
    m_objectArea->add(0,0);
    m_objectArea->add(0,0);
    m_objectArea->setColor(0, 0.0f, 0.0f, 0.0f);
    m_objectArea->setColor(1, 0.0f, 0.0f, 0.0f);
    m_objectArea->setColor(2, 0.0f, 0.0f, 0.0f);
    m_objectArea->setColor(3, 0.0f, 0.0f, 0.0f);
    m_areas.push_back(m_objectArea);

    m_viewArea = new CIvfArea2D();
    m_viewArea->add(0,0);
    m_viewArea->add(0,0);
    m_viewArea->add(0,0);
    m_viewArea->add(0,0);
    m_viewArea->setColor(0, 0.0f, 0.0f, 0.0f);
    m_viewArea->setColor(1, 0.0f, 0.0f, 0.0f);
    m_viewArea->setColor(2, 0.0f, 0.0f, 0.0f);
    m_viewArea->setColor(3, 0.0f, 0.0f, 0.0f);
    m_areas.push_back(m_viewArea);

    // Create edit toolbar

    m_editButtons = new CIvfButtonGroup();

    button= new CIvfPlaneButton(BTN_SELECT, "images/tlselect.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0,30.0,0.0);
    button->setHint("Select nodes or elements");
    m_editButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_MOVE, "images/tlmove.png");
    button->setSize(40.0,40.0);
    button->setPosition(30.0,110.0,0.0);
    button->setHint("Move nodes or elements");
    m_editButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_INSPECT, "images/tlinspect.png");
    button->setSize(40.0,40.0);
    button->setPosition(30.0,170,0.0);
    button->setHint("Node or element info");
    m_editButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_DELETE, "images/tldelete.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0,240.0,0.0);
    button->setHint("Delete node or element");
    m_editButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_FEEDBACK, "images/tlfeedback.png");
    button->setSize(40.0,40.0);
    button->setPosition(30.0,330.0,0.0);
    button->setHint("Feedback mode");
    m_editButtons->addChild(button);

    m_overlayScene->addChild(m_editButtons);

    //
    // Create object toolbar
    //

    m_objectButtons = new CIvfButtonGroup();

    button= new CIvfPlaneButton(BTN_CREATE_NODE, "images/tlnode.png");
    button->setSize(50.0,50.0);
    button->setPosition(30.0,30.0,0.0);
    button->setHint("Create node");
    m_objectButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_CREATE_BEAM, "images/tlsolidline.png");
    button->setSize(50.0,50.0);
    button->setPosition(90.0,30.0,0.0);
    button->setHint("Create element");
    m_objectButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_NODE_LOAD, "images/tlnodeloads.png");
    button->setSize(50.0,50.0);
    button->setPosition(150.0,30.0,0.0);
    button->setHint("Show node loads");
    m_objectButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_BEAM_LOAD, "images/tldload.png");
    button->setSize(50.0,50.0);
    button->setPosition(210.0,30.0,0.0);
    button->setHint("Show element loads");
    m_objectButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_NODE_BC, "images/tlbc.png");
    button->setSize(50.0,50.0);
    button->setPosition(270.0,30.0,0.0);
    button->setHint("Show boundary conditions");
    m_objectButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_MATERIALS, "images/tlmaterials.png");
    button->setSize(50.0,50.0);
    button->setPosition(330.0,30.0,0.0);
    button->setHint("Show beam properties");
    m_objectButtons->addChild(button);

    m_overlayScene->addChild(m_objectButtons);

    //
    // Create view toolbar
    //

    m_viewButtons = new CIvfButtonGroup();

    button= new CIvfPlaneButton(BTN_VIEW_ZOOM, "images/tlviewzoom.png");
    button->setSize(40.0,40.0);
    button->setPosition(90.0,30.0,0.0);
    button->setHint("View/Zoom mode");
    m_viewButtons->addChild(button);

    button= new CIvfPlaneButton(BTN_VIEW_RESET, "images/tlviewreset.png");
    button->setSize(40.0,40.0);
    button->setPosition(150.0,30.0,0.0);
    button->setHint("Reset view");
    m_viewButtons->addChild(button);

    m_overlayScene->addChild(m_viewButtons);
#endif
}

// ------------------------------------------------------------
void CIvfFemWidget::assignNodeBCSelected()
{
    // Assign a node load to selected nodes

    if (m_currentNodeBC!=NULL)
    {
        CIvfComposite* selected = this->getSelectedShapes();
        for (int i=0; i<selected->getSize(); i++)
        {
            CIvfShape* shape = selected->getChild(i);
            if (shape->isClass("CIvfFemNode"))
            {
                CIvfFemNode* ivfNode = (CIvfFemNode*)shape;
                m_currentNodeBC->addNode((CFemNode*)ivfNode->getFemNode());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::showNodeBCs()
{
    // Show beam loads dialog

    setRepresentation(FRAME_FEM);
    m_dlgNodeBCs->setBCSet(m_beamModel->getNodeBCSet());
    m_dlgNodeBCs->show();
}

// ------------------------------------------------------------
void CIvfFemWidget::executeCalc()
{
    // Execute DFEMC Beam solver (CORBA)

    double maxNodeValue;
#ifdef HAVE_CORBA
    CFemDFEMCInterface* dfemcInterface = new CFemDFEMCInterface();
    dfemcInterface->setBeamModel(m_beamModel);
    dfemcInterface->setArguments(m_argc, m_argv);
    dfemcInterface->execute();
    maxNodeValue = dfemcInterface->getMaxNodeValue();
    delete dfemcInterface;
#else
    if (m_internalSolver!=NULL)
        delete m_internalSolver;

    m_internalSolver = new CFemInternalSolver();
    m_internalSolver->setBeamModel(m_beamModel);
    m_internalSolver->setResultInfo(m_beamModel->getResultInfo());
    m_internalSolver->execute();
    if (m_internalSolver->getLastError()!=BS_NO_ERROR)
    {
        switch (m_internalSolver->getLastError()) {
        case BS_NO_NODES:
            fl_message("No nodes defined. \nCalculation not executed.");
            break;
        case BS_NO_ELEMENTS:
            fl_message("No elements defined. \nCalculation not executed.");
            break;
        case BS_NO_BC:
            fl_message("No boundary conditions defined. \nCalculation not executed.");
            break;
        case BS_NO_LOADS:
            fl_message("No loads defined. \nCalculation not executed.");
            break;
        case BS_UNSTABLE:
            fl_message("System unstable. Try adding boundary conditions.\nCalculation not executed.");
            break;
        case BS_SINGULAR:
            fl_message("System is singular. Check for free nodes or other strange things. \nCalculation not executed.");
            break;
        case BS_INVALID_MODEL:
            fl_message("This should not happen.\nCalculation not executed.");
            break;
        case BS_UNDEFINED_MATERIAL:
            fl_message("Beams without materials found.");
            break;
        default:
            fl_message("Unhandled error.\nCalculation not executed.");
            break;
        }
        m_needRecalc = true;
    }
    else
        m_needRecalc = false;

    maxNodeValue = m_internalSolver->getMaxNodeValue();
#endif


    // Calculate default scalefactor

    //so_print("Max node value = " << maxNodeValue);

    if (!m_lockScaleFactor)
    {
        if (maxNodeValue>0.0)
            m_beamModel->setScaleFactor(this->getWorkspace()*0.020/maxNodeValue);
        else
            m_beamModel->setScaleFactor(1.0);
    }

    //so_print("Scale factor = " << m_scalefactor);

    // Show displacements

    this->setRepresentation(FRAME_RESULTS);

}

// ------------------------------------------------------------
void CIvfFemWidget::showScalefactorDlg()
{
    m_dlgScalefactor->setScaling(m_beamModel->getScaleFactor());
    m_dlgScalefactor->show();
}

// ------------------------------------------------------------
void CIvfFemWidget::selectAllNodes()
{
    setSelectFilter(SF_NODES);
    selectAll();
}

// ------------------------------------------------------------
void CIvfFemWidget::selectAllElements()
{
    setSelectFilter(SF_ELEMENTS);
    selectAll();
}

// ------------------------------------------------------------
void CIvfFemWidget::doFeedback()
{
    // Is there a calculation ?

    if (m_needRecalc)
    {
        if (m_interactionNode!=NULL)
        {
            CFeedbackDlg* dlg = new CFeedbackDlg();
            dlg->show();
            Fl::wait();

            double maxNodeValue;

            if (m_internalSolver!=NULL)
                delete m_internalSolver;

            m_internalSolver = new CFemInternalSolver();
            m_internalSolver->setResultInfo(m_beamModel->getResultInfo());
            m_internalSolver->setBeamModel(m_beamModel);

            double v[3];

            // Setup feedback force

            m_tactileForce->getDirection(v[0], v[1], v[2]);
            m_internalSolver->setFeedbackForce(
                m_interactionNode->getFemNode(),
                m_tactileForceValue*v[0], m_tactileForceValue*v[1], m_tactileForceValue*v[2]);

            m_internalSolver->execute();

            // We assume the worst case

            m_saneModel = false;

            switch (m_internalSolver->getLastError()) {
            case BS_NO_NODES:
                fl_message("No nodes defined. \nCalculation not executed.");
                break;
            case BS_NO_ELEMENTS:
                fl_message("No elements defined. \nCalculation not executed.");
                break;
            case BS_NO_BC:
                fl_message("No boundary conditions defined. \nCalculation not executed.");
                break;
            case BS_NO_LOADS:
                fl_message("No loads defined. \nCalculation not executed.");
                break;
            case BS_UNSTABLE:
                fl_message("System unstable. Try adding boundary conditions.\nCalculation not executed.");
                break;
            case BS_SINGULAR:
                fl_message("System is singular. Check for free nodes or other strange things. \nCalculation not executed.");
                break;
            case BS_INVALID_MODEL:
                fl_message("This should not happen.\nCalculation not executed.");
                break;
            case BS_UNDEFINED_MATERIAL:
                fl_message("Beams without materials found.");
                break;
            default:
                m_saneModel = true;
                break;
            }

            maxNodeValue = m_internalSolver->getMaxNodeValue();

            // Only compute the scale factor at the first attempt

            if (!m_lockScaleFactor)
            {
                if (!m_haveScaleFactor)
                {
                    m_beamModel->setScaleFactor(this->getWorkspace()*0.020/m_internalSolver->getMaxNodeValue());
                    m_haveScaleFactor = true;
                }
            }

            m_needRecalc = false;
            dlg->hide();

            delete dlg;

            // Show displacements

            this->setRepresentation(FRAME_RESULTS);
        }
    }

    // Continuosly recompute solution

    if (m_saneModel)
    {
        if (m_internalSolver!=NULL)
        {
            //Fl::check();
            if (m_interactionNode!=NULL)
            {

                double v[3];

                // Setup feedback force

                m_tactileForce->getDirection(v[0], v[1], v[2]);
                m_internalSolver->setFeedbackForce(
                    m_interactionNode->getFemNode(),
                    m_tactileForceValue*v[0], m_tactileForceValue*v[1], m_tactileForceValue*v[2]);

                // Execute calculation

                m_internalSolver->recompute();
                m_internalSolver->update(); // NEW


                // Only compute the scale factor at the first attempt

                if (!m_lockScaleFactor)
                {
                    if (!m_haveScaleFactor)
                    {
                        m_beamModel->setScaleFactor(this->getWorkspace()*0.020/m_internalSolver->getMaxNodeValue());
                        m_haveScaleFactor = true;
                    }
                }

                // Update tactile force icon

                double x, y, z;
                m_interactionNode->getDisplacedPosition(x, y, z);
                m_tactileForce->setPosition(x, y, z);

                // Refresh scene (Solid lines must be updated)

                this->getScene()->getComposite()->refresh();

                this->redraw();         // set damage(FL_DAMAGE_ALL)
                Fl::flush();
                Fl::check();
            }
        }
    }
    else
    {
        setEditMode(IVF_SELECT);
        refreshToolbars();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::showStructureDlg()
{
    int size[3];
    double spacing[3];

    m_dlgStructure->show();
    if (m_dlgStructure->getModalResult()==MR_OK)
    {
        m_dlgStructure->getSize(size[0], size[1], size[2]);
        m_dlgStructure->getSpacing(spacing[0], spacing[1], spacing[2]);

        CStructureFactory* factory = new CStructureFactory();
        factory->setBeamModel(m_beamModel);
        factory->setCurrentMaterial(this->getCurrentMaterial());
        factory->setSize(size[0], size[1], size[2]);
        factory->setSpacing(spacing[0], spacing[1], spacing[2]);
        factory->create();
        delete factory;

        this->redraw();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::doHint()
{
    m_hintColor[0] *= 0.95;
    m_hintColor[1] *= 0.95;
    m_hintColor[2] *= 0.95;

    if (m_hintColor[0]<0.05)
    {
        m_hintColor[0] = 0.0;
        m_hintColor[1] = 0.0;
        m_hintColor[2] = 0.0;
        m_hintFinished = true;
    }

    this->redraw();
}

// ------------------------------------------------------------
void CIvfFemWidget::initiateHint()
{
#ifdef ADVANCED_GL
    m_hintColor[0] = 0.6f;
    m_hintColor[1] = 0.6f;
    m_hintColor[2] = 0.6f;

    m_hintFinished = false;
    Fl::add_timeout(0.1f, hintCallback, this);
#endif
}

// ------------------------------------------------------------
bool CIvfFemWidget::isHintFinished()
{
    return m_hintFinished;
}

// ------------------------------------------------------------
void CIvfFemWidget::disableHint()
{
#ifdef ADVANCED_GL
    m_hintFinished = true;
    Fl::remove_timeout(hintCallback, this);
#endif
}

// ------------------------------------------------------------
void CIvfFemWidget::lockScaleFactor()
{
    m_lockScaleFactor = true;
}

// ------------------------------------------------------------
void CIvfFemWidget::unlockScaleFactor()
{
    m_lockScaleFactor = false;
}


// ------------------------------------------------------------
// ------------------------------------------------------------
// Widget events
// ------------------------------------------------------------
// ------------------------------------------------------------

void CIvfFemWidget::doMouse(int x, int y)
{
    if (!m_overlaySelected)
        CIvfFltkWidget::doMouse(x, y);
}

// ------------------------------------------------------------
void CIvfFemWidget::onCreateNode(double x, double y, double z, CIvfNode* &newNode)
{
    // Create a node


    // First we create a FemNode

    CFemNode* femNode = new CFemNode();

    // Add it to the Fem model

    m_beamModel->getNodeSet()->addNode(femNode);
    femNode->setNumber(m_beamModel->getNodeSet()->getSize()-1);

    // Create Ivf representation

    CIvfFemNode* ivfNode = new CIvfFemNode();
    ivfNode->setBeamModel(m_beamModel);
    ivfNode->setFemNode(femNode);
    ivfNode->setPosition(x, y, z);
    ivfNode->setMaterial(m_nodeMaterial);
    ivfNode->setDirectRefresh(true);

    // We need a recalc

    m_needRecalc = true;

    newNode = ivfNode;
}

// ------------------------------------------------------------
void CIvfFemWidget::onCreateLine(CIvfNode* node1, CIvfNode* node2, CIvfShape* &newLine)
{
    // Create visual representation

    CIvfFemBeam* ivfBeam = new CIvfFemBeam();
    ivfBeam->setBeamModel(m_beamModel);

    // Create model representation

    CFemBeam* femBeam =  new CFemBeam();

    // Extract FemNode:s from the IvfNodes

    CIvfFemNode* ivfNode1 = (CIvfFemNode*) node1;
    CIvfFemNode* ivfNode2 = (CIvfFemNode*) node2;

    if (node1==node2)
        return;

    // Add FemNodes to beam element

    femBeam->addNode(ivfNode1->getFemNode());
    femBeam->addNode(ivfNode2->getFemNode());

    // Set the material

    femBeam->setMaterial((CFemBeamMaterial*)m_beamModel->getMaterialSet()->currentMaterial());
    /*
    femBeam->setMaterial(
        m_dlgMaterials->getCurrentMaterial());
    */

    // Add beam element to beam model

    m_beamModel->getElementSet()->addElement(femBeam);

    // Initialize the representation

    ivfBeam->setBeam(femBeam);
    ivfBeam->setNodes((CIvfFemNode*)node1, (CIvfFemNode*)node2);
    ivfBeam->refresh();

    // We need a recalc

    m_needRecalc = true;

    // Return the finished object

    newLine = (CIvfShape*)ivfBeam;
}

// ------------------------------------------------------------
void CIvfFemWidget::onSelect(CIvfComposite* selectedShapes)
{
    // Handle object selection

    if (m_customMode == OF_NORMAL)
    {

        // Disable all dialogs

        m_dlgNodeProp->setNode(NULL);
        m_dlgBeamProp->setBeam(NULL);

        // Update dialogs with new selection

        if (selectedShapes->getSize()>0)
        {
            CIvfShape* firstShape = selectedShapes->getChild(0);
            m_selectedShape = firstShape;
            if (firstShape->isClass("CIvfFemNode"))
                m_dlgNodeProp->setNode((CIvfFemNode*)firstShape);
            if (firstShape->isClass("CIvfFemBeam"))
                m_dlgBeamProp->setBeam((CIvfFemBeam*)firstShape);
        }
    }
    else
    {
        if (selectedShapes->getSize()>0)
        {
            CIvfShape* shape = selectedShapes->getChild(0);
            if (shape->isClass("CIvfFemNode"))
            {
                CIvfFemNode* ivfNode = (CIvfFemNode*) shape;
                m_interactionNode = ivfNode;
                clearSelection();
                m_customModeSet = true;
                setEditMode(IVF_USER_MODE);
                doFeedback();
                this->redraw();
                m_haveScaleFactor = false;
            }
        }
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::onCoordinate(double x, double y, double z)
{
    // Update coordinate display

    m_coordText = "";
    std::stringstream coordStream(m_coordText);
    coordStream << "x = " << x << ", y = " << y << ", z = " << z;

    if (m_coordWidget!=NULL)
    {
        m_coordWidget->label(m_coordText.c_str());
        m_coordWidget->redraw();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::onDeleteShape(CIvfShape* shape, bool &doit)
{
    // Handle shape deletion

    disableHint();

    if ( (m_deleteFilter==DF_ALL)||(m_deleteFilter==DF_NODES))
    {
        if (shape->isClass("CIvfFemNode"))
        {
            CIvfFemNode* ivfNode = (CIvfFemNode*) shape;

            if (m_beamModel->getNodeSet()->removeNode(ivfNode->getFemNode()))
                doit = true;
            else
                doit = false;
        }
    }


    if ( (m_deleteFilter==DF_ALL)||(m_deleteFilter==DF_ELEMENTS))
    {
        if (shape->isClass("CIvfFemBeam"))
        {
            CIvfFemBeam* ivfBeam = (CIvfFemBeam*) shape;
            CFemBeam* femBeam = ivfBeam->getBeam();
            CFemBeamSet* beamSet = m_beamModel->getElementSet();

            if (beamSet->removeElement(femBeam))
                doit = true;
            else
                doit = false;
        }
    }

    initiateHint();

    m_needRecalc = doit;
}

// ------------------------------------------------------------
void CIvfFemWidget::onMove(CIvfComposite *selectedShapes, double &dx, double &dy, double &dz, bool &doit)
{
    doit = true;
    m_needRecalc = true;
}

// ------------------------------------------------------------
void CIvfFemWidget::onOverlay()
{
#ifdef ADVANCED_GL
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    //glLineWidth(2.0);

    // Render areas

    m_objectArea->setCoord(0, 0,h()-80);
    m_objectArea->setCoord(1, w()/2,h()-80);
    m_objectArea->setCoord(2, w()/2,h());
    m_objectArea->setCoord(3, 0,h());
    m_viewArea->setCoord(0, w()/2,h()-80);
    m_viewArea->setCoord(1, w(),h()-80);
    m_viewArea->setCoord(2, w(),h());
    m_viewArea->setCoord(3, w()/2,h());

    m_objectArea->render();
    m_editArea->render();
    m_viewArea->render();

    // Render logo text

    /*
     glColor4f(0.8f,0.8f,0.0f,1.0f);
     m_logoFont->output(
     w()-m_logoFont->getWidth(OBJFRAME_VERSION_STRING)-40,
     m_logoFont->getHeight()+5,
     OBJFRAME_VERSION_STRING );
     */

    // Render coordinate text


    //glColor4f(0.5f,0.5f,0.0f,1.0f);
    if (m_coordText.length()>0)
    {
        /*
         m_coordFont->output(
         w()-m_logoFont->getWidth(OBJFRAME_VERSION_STRING)-40,
         m_coordFont->getHeight()+35,
         m_coordText );
         */
    }
    else
    {
        if (m_selectedButton!=NULL)
        {
            if (m_selectedButton->getHint()!="")
            {
                /*
                 glColor3fv(m_hintColor);
                 m_coordFont->output(
                 w()/2-m_logoFont->getWidth(m_selectedButton->getHint())/2+50,
                 h()/2-m_coordFont->getHeight()/2,
                 m_selectedButton->getHint() );
                 */
            }
        }
    }

    // Update button positions

    m_objectButtons->setPosition(0.0,h()-70,0.0);
    m_viewButtons->setPosition(w()-200.0,h()-70.0,0.0);

    // Render overlay "scene"

    m_overlayScene->render();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0);
#endif
}

// ------------------------------------------------------------
void CIvfFemWidget::onInitContext()
{
    CIvfFltkWidget::onInitContext();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.4, 0.4, 0.4, 1.0);
#ifdef ADVANCED_GL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
}

// ------------------------------------------------------------
void CIvfFemWidget::onPassiveMotion(int x, int y)
{
#ifdef ADVANCED_GL
    unsigned int i;
    bool inside = false;
    bool needInvalidate = false;

    // Only do select inside defined areas

    this->setEditEnabled(true);

    //cursorState = getScene()->getCursor()->getState();
    m_overlaySelected = false;
    for (i=0; i<m_areas.size(); i++)
    {
        if (m_areas[i]->inside(x,y))
        {
            inside = true;

            //getScene()->getCursor()->setState(IVF_OBJECT_OFF);
            this->setEditEnabled(false);
            needInvalidate = false;

            CIvfPlaneButton* oldButton = m_selectedButton;

            if (m_selectedButton!=NULL)
            {
                m_selectedButton->setHighlight(CIvfShape::HS_OFF);
                m_selectedButton->setScale(1.0, 1.0, 1.0);
                needInvalidate = true;
                m_overlaySelected = false;
            }

            m_overlayScene->setViewport(w(),h());
            m_overlayScene->pick(x, y);
            m_selectedButton = (CIvfPlaneButton*) m_overlayScene->getSelectedShape();

            if (m_selectedButton!=NULL)
            {
                m_selectedButton->setHighlight(CIvfShape::HS_ON);
                m_selectedButton->setScale(1.1, 1.1, 1.1);
                needInvalidate = true;
                m_overlaySelected = true;
                if (oldButton!=m_selectedButton)
                    initiateHint();
            }
        }
    }

    if (!inside)
    {
        m_overWorkspace = !inside;
        if (m_overWorkspace!=m_lastOverWorkspace)
            needInvalidate = true;
        m_lastOverWorkspace = m_overWorkspace;
    }

    if (needInvalidate)
    {
        redraw();
    }
#endif
}

// ------------------------------------------------------------
void CIvfFemWidget::onMouse(int x, int y)
{

}

// ------------------------------------------------------------
void CIvfFemWidget::onMouseDown(int x, int y)
{
    m_mouseDownPos[0] = x;
    m_mouseDownPos[1] = y;
#ifdef ADVANCED_GL
    if ((m_overlaySelected)&&(m_selectedButton!=NULL))
    {
        m_interactionNode = NULL;
        m_selectedButton->setButtonState(CIvfGenericButton::BS_PRESSED);
        this->redraw();
    }
#endif

    if ((getEditMode()==IVF_USER_MODE)&&(!m_overlaySelected))
    {
        so_print("FemWidget: onMouseDown USER_MODE.");
        if (getCurrentMouseButton()==IVF_BUTTON1)
        {
            if (m_saneModel)
                if (m_internalSolver!=NULL)
                    setRepresentation(FRAME_RESULTS);
        }
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::onMouseUp(int x, int y)
{
#ifdef ADVANCED_GL
    if ((m_overlaySelected)&&(m_selectedButton!=NULL))
    {
        this->onButton(m_selectedButton->getId(), (CIvfPlaneButton*)m_selectedButton);
        this->redraw();
    }
#endif

    m_startAlfa = m_alfa;
    m_startBeta = m_beta;

    if (getEditMode()==IVF_USER_MODE)
    {
        if (getCurrentMouseButton()==IVF_BUTTON3)
            setCustomMode(OF_FEEDBACK);
        if (getCurrentMouseButton()==IVF_BUTTON1)
        {
            if (m_saneModel)
                if (m_internalSolver!=NULL)
                {
                    m_internalSolver->update();
                    setRepresentation(FRAME_RESULTS);
                }
        }
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::onMotion(int x, int y)
{
    if ( (m_customMode==OF_FEEDBACK)&&(getCurrentMouseButton()==IVF_BUTTON1) )
    {
        double v[3];


        switch (this->getCurrentModifier()) {
        case IVF_SHIFT:
            m_alfa = m_startAlfa + (x - m_mouseDownPos[0])*M_PI/500.0;
            break;
        case IVF_CTRL:
            m_beta = m_startBeta + (y - m_mouseDownPos[1])*M_PI/500.0;
            break;
            /*
             case IVF_ALT:
             m_tactileForceValue = 100.0*sqrt(pow(x - m_mouseDownPos[0],2)+pow(y - m_mouseDownPos[1],2));
             //m_haveScaleFactor = false;
             break;
             */
        default:
            m_alfa = m_startAlfa + (x - m_mouseDownPos[0])*M_PI/500.0;
            m_beta = m_startBeta + (y - m_mouseDownPos[1])*M_PI/500.0;
            break;
        }

        v[0] = cos(m_beta)*cos(m_alfa);
        v[1] = sin(m_beta);
        v[2] = cos(m_beta)*sin(m_alfa);

        m_tactileForce->setDirection(v[0], v[1], v[2]);

        doFeedback();

        this->redraw();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::onDeSelect()
{
    m_dlgNodeProp->setNode(NULL);
    m_dlgBeamProp->setBeam(NULL);

    if (m_customMode==OF_FEEDBACK)
    {
        m_interactionNode = NULL;
        m_tactileForce->setState(CIvfShape::OS_OFF);
        this->redraw();
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::onHighlightShape(CIvfShape *shape)
{
    if (m_customMode == OF_FEEDBACK)
    {
        if ( (shape->isClass("CIvfNode"))&&(m_interactionNode==NULL) )
        {
            double x, y, z;
            m_tactileForce->setState(CIvfShape::OS_ON);
            shape->getPosition(x, y, z);
            m_tactileForce->setPosition(x, y, z);
            this->redraw();
        }
        else
        {
            if (m_interactionNode==NULL)
                m_tactileForce->setState(CIvfShape::OS_OFF);
        }
    }
}

// ------------------------------------------------------------
void CIvfFemWidget::onSelectFilter(CIvfShape *shape, bool &select)
{
    switch (m_selectFilter) {
    case SF_ALL:
        select = true;
        break;
    case SF_NODES:
        if (shape->isClass("CIvfNode"))
            select = true;
        else
            select = false;
        break;
    case SF_ELEMENTS:
        if (shape->isClass("CIvfFemBeam"))
            select = true;
        else
            select = false;
        break;
    default:
        select = true;
        break;
    }
}

// ------------------------------------------------------------
#ifdef ADVANCED_GL
void CIvfFemWidget::onButton(int objectName, CIvfPlaneButton *button)
{
    m_editButtons->clearChecked();
    m_objectButtons->clearChecked();
    m_viewButtons->clearChecked();
    switch (objectName) {
    case BTN_SELECT:
        m_editButtons->check(0);
        this->setEditMode(IVF_SELECT);
        break;
    case BTN_MOVE:
        m_editButtons->check(1);
        this->setEditMode(IVF_MOVE);
        break;
    case BTN_CREATE_NODE:
        m_objectButtons->check(0);
        this->setEditMode(IVF_CREATE_NODE);
        break;
    case BTN_CREATE_BEAM:
        m_objectButtons->check(1);
        this->setEditMode(IVF_CREATE_LINE);
        break;
    case BTN_FEEDBACK:
        m_editButtons->check(4);
        this->setCustomMode(OF_FEEDBACK);
        break;
    case BTN_VIEW_ZOOM:
        this->setEditMode(IVF_VIEW_ZOOM);
        m_viewButtons->check(0);
        break;
    case BTN_VIEW_PAN:
        this->setEditMode(IVF_VIEW_PAN);
        m_viewButtons->check(1);
        break;
    case BTN_VIEW_RESET:
        this->resetView();
        m_viewButtons->recheck();
        break;
    case BTN_DELETE:
        this->setEditEnabled(true);
        this->deleteSelected();
        this->setEditEnabled(false);
        m_editButtons->recheck();
        break;
    case BTN_INSPECT:
        this->showProperties();
        m_editButtons->recheck();
        break;
    case BTN_NODE_LOAD:
        this->showNodeLoads();
        m_objectButtons->recheck();
        break;
    case BTN_BEAM_LOAD:
        this->showBeamLoads();
        m_objectButtons->recheck();
        break;
    case BTN_MATERIALS:
        this->showMaterials();
        m_objectButtons->recheck();
        break;
    case BTN_NODE_BC:
        this->showNodeBCs();
        break;
    default:
        break;
    }
    this->redraw();
}
#endif

void CIvfFemWidget::onHighlightFilter(CIvfShape *shape, bool &highlight)
{
    switch (m_highlightFilter) {
    case HF_ALL:
        highlight = true;
        break;
    case HF_NODES:
        if (shape->isClass("CIvfNode"))
            highlight = true;
        else
            highlight = false;
        break;
    case HF_ELEMENTS:
        if (shape->isClass("CIvfFemBeam"))
            highlight = true;
        else
            highlight = false;
        break;
    default:
        highlight = true;
        break;
    }
}

void CIvfFemWidget::onKeyboard(int key)
{
    if (key == 122)
        this->setEditMode(IVF_VIEW_PAN);
}

void CIvfFemWidget::setRelNodeSize(double size)
{
    m_relNodeSize = size;
}

void CIvfFemWidget::setRelLineRadius(double radius)
{
    m_relLineRadius = radius;
}

void CIvfFemWidget::setRelLoadSize(double size)
{
    m_relLoadSize = size;
}

double CIvfFemWidget::getRelNodeSize()
{
    return m_relNodeSize;
}

double CIvfFemWidget::getRelLineRadius()
{
    return m_relLineRadius;
}

double CIvfFemWidget::getRelLoadSize()
{
    return m_relLoadSize;
}

void CIvfFemWidget::refreshToolbars()
{
#ifdef ADVANCED_GL
    m_editButtons->clearChecked();
    m_objectButtons->clearChecked();
    m_viewButtons->clearChecked();

    switch (getEditMode()) {
    case IVF_SELECT:
        m_editButtons->check(0);
        break;
    case IVF_MOVE:
        m_editButtons->check(1);
        break;
    case IVF_CREATE_LINE:
        m_objectButtons->check(1);
        break;
    case IVF_CREATE_NODE:
        m_objectButtons->check(0);
        break;
    case IVF_VIEW_ZOOM:
        m_viewButtons->check(0);
        break;
    case IVF_VIEW_PAN:
        m_viewButtons->check(1);
        break;
    default:

        break;
    }

    this->redraw();
#endif
}

void CIvfFemWidget::removeNodeLoadsFromSelected()
{
    // Remove materials from selected shapes

    CFemBeamNodeLoad* nodeLoad = this->getCurrentNodeLoad();

    if (nodeLoad!=NULL)
    {
        CIvfComposite* selected = this->getSelectedShapes();
        for (int i=0; i<selected->getSize(); i++)
        {
            CIvfShape* shape = selected->getChild(i);
            if (shape->isClass("CIvfFemNode"))
            {
                CIvfFemNode* ivfNode = (CIvfFemNode*)shape;
                CFemNode* node = ivfNode->getFemNode();
                nodeLoad->removeNode(node);
            }
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void CIvfFemWidget::removeNodeBCsFromSelected()
{
    // Remove materials from selected shapes

    CFemBeamNodeBC* nodeBC = this->getCurrentNodeBC();

    if (nodeBC!=NULL)
    {
        CIvfComposite* selected = this->getSelectedShapes();
        for (int i=0; i<selected->getSize(); i++)
        {
            CIvfShape* shape = selected->getChild(i);
            if (shape->isClass("CIvfFemNode"))
            {
                CIvfFemNode* ivfNode = (CIvfFemNode*)shape;
                CFemNode* node = ivfNode->getFemNode();
                nodeBC->removeNode(node);
            }
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void CIvfFemWidget::removeBeamLoadsFromSelected()
{
    // Remove materials from selected shapes

    CFemBeamLoad* beamLoad = this->getCurrentBeamLoad();

    if (beamLoad!=NULL)
    {
        CIvfComposite* selected = this->getSelectedShapes();
        for (int i=0; i<selected->getSize(); i++)
        {
            CIvfShape* shape = selected->getChild(i);
            if (shape->isClass("CIvfFemBeam"))
            {
                CIvfFemBeam* ivfBeam = (CIvfFemBeam*)shape;
                CFemBeam* beam = ivfBeam->getBeam();
                beamLoad->removeElement(beam);
            }
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

CFemBeamNodeBC* CIvfFemWidget::getCurrentNodeBC()
{
    return m_currentNodeBC;
}


void CIvfFemWidget::hideAllDialogs()
{
    so_print("FemWidget: Hiding all dialogs.");

    m_dlgNodeProp->hide();
    m_dlgBeamProp->hide();
    m_dlgMaterials->hide();
    m_dlgElementLoads->hide();
    m_dlgNodeLoads->hide();
    m_dlgNodeBCs->hide();

    so_hide();
}

void CIvfFemWidget::setResultType(int type)
{
    m_beamModel->setResultType(type);
    this->set_changed();
    this->redraw();
}


// ------------------------------------------------------------
void CIvfFemWidget::setProgramPath(const std::string& progPath)
{
    m_progPath = progPath;
}

// ------------------------------------------------------------
const std::string CIvfFemWidget::getProgPath()
{
    return m_progPath;
}
