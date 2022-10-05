#include "FemWidget.h"

#ifdef WIN32
#include "resource.h"
#endif

#include <filesystem>
#include <functional>
#include <sstream>

#include <chaiscript/chaiscript_stdlib.hpp>
#include <chaiscript/extras/math.hpp>

#include <FL/fl_draw.H>
#include <FL/gl.h>
#include <FL/x.H>

#include <FL/Fl_Native_File_Chooser.H>

#include <ivf/BitmapFont.h>
#include <ivf/CoordinateSystem.h>
#include <ivf/Fog.h>
#include <ivf/TextLabel.h>
#include <ivf/Texture.h>
#include <ivfimage/SgiImage.h>

#include <ofem/beam.h>
#include <ofem/beam_load.h>
#include <ofem/node.h>

#include <ofutil/util_functions.h>

#include <logger.h>

#include "script_plugin.h"

using namespace ivf;
using namespace std;
using namespace ofui;

void feedbackCallback(void* pointer)
{
    FemWidget* widget = (FemWidget*)pointer;
    widget->doFeedback();
    Fl::add_timeout(0.01f, feedbackCallback, widget);
}

// Constructor/Destructor

FemWidget::FemWidget(int X, int Y, int W, int H, const char* L)
    : FltkWidget(X, Y, W, H, L)
{
    m_width = W;
    m_height = H;
    m_tactileForce = nullptr;
    m_internalSolver = nullptr;
    m_relNodeSize = 0.004;
    m_relLineRadius = 0.0015;
    m_relLoadSize = 0.07;
    m_customMode = CustomMode::Normal;
    m_customModeSet = false;
    m_alfa = 0.0;
    m_beta = 0.0;
    m_startAlfa = 0.0;
    m_startBeta = M_PI / 2.0;
    m_haveScaleFactor = false;
    m_needRecalc = true;
    m_selectFilter = SelectMode::All;
    m_deleteFilter = DeleteMode::All;
    m_highlightFilter = HighlightMode::All;
    m_overWorkspace = true;
    m_lastOverWorkspace = true;
    m_hintFinished = true;
    m_lockScaleFactor = false;
    m_saneModel = false;
    m_selectedPos[0] = 0.0;
    m_selectedPos[1] = 0.0;
    m_selectedPos[2] = 0.0;

    m_tactileForceValue = 1000.0;

    // Initialize GUI variables

    m_coordWidget = nullptr;

    m_progPath = "";

    m_showNodeBCsWindow = false;
    m_showBCPropPopup = false;
    m_prevButton = nullptr;

    m_nodeSelection = false;
    m_elementSelection = false;
    m_mixedSelection = false;

    this->setupScripting();
    this->setupPlugins();
}

FemWidget::~FemWidget()
{
    log("Destructor.");

    // Do some cleanup

    log("Deleting internal solver.");
    delete m_internalSolver;
}

void FemWidget::runScript(const std::string filename)
{

    ScriptPlugin plugin("plugins/frame.chai");

    plugin.param("floors", "4");
    plugin.param("rows", "4");
    plugin.param("cols", "4");
    plugin.param("dx", "1.0");
    plugin.param("dy", "1.0");
    plugin.param("dz", "1.0");

    cout << plugin.source();

    chaiscript::ChaiScript::State s = m_chai.get_state(); // get initial state
    try
    {
        m_chai.eval_file(filename);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
        log(e.pretty_print());
    }

    m_chai.set_state(s);
}

void FemWidget::runPlugin(ScriptPlugin* plugin)
{
    this->snapShot();

    chaiscript::ChaiScript::State s = m_chai.get_state(); // get initial state
    try
    {
        m_pluginRunning = true;
        m_chai.eval(plugin->source());
        m_pluginRunning = false;
    }
    catch (const chaiscript::exception::eval_error& e)
    {
        log(e.pretty_print());
    }
    m_chai.set_state(s);
}

// Get/set methods

void FemWidget::setCoordWidget(Fl_Widget* widget)
{
    m_coordWidget = widget;
}

Fl_Widget* FemWidget::getCoordWidget()
{
    return m_coordWidget;
}

void FemWidget::setFileName(const std::string& name)
{
    m_fileName = name;
}

const std::string FemWidget::getFileName()
{
    // Return current filename

    return m_fileName;
}

void FemWidget::updateAxisLabels()
{
    m_textLayer->clear();

    auto axisLabelPlusX = ivf::TextLabel::create();
    axisLabelPlusX->setCamera(this->getCamera());
    axisLabelPlusX->setFont(m_redFont);
    axisLabelPlusX->setText("+X", 0.5);
    // axisLabelPlusX->setRotationQuat(1.0, 0.0, 0.0, 90.0);
    axisLabelPlusX->setAlignObject(IVF_ALIGN_CAMERA);
    axisLabelPlusX->setVector(0.0, 1.0, 0.0);
    axisLabelPlusX->setPosition(this->getWorkspace() / 2.0 + 1.0, 0.0, 0.0);
    axisLabelPlusX->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(axisLabelPlusX);

    auto axisLabelMinusX = ivf::TextLabel::create();
    axisLabelMinusX->setCamera(this->getCamera());
    axisLabelMinusX->setFont(m_redFont);
    axisLabelMinusX->setText("-X", 0.5);
    // axisLabelMinusX->setRotationQuat(1.0, 0.0, 0.0, 90.0);
    axisLabelMinusX->setAlignObject(IVF_ALIGN_CAMERA);
    axisLabelMinusX->setPosition(-this->getWorkspace() / 2.0 - 1.0, 0.0, 0.0);
    axisLabelMinusX->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(axisLabelMinusX);

    auto axisLabelPlusY = ivf::TextLabel::create();
    axisLabelPlusY->setCamera(this->getCamera());
    axisLabelPlusY->setFont(m_greenFont);
    axisLabelPlusY->setText("+Z", 0.5);
    // axisLabelPlusY->setRotationQuat(1.0, 0.0, 0.0, -90.0);
    axisLabelPlusY->setAlignObject(IVF_ALIGN_CAMERA);
    axisLabelPlusY->setPosition(0.0, 0.0, this->getWorkspace() / 2.0 + 1.0);
    axisLabelPlusY->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(axisLabelPlusY);

    auto axisLabelMinusY = ivf::TextLabel::create();
    axisLabelMinusY->setCamera(this->getCamera());
    axisLabelMinusY->setFont(m_greenFont);
    axisLabelMinusY->setText("-Z", 0.5);
    // axisLabelMinusY->setRotationQuat(1.0, 0.0, 0.0, -90.0);
    axisLabelMinusY->setAlignObject(IVF_ALIGN_CAMERA);
    axisLabelMinusY->setPosition(0.0, 0.0, -this->getWorkspace() / 2.0 - 1.0);
    axisLabelMinusY->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(axisLabelMinusY);

    auto upperLeft = ivf::TextLabel::create();
    upperLeft->setCamera(this->getCamera());
    upperLeft->setFont(m_axisFont);
    upperLeft->setText(ofutil::to_coord_string(-this->getWorkspace() / 2.0, -this->getWorkspace() / 2.0), 0.5);
    // upperLeft->setRotationQuat(1.0, 0.0, 0.0, -90.0);
    upperLeft->setAlignObject(IVF_ALIGN_CAMERA);
    upperLeft->setPosition(-this->getWorkspace() / 2.0 - 1.0, 0.0, -this->getWorkspace() / 2.0 - 1.0);
    upperLeft->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(upperLeft);

    auto upperRight = ivf::TextLabel::create();
    upperRight->setCamera(this->getCamera());
    upperRight->setFont(m_axisFont);
    upperRight->setText(ofutil::to_coord_string(this->getWorkspace() / 2.0, -this->getWorkspace() / 2.0), 0.5);
    // upperRight->setRotationQuat(1.0, 0.0, 0.0, -90.0);
    upperRight->setAlignObject(IVF_ALIGN_CAMERA);
    upperRight->setPosition(this->getWorkspace() / 2.0 + 1.0, 0.0, -this->getWorkspace() / 2.0 - 1.0);
    upperRight->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(upperRight);

    auto lowerLeft = ivf::TextLabel::create();
    lowerLeft->setCamera(this->getCamera());
    lowerLeft->setFont(m_axisFont);
    lowerLeft->setText(ofutil::to_coord_string(-this->getWorkspace() / 2.0, this->getWorkspace() / 2.0), 0.5);
    // lowerLeft->setRotationQuat(1.0, 0.0, 0.0, -90.0);
    lowerLeft->setAlignObject(IVF_ALIGN_CAMERA);
    lowerLeft->setPosition(-this->getWorkspace() / 2.0 - 1.0, 0.0, this->getWorkspace() / 2.0 + 1.0);
    lowerLeft->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(lowerLeft);

    auto lowerRight = ivf::TextLabel::create();
    lowerRight->setCamera(this->getCamera());
    lowerRight->setFont(m_axisFont);
    lowerRight->setText(ofutil::to_coord_string(this->getWorkspace() / 2.0, this->getWorkspace() / 2.0), 0.5);
    // lowerRight->setRotationQuat(1.0, 0.0, 0.0, -90.0);
    lowerRight->setAlignObject(IVF_ALIGN_CAMERA);
    lowerRight->setPosition(this->getWorkspace() / 2.0 + 1.0, 0.0, this->getWorkspace() / 2.0 + 1.0);
    lowerRight->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(lowerRight);
}

void FemWidget::setWorkspace(double size, bool resetCamera)
{
    FltkWidget::setWorkspace(size, resetCamera);

    if (m_beamModel != nullptr)
    {
        m_beamModel->setNodeSize(this->getWorkspace() * m_relNodeSize);
        m_beamModel->setNodeType(ivf::Node::NT_CUBE);
        m_beamModel->setLineRadius(this->getWorkspace() * m_relLineRadius);
        m_beamModel->setLoadSize(this->getWorkspace() * m_relLoadSize);
        m_beamModel->setBeamLoadSize(this->getWorkspace() * m_relLoadSize);
    }

    this->set_changed();

    if (resetCamera)
    {
        this->resetView();
        this->getCamera()->setPerspective(45.0, size / 50.0, size * 100.0);
    }

    if (m_tactileForce != nullptr)
    {
        double loadSize;
        if (m_beamModel != nullptr)
            loadSize = m_beamModel->getLoadSize();
        else
            loadSize = 1.0;

        m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
        m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
        m_tactileForce->setDirection(0.0, -1.0, 0.0);
        m_tactileForce->setOffset(-loadSize * 0.7);
    }
    updateAxisLabels();
}

void FemWidget::setCurrentMaterial(ofem::BeamMaterial* material)
{
    // Set current material

    m_currentMaterial = material;
}

ofem::BeamMaterial* FemWidget::getCurrentMaterial()
{
    // Return current material

    return m_currentMaterial;
}

void FemWidget::setCurrentBeamLoad(ofem::BeamLoad* elementLoad)
{
    // Set current elementload

    m_currentElementLoad = elementLoad;
}

void FemWidget::setRepresentation(RepresentationMode repr)
{
    // Change model representation
    m_representation = repr;

    switch (m_representation)
    {
    case RepresentationMode::Fem:
        log("Setting representation to FRAME_FEM.");
        ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_EDGE | TUBE_JN_ANGLE);
        m_beamModel->setBeamType(IVF_BEAM_SOLID);
        m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
        break;
    case RepresentationMode::Geometry:
        log("Setting representation to FRAME_GEOMETRY.");
        ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_FACET | TUBE_JN_ANGLE);
        m_beamModel->setBeamType(IVF_BEAM_EXTRUSION);
        m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
        break;
    case RepresentationMode::Displacements:
        log("Setting representation to FRAME_DISPLACEMENTS.");
        ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_EDGE | TUBE_JN_ANGLE);
        m_beamModel->setBeamType(IVF_BEAM_SOLID);
        m_beamModel->setNodeType(IVF_NODE_DISPLACEMENT);
        break;
    case RepresentationMode::Results:
        log("Setting representation to FRAME_RESULTS.");
        ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_EDGE | TUBE_JN_ANGLE);
        m_beamModel->setBeamType(IVF_BEAM_RESULTS);
        m_beamModel->setNodeType(IVF_NODE_DISPLACEMENT);
        break;
    default:
        log("Setting representation to DEFAULT.");
        m_beamModel->setBeamType(IVF_BEAM_SOLID);
        m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
        break;
    }

    // Shapes has to be refreshed to represent the
    // the changes

    this->set_changed();
    this->redraw();
}

ofem::BeamModel* FemWidget::getModel()
{
    return m_beamModel;
}

Shape* FemWidget::getSelectedShape()
{
    // Return currently selected shape

    return m_selectedShape;
}

ofem::BeamLoad* FemWidget::getCurrentBeamLoad()
{
    return m_currentElementLoad;
}

void FemWidget::setCurrentNodeLoad(ofem::BeamNodeLoad* nodeLoad)
{
    m_currentNodeLoad = nodeLoad;
}

ofem::BeamNodeLoad* FemWidget::getCurrentNodeLoad()
{
    return m_currentNodeLoad;
}

void FemWidget::setEditMode(WidgetMode mode)
{
    // If setEditMode is not called in response to a setCustomMode call,
    // the custom mode is set to OF_NORMAL

    // set highlight filter

    this->getScene()->disableCursor();
    this->getScene()->disableCursorShape();
    this->setUseOverlay(true);
    m_pluginWindow->hide();

    switch (mode)
    {
    case WidgetMode::Select:
        log("WidgetMode::Select");
        m_consoleWindow->clear();
        console("Select: Click on objects to select. Click outside to deselect.");
        setHighlightFilter(HighlightMode::All);
        setSelectFilter(SelectMode::All);
        setRepresentation(RepresentationMode::Fem);
        break;
    case WidgetMode::BoxSelection:
        log("WidgetMode::BoxSelect");
        m_consoleWindow->clear();
        console("Select: Click on objects to select. Click outside to deselect.");
        setHighlightFilter(HighlightMode::All);
        setSelectFilter(SelectMode::All);
        setRepresentation(RepresentationMode::Fem);
        break;
    case WidgetMode::CreateNode:
        log("WidgetMode::CreateNode");
        m_consoleWindow->clear();
        console("Create nodes: Use the cursor to create nodes. [Shift] moves up/down.");
        setHighlightFilter(HighlightMode::Nodes);
        setSelectFilter(SelectMode::Nodes);
        setRepresentation(RepresentationMode::Fem);
        break;
    case WidgetMode::CreateLine:
        log("WidgetMode::CreateLine");
        m_consoleWindow->clear();
        console("Create beams: Select 2 nodes to create a beam element.");
        setHighlightFilter(HighlightMode::Nodes);
        setSelectFilter(SelectMode::Nodes);
        setRepresentation(RepresentationMode::Fem);
        break;
    case WidgetMode::Move:
        log("WidgetMode::Move");
        m_consoleWindow->clear();
        console("Move: Move selected nodes with cursor. Click and hold mouse to move. [Shift] moves up/down.");
        break;
    default:
        setHighlightFilter(HighlightMode::All);
        setSelectFilter(SelectMode::All);
        break;
    }

    if ((m_customMode == CustomMode::Feedback) && (m_customModeSet))
    {
        m_consoleWindow->clear();
        console("Feedback mode: Click on a node to apply interactive force. Move mouse with button down to move force.");
        setHighlightFilter(HighlightMode::Nodes);
        setSelectFilter(SelectMode::Nodes);
    }

    if (!m_customModeSet)
    {
        m_tactileForce->setState(Shape::OS_OFF);
        m_interactionNode = nullptr;
        m_customMode = CustomMode::Normal;
        ;
        this->redraw();
    }
    else
        m_customModeSet = false;

    if (m_customMode == CustomMode::Feedback)
    {
        this->getScene()->getComposite()->setHighlightChildren(Shape::HS_OFF);
        this->setRepresentation(RepresentationMode::Displacements);
    }

    m_coordText = "";

    FltkWidget::setEditMode(mode);

    if (mode == WidgetMode::CreateNode)
    {
        this->getScene()->enableCursor();
        this->getScene()->enableCursorShape();
    }
    if (mode == WidgetMode::Move)
    {
        this->getScene()->enableCursor();
        this->getScene()->disableCursorShape();
    }

    this->updateButtonState();
}

void FemWidget::setBeamRefreshMode(ivf::LineRefreshMode mode)
{
    auto scene = this->getScene()->getComposite();
    int i;

    for (i = 0; i < scene->getSize(); i++)
    {
        auto shape = scene->getChild(i);
        if (shape->isClass("vfem::SolidLine"))
        {
            SolidLine* solidLine = static_cast<SolidLine*>(shape);
            solidLine->setRefresh(mode);
        }
    }
}

void FemWidget::setArguments(int argc, char** argv)
{
    m_argc = argc;
    m_argv = argv;
}

void FemWidget::setScalefactor(double scalefactor)
{
    m_beamModel->setScaleFactor(scalefactor);
    this->set_changed();
    this->redraw();
}

double FemWidget::getScalefactor()
{
    return m_beamModel->getScaleFactor();
}

void FemWidget::setCurrentNodeBC(ofem::BeamNodeBC* bc)
{
    m_currentNodeBC = bc;
}

void FemWidget::setCustomMode(CustomMode mode)
{
    if (m_customMode == CustomMode::Feedback)
    {
        m_interactionNode = nullptr;
    }

    m_customMode = mode;
    m_customModeSet = true;
    m_haveScaleFactor = false;
    this->setBeamRefreshMode(ivf::rmNodes);

    if (m_customMode == CustomMode::Feedback)
    {
        m_tactileForce->setState(Shape::OS_OFF);
        m_interactionNode = nullptr;
        this->clearSelection();
        this->redraw();
        m_beamModel->clearNodeValues();
        this->setResultType(IVF_BEAM_NO_RESULT);
        this->setEditMode(WidgetMode::Select);
    }

    if ((m_customMode == CustomMode::Structure) || (m_customMode == CustomMode::Paste))
    {
        this->setEditMode(WidgetMode::SelectPosition);
    }
    else
    {
        m_pluginWindow->hide();
    }
}

void FemWidget::setSelectFilter(SelectMode filter)
{
    m_selectFilter = filter;
}

void FemWidget::setDeleteFilter(DeleteMode filter)
{
    m_deleteFilter = filter;
}

void FemWidget::setHighlightFilter(HighlightMode filter)
{
    m_highlightFilter = filter;
}

void FemWidget::setNeedRecalc(bool flag)
{
    m_needRecalc = flag;
}

void FemWidget::setRelNodeSize(double size)
{
    m_relNodeSize = size;
}

void FemWidget::setRelLineRadius(double radius)
{
    m_relLineRadius = radius;
}

void FemWidget::setRelLoadSize(double size)
{
    m_relLoadSize = size;
}

double FemWidget::getRelNodeSize()
{
    return m_relNodeSize;
}

double FemWidget::getRelLineRadius()
{
    return m_relLineRadius;
}

double FemWidget::getRelLoadSize()
{
    return m_relLoadSize;
}

ofem::BeamNodeBC* FemWidget::getCurrentNodeBC()
{
    return m_currentNodeBC;
}

void FemWidget::setResultType(int type)
{
    m_beamModel->setResultType(type);
    this->set_changed();
    this->redraw();
}

void FemWidget::setProgramPath(const std::string& progPath)
{
    m_progPath = progPath;
}

const std::string FemWidget::getProgPath()
{
    return m_progPath;
}

// Widget methods

void FemWidget::save()
{
    // Save model

    if (m_fileName == "noname.df3")
    {
        Fl_Native_File_Chooser fnfc;
        fnfc.title("Save");
        fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
        fnfc.filter("ObjectiveFrame\t*.df3\n");
        fnfc.directory(""); // default directory to use

        int result = fnfc.show();

        if (result != 0)
            return;

        std::string filename = fnfc.filename();
        this->setFileName(filename);
    }
    m_beamModel->setFileName(m_fileName);
    m_beamModel->save();
}

void FemWidget::saveAs()
{
    // Save model

    Fl_Native_File_Chooser fnfc;
    fnfc.title("Save as");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    fnfc.filter("ObjectiveFrame\t*.df3\n");
    fnfc.directory(""); // default directory to use

    int result = fnfc.show();

    if (result == 0)
    {
        std::string filename = fnfc.filename();
        this->setFileName(filename);
        m_beamModel->setFileName(m_fileName);
        m_beamModel->save();
    }
}

void FemWidget::exportAsCalfem()
{
    // Save model

    Fl_Native_File_Chooser fnfc;
    fnfc.title("Save as");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    fnfc.filter("CALFEM for Python\t*.py\n");
    fnfc.directory(""); // default directory to use

    int result = fnfc.show();

    if (result == 0)
    {
        std::string filename = fnfc.filename();
        auto writer = new ofem::CalfemWriter(filename);
        writer->setFemModel(m_beamModel);
        writer->save();
    }
}

void FemWidget::snapShot()
{
    m_beamModel->snapShot();
    log("Current undo level: " + ofutil::to_string(m_beamModel->snapShotCount()));
}

void FemWidget::restoreLastSnapShot()
{
    auto prevEditMode = this->getEditMode();

    m_beamModel->restoreLastSnapShot();
    log("Current undo level: " + ofutil::to_string(m_beamModel->snapShotCount()));

    this->hideAllDialogs();
    this->deleteAll();

    log("Setting color map path.");
    std::string colorPath = "";

    colorPath = colorPath + m_progPath;
    colorPath = colorPath + "maps/";

    // Initialize and open beam model

    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());
    m_beamModel->setShowNodeNumbers(false);

    // Generate a Ivf++ representation

    m_beamModel->generateModel();

    // Update dialogs

    m_nodePropWindow->setNode(nullptr);
    m_elementPropWindow->setBeam(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_needRecalc = true;

    if (m_internalSolver != nullptr)
        delete m_internalSolver;

    m_internalSolver = nullptr;

    this->setEditMode(prevEditMode);
}

void FemWidget::revertLastSnapShot()
{
    auto prevEditMode = this->getEditMode();

    m_beamModel->revertLastSnapShot();
    log("Current undo level: " + ofutil::to_string(m_beamModel->snapShotCount()));

    this->hideAllDialogs();
    this->deleteAll();

    log("Setting color map path.");
    std::string colorPath = "";

    colorPath = colorPath + m_progPath;
    colorPath = colorPath + "maps/";

    // Initialize and open beam model

    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());
    m_beamModel->setShowNodeNumbers(true);

    // Generate a Ivf++ representation

    m_beamModel->generateModel();

    // Update dialogs

    m_nodePropWindow->setNode(nullptr);
    m_elementPropWindow->setBeam(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_needRecalc = true;

    if (m_internalSolver != nullptr)
        delete m_internalSolver;

    m_internalSolver = nullptr;

    this->setEditMode(prevEditMode);
}

void FemWidget::open(std::string filename)
{
    // Change filename and erase previous model/scene

    this->setFileName(filename);
    this->deleteAll();

    log("Setting color map path.");
    std::string colorPath = "";

    colorPath = colorPath + m_progPath;
    colorPath = colorPath + "maps/";

    // Initialize and open beam model

    m_beamModel->initialize();
    m_beamModel->setFileName(m_fileName);
    m_beamModel->setPath(colorPath);
    m_beamModel->open();
    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());
    m_beamModel->setShowNodeNumbers(true);

    // Generate a Ivf++ representation

    m_beamModel->generateModel();

    // Update dialogs

    m_nodePropWindow->setNode(nullptr);
    m_elementPropWindow->setBeam(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_needRecalc = true;

    if (m_internalSolver != nullptr)
        delete m_internalSolver;

    m_internalSolver = nullptr;

    this->setEditMode(WidgetMode::Select);
}

void FemWidget::open()
{
    // Open model

    this->hideAllDialogs();

    // Prompt for a filename

    Fl_Native_File_Chooser fnfc;
    fnfc.title("Open file");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fnfc.filter("ObjectiveFrame\t*.df3\n");
    fnfc.directory(""); // default directory to use

    int result = fnfc.show();

    // char* fname = fl_file_chooser("Open file", "*.df3", "");

    // If we have a filename we try to open.

    if (result == 0)
    {
        // Change filename and erase previous model/scene

        std::string filename = fnfc.filename();
        this->open(filename);
    }
}

void FemWidget::openScript()
{
    // Open model

    this->hideAllDialogs();

    // Prompt for a filename

    Fl_Native_File_Chooser fnfc;
    fnfc.title("Open file");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fnfc.filter("ObjectiveFrame\t*.chai\n");
    fnfc.directory(""); // default directory to use

    int result = fnfc.show();

    // If we have a filename we try to open.

    if (result == 0)
    {
        // Change filename and erase previous model/scene

        std::string filename = fnfc.filename();
        this->runScript(filename);
    }
}

void FemWidget::copy()
{
    auto selectedShapes = this->getSelectedShapes();

    m_modelClipBoard->clear();

    for (int i = 0; i < selectedShapes->getSize(); i++)
    {
        auto shape = selectedShapes->getChild(i);

        if (shape->isClass("vfem::Node"))
        {
            auto vnode = static_cast<vfem::Node*>(shape);
            auto node = vnode->getFemNode();
            m_modelClipBoard->addNode(node);
        }

        if (shape->isClass("vfem::Beam"))
        {
            auto vbeam = static_cast<vfem::Beam*>(shape);
            auto beam = vbeam->getBeam();
            m_modelClipBoard->addElement(beam);
        }
    }
}

void FemWidget::paste()
{
    this->setCustomMode(CustomMode::Paste);
}

void FemWidget::showProperties()
{
    // Properties for selected shape

    this->onSelect(this->getSelectedShapes());

    if (m_nodeSelection || m_singleNodeSelection)
        m_nodePropWindow->show();

    if (m_elementSelection || m_singleElementSelection)
        m_elementPropWindow->show();

    /*
    if (this->getSelectedShape() != nullptr)
    {
            if (this->getSelectedShape()->isClass("vfem::Node"))
            {
                    m_nodePropWindow->align(3);
                    m_nodePropWindow->setVisible(true);
            }

            if (this->getSelectedShape()->isClass("vfem::Beam"))
            {
                    m_elementPropWindow->setBeam(static_cast<vfem::Beam*>(this->getSelectedShape()));
                    m_elementPropWindow->setVisible(true);
            }
    }
    else
    */
    if ((!m_nodeSelection) && (!m_elementSelection))
    {
        m_consoleWindow->clear();
        console("Inspect: Please select an object(s) to inspect.");
    }

    setEditMode(WidgetMode::Select);
    this->refreshToolbars();
}

void FemWidget::showMaterials()
{
}

void FemWidget::newModel()
{
    this->hideAllDialogs();

    // Delete all Ivf++ objects

    this->deleteAll();

    // Setup color map path

    std::string colorPath = m_progPath;
    colorPath = colorPath + "maps\\";

    // Setup new beam model

    m_beamModel = new vfem::BeamModel();
    m_beamModel->initialize();
    m_beamModel->setPath(colorPath);
    m_beamModel->setScene(this->getScene()->getComposite());
    m_beamModel->setNodeSize(this->getWorkspace() * m_relNodeSize);
    m_beamModel->setNodeType(ivf::Node::NT_CUBE);
    m_beamModel->setLineRadius(this->getWorkspace() * m_relLineRadius);
    m_beamModel->setLoadSize(this->getWorkspace() * m_relLoadSize);
    m_beamModel->setBeamLoadSize(this->getWorkspace() * m_relLoadSize);
    m_beamModel->setNodeMaterial(m_nodeMaterial);
    m_beamModel->setBeamMaterial(m_lineMaterial);

    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());
    m_beamModel->setShowNodeNumbers(true);

    m_beamModel->generateModel();

    m_currentMaterial = nullptr;

    // Initialize color table

    auto colorTable = m_beamModel->getColorTable();

    uchar r, g, b;

    for (int i = 0; i < 256; i++)
    {
        Fl::get_color((Fl_Color)i, r, g, b);
        colorTable->setColor(i,
            (float)r / 255.0f,
            (float)g / 255.0f,
            (float)b / 255.0f);
    }

    // Initialize dialogs

    m_nodePropWindow->setNode(nullptr);
    m_elementPropWindow->setBeam(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_nodeCursor = Sphere::create();
    m_nodeCursor->setMaterial(m_nodeMaterial);
    m_nodeCursor->setRadius(m_beamModel->getNodeSize());
    this->getScene()->setCursorShape(m_nodeCursor);

    this->getScene()->addChild(m_tactileForce);

    m_needRecalc = true;

    if (m_internalSolver != nullptr)
        delete m_internalSolver;

    m_internalSolver = nullptr;

    this->setEditMode(WidgetMode::ViewZoom);

    // Update screen

    this->redraw();
}

void FemWidget::assignMaterialToSelected()
{
    // Assigns a material to selected shapes

    if (this->getCurrentMaterial() != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Beam"))
            {
                vfem::Beam* visBeam = static_cast<vfem::Beam*>(shape);
                visBeam->getBeam()->setMaterial(this->getCurrentMaterial());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

void FemWidget::removeMaterialFromSelected()
{
    // Remove materials from selected shapes

    auto selected = this->getSelectedShapes();
    for (int i = 0; i < selected->getSize(); i++)
    {
        auto shape = selected->getChild(i);
        if (shape->isClass("vfem::Beam"))
        {
            vfem::Beam* visBeam = static_cast<vfem::Beam*>(shape);
            visBeam->getBeam()->setMaterial(nullptr);
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemWidget::deleteBeamLoad(ofem::BeamLoad* elementLoad)
{
    // Delete a beam load

    // Get ivf representation from element load

    vfem::BeamLoad* visBeamLoad = static_cast<vfem::BeamLoad*>(elementLoad->getUser());

    // Remove shape from scene and delete it

    this->getScene()->getComposite()->removeShape(visBeamLoad);
    delete visBeamLoad;

    // Remove load from beam model

    m_needRecalc = true;
    m_beamModel->getElementLoadSet()->removeLoad(elementLoad);
}

void FemWidget::deleteSelected()
{
    // Delete things in an orderly fashion

    this->snapShot();

    m_elementPropWindow->setBeam(nullptr);
    m_nodePropWindow->setNode(nullptr);

    setDeleteFilter(DeleteMode::Elements);
    deleteSelectedKeep();
    setDeleteFilter(DeleteMode::Nodes);
    deleteSelectedKeep();
    setDeleteFilter(DeleteMode::All);

    m_beamModel->enumerate();

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemWidget::addBeamLoad(ofem::BeamLoad* elementLoad)
{
    // Add a beam load

    // Create ivf represenation

    vfem::BeamLoad* visLoad = new vfem::BeamLoad();
    visLoad->setBeamModel(m_beamModel);
    visLoad->setBeamLoad(elementLoad);

    // Set user property of element load to point to
    // our ivf representation

    elementLoad->setUser(static_cast<void*>(visLoad));

    // Initialize ivf representation

    visLoad->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(visLoad);
}

void FemWidget::addNodeLoad(ofem::BeamNodeLoad* nodeLoad)
{
    // Add a node load

    // Create ivf represenation

    vfem::NodeLoad* visNodeLoad = new vfem::NodeLoad();
    visNodeLoad->setBeamModel(m_beamModel);
    visNodeLoad->setNodeLoad(nodeLoad);

    // Set user property of element load to point to
    // our ivf representation

    nodeLoad->setUser(static_cast<void*>(visNodeLoad));

    // Initialize ivf representation

    visNodeLoad->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(visNodeLoad);
}

void FemWidget::addNodeBC(ofem::BeamNodeBC* bc)
{
    // Add a node load

    // Create ivf represenation

    vfem::NodeBC* visNodeBC = new vfem::NodeBC();
    visNodeBC->setBeamModel(m_beamModel);
    visNodeBC->setNodeBC(bc);

    // Set user property of element load to point to
    // our ivf representation

    bc->setUser(static_cast<void*>(visNodeBC));

    // Initialize ivf representation

    visNodeBC->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(visNodeBC);
}

void FemWidget::showBeamLoads()
{
}

void FemWidget::assignBeamLoadSelected()
{
    // Assign a beam load to selected beams

    if (m_currentElementLoad != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Beam"))
            {
                vfem::Beam* visBeam = static_cast<vfem::Beam*>(shape);
                m_currentElementLoad->addElement((ofem::Element*)visBeam->getBeam());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

void FemWidget::assignNodeLoadSelected()
{
    // Assign a node load to selected nodes

    if (m_currentNodeLoad != nullptr)
    {
        this->snapShot();

        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Node"))
            {
                vfem::Node* visNode = static_cast<vfem::Node*>(shape);
                m_currentNodeLoad->addNode(static_cast<ofem::Node*>(visNode->getFemNode()));
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

void FemWidget::deleteNodeLoad(ofem::BeamNodeLoad* nodeLoad)
{
    // Delete a node load

    // Get ivf representation from element load

    vfem::NodeLoad* visNodeLoad = static_cast<vfem::NodeLoad*>(nodeLoad->getUser());

    // Remove shape from scene and delete it

    this->getScene()->getComposite()->removeShape(visNodeLoad);
    delete visNodeLoad;

    // Remove load from beam model

    m_beamModel->getNodeLoadSet()->removeLoad(nodeLoad);
    setCurrentNodeLoad(nullptr);
}

void FemWidget::deleteNodeBC(ofem::BeamNodeBC* bc)
{
    // Delete a node load

    // Get ivf representation from element load

    vfem::NodeBC* visNodeBC = static_cast<vfem::NodeBC*>(bc->getUser());

    // Remove shape from scene and delete it

    this->getScene()->getComposite()->removeShape(visNodeBC);
    delete visNodeBC;

    // Remove load from beam model

    m_needRecalc = true;
    m_beamModel->getNodeBCSet()->removeBC(bc);
    setCurrentNodeBC(nullptr);
}

void FemWidget::setRotationSelected(double rotation)
{
    // Assigns a material to selected shapes

    auto selected = this->getSelectedShapes();
    for (int i = 0; i < selected->getSize(); i++)
    {
        auto shape = selected->getChild(i);
        if (shape->isClass("vfem::Beam"))
        {
            vfem::Beam* visBeam = static_cast<vfem::Beam*>(shape);
            visBeam->getBeam()->setBeamRotation(rotation);
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    this->set_changed();
    this->redraw();
}

void FemWidget::setupOverlay()
{
    PlaneButton* button;

    m_editArea = new Area2D();
    m_editArea->add(0, 0);
    m_editArea->add(65, 0);
    m_editArea->add(65, 600);
    m_editArea->add(0, 520);
    m_editArea->setColor(0, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(1, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(2, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(3, 0.0f, 0.0f, 0.0f);
    m_areas.push_back(m_editArea);

    m_objectArea = new Area2D();
    m_objectArea->add(0, 0);
    m_objectArea->add(0, 0);
    m_objectArea->add(0, 0);
    m_objectArea->add(0, 0);
    m_objectArea->setColor(0, 0.0f, 0.0f, 0.0f);
    m_objectArea->setColor(1, 0.0f, 0.0f, 0.0f);
    m_objectArea->setColor(2, 0.0f, 0.0f, 0.0f);
    m_objectArea->setColor(3, 0.0f, 0.0f, 0.0f);
    m_areas.push_back(m_objectArea);

    // Create edit toolbar

    m_editButtons = new ButtonGroup();

    button = new PlaneButton(ToolbarButton::Select, "images/tlselect.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 60.0, 0.0);
    button->setHint("Select nodes or elements");
    m_editButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::SelectBox, "images/tlselectbox.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 120.0, 0.0);
    button->setHint("Select nodes or elements");
    m_editButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::Move, "images/tlmove.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 200.0, 0.0);
    button->setHint("Move nodes or elements");
    m_editButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::Inspect, "images/tlinspect.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 270, 0.0);
    button->setHint("Node or element info");
    m_editButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::Delete, "images/tldelete.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 360.0, 0.0);
    button->setHint("Delete node or element");
    m_editButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::Feedback, "images/tlfeedback.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 440.0, 0.0);
    button->setHint("Feedback mode");
    m_editButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::Run, "images/run.png");
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 520.0, 0.0);
    button->setHint("Excecute calculation");
    m_editButtons->addChild(button);

    m_overlayScene->addChild(m_editButtons);

    //
    // Create object toolbar
    //

    m_objectButtons = new ButtonGroup();

    button = new PlaneButton(ToolbarButton::CreateNode, "images/tlnode.png");
    button->setSize(50.0, 50.0);
    button->setPosition(30.0, 30.0, 0.0);
    button->setHint("Create node");
    m_objectButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::CreateBeam, "images/tlsolidline.png");
    button->setSize(50.0, 50.0);
    button->setPosition(90.0, 30.0, 0.0);
    button->setHint("Create element");
    m_objectButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::NodeLoad, "images/tlnodeloads.png");
    button->setSize(50.0, 50.0);
    button->setPosition(150.0, 30.0, 0.0);
    button->setHint("Show node loads");
    m_objectButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::BeamLoad, "images/tldload.png");
    button->setSize(50.0, 50.0);
    button->setPosition(210.0, 30.0, 0.0);
    button->setHint("Show element loads");
    m_objectButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::NodeBC, "images/tlbc.png");
    button->setSize(50.0, 50.0);
    button->setPosition(270.0, 30.0, 0.0);
    button->setHint("Show boundary conditions");
    m_objectButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::Materials, "images/tlmaterials.png");
    button->setSize(50.0, 50.0);
    button->setPosition(330.0, 30.0, 0.0);
    button->setHint("Show beam properties");
    m_objectButtons->addChild(button);

    m_logoButton = new PlaneButton(1234, "images/logo.png");
    m_logoButton->setSize(120.0, 120.0);

    m_overlayScene->addChild(m_logoButton);

    m_overlayScene->addChild(m_objectButtons);
}

void FemWidget::setupScripting()
{
    // Add math library to script environment

    auto mathlib = chaiscript::extras::math::bootstrap();
    m_chai.add(mathlib);

    // Bind ObjectiveFrame specific functions

    m_chai.add(chaiscript::fun(&FemWidget::addNode, this), "addNode");
    m_chai.add(chaiscript::fun(&FemWidget::newModel, this), "newModel");
    m_chai.add(chaiscript::fun(&FemWidget::addBeam, this), "addBeam");
    m_chai.add(chaiscript::fun(&FemWidget::nodeCount, this), "nodeCount");
}

void FemWidget::setupPlugins()
{
    std::string path = "plugins";

    if (std::filesystem::is_directory(path))
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            auto filename = entry.path();
            auto plugin = ScriptPlugin::create(filename.string());
            m_plugins.push_back(plugin);
            log("Loading plugin - " + filename.string() + " - " + plugin->name());
        }
    }
    else
        log("Could find load any plugins...");
}

void FemWidget::assignNodeBCSelected()
{
    // Assign a node load to selected nodes

    if (m_currentNodeBC != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Node"))
            {
                vfem::Node* visNode = static_cast<vfem::Node*>(shape);
                m_currentNodeBC->addNode((ofem::Node*)visNode->getFemNode());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

void FemWidget::assignNodeFixedBCSelected()
{
    this->snapShot();
    this->setCurrentNodeBC(m_beamModel->defaultNodeFixedBC());
    this->assignNodeBCSelected();
    this->setCurrentNodeBC(nullptr);
}

void FemWidget::assignNodePosBCSelected()
{
    this->snapShot();
    this->setCurrentNodeBC(m_beamModel->defaultNodePosBC());
    this->assignNodeBCSelected();
    this->setCurrentNodeBC(nullptr);
}

void FemWidget::assignNodeFixedBCGround()
{
    this->snapShot();
    this->clearSelection();
    this->setSelectFilter(SelectMode::GroundNodes);
    this->selectAll();
    this->assignNodeFixedBCSelected();
}

void FemWidget::assignNodePosBCGround()
{
    this->snapShot();
    this->clearSelection();
    this->setSelectFilter(SelectMode::GroundNodes);
    this->selectAll();
    this->assignNodePosBCSelected();
}

void FemWidget::executeCalc()
{
    double maxNodeValue;

    if (m_internalSolver != nullptr)
        delete m_internalSolver;

    m_internalSolver = new FrameSolver();
    m_internalSolver->setBeamModel(m_beamModel);
    m_internalSolver->setResultInfo(m_beamModel->getResultInfo());
    m_internalSolver->execute();
    if (m_internalSolver->getLastError() != BS_NO_ERROR)
    {
        switch (m_internalSolver->getLastError())
        {
        case BS_NO_NODES:
            this->showMessage("No nodes defined. \nCalculation not executed.");
            break;
        case BS_NO_ELEMENTS:
            this->showMessage("No elements defined. \nCalculation not executed.");
            break;
        case BS_NO_BC:
            this->showMessage("No boundary conditions defined. \nCalculation not executed.");
            break;
        case BS_NO_LOADS:
            this->showMessage("No loads defined. \nCalculation not executed.");
            break;
        case BS_UNSTABLE:
            this->showMessage("System unstable. Try adding boundary conditions.\nCalculation not executed.");
            break;
        case BS_SINGULAR:
            this->showMessage("System is singular. Check for free nodes or other strange things. \nCalculation not executed.");
            break;
        case BS_INVALID_MODEL:
            this->showMessage("This should not happen.\nCalculation not executed.");
            break;
        case BS_UNDEFINED_MATERIAL:
            this->showMessage("Beams without materials found.");
            break;
        default:
            this->showMessage("Unhandled error.\nCalculation not executed.");
            break;
        }
        m_needRecalc = true;
    }
    else
        m_needRecalc = false;

    maxNodeValue = m_internalSolver->getMaxNodeValue();

    // Calculate default scalefactor

    // log("Max node value = " << maxNodeValue);

    if (!m_lockScaleFactor)
    {
        if (maxNodeValue > 0.0)
            m_beamModel->setScaleFactor(this->getWorkspace() * 0.020 / maxNodeValue);
        else
            m_beamModel->setScaleFactor(1.0);
    }

    m_settingsWindow->update();

    // log("Scale factor = " << m_scalefactor);

    // Show displacements

    this->setRepresentation(RepresentationMode::Results);
}

void FemWidget::selectAllNodes()
{
    setSelectFilter(SelectMode::Nodes);
    selectAll();
}

void FemWidget::selectAllElements()
{
    setSelectFilter(SelectMode::Elements);
    selectAll();
}

void FemWidget::doFeedback()
{
    // Is there a calculation ?

    if (m_needRecalc)
    {
        if (m_interactionNode != nullptr)
        {
            /*
            CFeedbackDlg* dlg = new CFeedbackDlg();
            dlg->show();
            Fl::wait();
            */

            double maxNodeValue = 0.0;

            if (m_internalSolver != nullptr)
                delete m_internalSolver;

            m_internalSolver = new FrameSolver();
            m_internalSolver->setResultInfo(m_beamModel->getResultInfo());
            m_internalSolver->setBeamModel(m_beamModel);

            double v[3];

            // Setup feedback force

            m_tactileForce->getDirection(v[0], v[1], v[2]);
            m_internalSolver->setFeedbackForce(
                m_interactionNode->getFemNode(),
                m_tactileForceValue * v[0], m_tactileForceValue * v[1], m_tactileForceValue * v[2]);

            m_internalSolver->execute();

            // We assume the worst case

            m_saneModel = false;

            switch (m_internalSolver->getLastError())
            {
            case BS_NO_NODES:
                this->showMessage("No nodes defined. \nCalculation not executed.");
                break;
            case BS_NO_ELEMENTS:
                this->showMessage("No elements defined. \nCalculation not executed.");
                break;
            case BS_NO_BC:
                this->showMessage("No boundary conditions defined. \nCalculation not executed.");
                break;
            case BS_NO_LOADS:
                this->showMessage("No loads defined. \nCalculation not executed.");
                break;
            case BS_UNSTABLE:
                this->showMessage("System unstable. Try adding boundary conditions.\nCalculation not executed.");
                break;
            case BS_SINGULAR:
                this->showMessage("System is singular. Check for free nodes or other strange things. \nCalculation not executed.");
                break;
            case BS_INVALID_MODEL:
                this->showMessage("This should not happen.\nCalculation not executed.");
                break;
            case BS_UNDEFINED_MATERIAL:
                this->showMessage("Beams without materials found.");
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
                    m_beamModel->setScaleFactor(this->getWorkspace() * 0.020 / m_internalSolver->getMaxNodeValue());
                    m_haveScaleFactor = true;
                }
            }

            m_needRecalc = false;

            // dlg->hide();
            // delete dlg;

            // Show displacements

            this->setRepresentation(RepresentationMode::Results);
        }
    }

    // Continuosly recompute solution

    if (m_saneModel)
    {
        if (m_internalSolver != nullptr)
        {
            // Fl::check();
            if (m_interactionNode != nullptr)
            {

                double v[3];

                // Setup feedback force

                m_tactileForce->getDirection(v[0], v[1], v[2]);
                m_internalSolver->setFeedbackForce(
                    m_interactionNode->getFemNode(),
                    m_tactileForceValue * v[0], m_tactileForceValue * v[1], m_tactileForceValue * v[2]);

                // Execute calculation

                m_internalSolver->recompute();
                m_internalSolver->update(); // NEW

                // Only compute the scale factor at the first attempt

                if (!m_lockScaleFactor)
                {
                    if (!m_haveScaleFactor)
                    {
                        m_beamModel->setScaleFactor(this->getWorkspace() * 0.020 / m_internalSolver->getMaxNodeValue());
                        m_haveScaleFactor = true;
                    }
                }

                // Update tactile force icon

                double x, y, z;
                m_interactionNode->getDisplacedPosition(x, y, z);
                m_tactileForce->setPosition(x, y, z);

                // Refresh scene (Solid lines must be updated)

                this->getScene()->getComposite()->refresh();

                this->redraw(); // set damage(FL_DAMAGE_ALL)
                Fl::flush();
                // Fl::check();
            }
        }
    }
    else
    {
        setEditMode(WidgetMode::Select);
        refreshToolbars();
    }
}

vfem::Node* FemWidget::addNode(double x, double y, double z)
{
    // First we create a FemNode

    ofem::Node* femNode = new ofem::Node();

    // Add it to the Fem model

    m_beamModel->getNodeSet()->addNode(femNode);
    femNode->setNumber(static_cast<long>(m_beamModel->getNodeSet()->getSize()) - 1);

    // Create Ivf representation

    vfem::Node* ivfNode = new vfem::Node();
    ivfNode->setBeamModel(m_beamModel);
    ivfNode->setFemNode(femNode);
    ivfNode->setPosition(x + m_selectedPos[0], y + m_selectedPos[1], z + m_selectedPos[2]);
    ivfNode->setMaterial(m_nodeMaterial);
    ivfNode->setDirectRefresh(true);
    ivfNode->nodeLabel()->setSize(float(m_beamModel->getNodeSize() * 1.5));

    femNode->setUser(static_cast<void*>(ivfNode));

    this->getScene()->addChild(ivfNode);
    return ivfNode;
}

vfem::Beam* FemWidget::addBeam(int i0, int i1)
{
    auto nodeSet = m_beamModel->getNodeSet();
    auto beamSet = m_beamModel->getElementSet();

    auto n0 = nodeSet->getNode(i0);
    auto n1 = nodeSet->getNode(i1);

    if ((n0 != nullptr) && (n1 != nullptr) && (n0 != n1))
    {
        ofem::Beam* beam = new ofem::Beam();
        beam->addNode(n0);
        beam->addNode(n1);
        beam->setMaterial((ofem::BeamMaterial*)m_beamModel->getMaterialSet()->currentMaterial());

        beamSet->addElement(beam);

        auto vNode0 = static_cast<vfem::Node*>(n0->getUser());
        auto vNode1 = static_cast<vfem::Node*>(n1->getUser());

        vfem::Beam* vBeam = new vfem::Beam();
        vBeam->setBeamModel(m_beamModel);
        vBeam->setBeam(beam);
        beam->setUser(static_cast<void*>(vBeam));

        // Initialize the representation

        vBeam->setNodes(vNode0, vNode1);
        vBeam->refresh();

        // We need a recalc

        m_needRecalc = true;

        this->addToScene(vBeam);

        return vBeam;
    }
    else
        return nullptr;
}

size_t FemWidget::nodeCount()
{
    return m_beamModel->getNodeSet()->getSize();
}

void FemWidget::showMessage(std::string message)
{
    m_messagePopup->setMessage(message);
    m_messagePopup->show();
}

#ifdef USE_LEAP
LeapInteraction* CIvfFemWidget::getLeapInteraction()
{
    return m_leapinteraction;
}

void CIvfFemWidget::updateLeapFrame(Frame leapFrame)
{
    m_leapinteraction->updateLeapFrame(leapFrame);
}
#endif

ExtrArrowPtr FemWidget::getTactileForce()
{
    return m_tactileForce;
}

void FemWidget::setTactileForce(ExtrArrowPtr force)
{
    m_tactileForce = force;
}

vfem::NodePtr FemWidget::getInteractionNode()
{
    return m_interactionNode;
}

void FemWidget::setInteractionNode(vfem::Node* interactionNode)
{
    m_interactionNode = interactionNode;
}

//
// void FemWidget::showStructureDlg()
//{
//	int size[3];
//	double spacing[3];
//
//	m_dlgStructure->show();
//	if (m_dlgStructure->getModalResult() == MR_OK)
//	{
//		m_dlgStructure->getSize(size[0], size[1], size[2]);
//		m_dlgStructure->getSpacing(spacing[0], spacing[1], spacing[2]);
//
//		CStructureFactory* factory = new CStructureFactory();
//		factory->setBeamModel(m_beamModel);
//		factory->setCurrentMaterial(this->getCurrentMaterial());
//		factory->setSize(size[0], size[1], size[2]);
//		factory->setSpacing(spacing[0], spacing[1], spacing[2]);
//		factory->create();
//		delete factory;
//
//		this->redraw();
//	}
// }

void FemWidget::lockScaleFactor()
{
    m_lockScaleFactor = true;
}

void FemWidget::unlockScaleFactor()
{
    m_lockScaleFactor = false;
}

void FemWidget::refreshToolbars()
{
    m_editButtons->clearChecked();
    m_objectButtons->clearChecked();
    // m_viewButtons->clearChecked();

    switch (getEditMode())
    {
    case WidgetMode::Select:
        m_editButtons->check(0);
        break;
    case WidgetMode::Move:
        m_editButtons->check(1);
        break;
    case WidgetMode::CreateLine:
        m_objectButtons->check(1);
        break;
    case WidgetMode::CreateNode:
        m_objectButtons->check(0);
        break;
    case WidgetMode::ViewZoom:
        // m_viewButtons->check(0);
        break;
    case WidgetMode::ViewPan:
        // m_viewButtons->check(1);
        break;
    default:

        break;
    }

    this->redraw();
}

void FemWidget::removeNodeLoadsFromSelected()
{
    // Remove materials from selected shapes

    ofem::BeamNodeLoad* nodeLoad = this->getCurrentNodeLoad();

    if (nodeLoad != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Node"))
            {
                vfem::Node* visNode = static_cast<vfem::Node*>(shape);
                ofem::Node* node = visNode->getFemNode();
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

void FemWidget::removeNodesFromNodeLoad()
{
    ofem::BeamNodeLoad* nodeLoad = this->getCurrentNodeLoad();

    if (nodeLoad != nullptr)
        nodeLoad->clearNodes();

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemWidget::removeNodeBCsFromSelected()
{
    // Remove materials from selected shapes

    ofem::BeamNodeBC* nodeBC = this->getCurrentNodeBC();

    if (nodeBC != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Node"))
            {
                vfem::Node* visNode = static_cast<vfem::Node*>(shape);
                ofem::Node* node = visNode->getFemNode();
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

void FemWidget::removeBCsFromBC()
{
    ofem::BeamNodeBC* nodeBC = this->getCurrentNodeBC();

    if (nodeBC != nullptr)
        nodeBC->clearNodes();

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemWidget::removeBeamLoadsFromSelected()
{
    // Remove materials from selected shapes

    ofem::BeamLoad* beamLoad = this->getCurrentBeamLoad();

    if (beamLoad != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Beam"))
            {
                vfem::Beam* visBeam = static_cast<vfem::Beam*>(shape);
                ofem::Beam* beam = visBeam->getBeam();
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

void FemWidget::hideAllDialogs()
{
    log("Hiding all dialogs.");

    m_nodeLoadsWindow->hide();
    m_nodeBCsWindow->hide();
    m_elementLoadsWindow->hide();
    m_materialsWindow->hide();
}

// Widget events

void FemWidget::onInit()
{
    // Create log window early as it will be called by the logger.

    m_logWindow = LogWindow::create("Log window");
    m_logWindow->setVisible(false);

    m_consoleWindow = ConsoleWindow::create("Hints");
    m_consoleWindow->setVisible(true);
    m_consoleWindow->centerBottom();

    using std::placeholders::_1;
    LoggerMessageFunc f = std::bind(&FemWidget::onMessage, this, _1);
    Logger::instance()->assignOnMessageShort(f);

    // Display version information.

    log(OBJFRAME_VERSION_STRING);
    log(OBJFRAME_RELEASE);
    log(OBJFRAME_COPYRIGHT_STRING);
    log(OBJFRAME_AUTHOR1);
    log(OBJFRAME_AUTHOR2);
    log("---------------------------------------------");

    console("This window will display helpful hints on how to use the different tools in ObjectiveFrame.");

    log("Initializing FemWidget.");

    this->setUseOverlay(true);
    this->setUseUnderlay(true);

    // Intialize transparent workspace plane

    auto material = ivf::Material::create();
    material->setDiffuseColor(1.0f, 1.0f, 1.0f, 0.8f);
    material->setSpecularColor(1.0f, 1.0f, 1.0f, 0.8f);
    material->setAmbientColor(0.3f, 0.3f, 0.3f, 0.8f);

    this->getScene()->getCurrentPlane()->getCursor()->setThickness(0.02);
    this->getScene()->getCurrentPlane()->getGrid()->setUseAxis(true);
    this->getScene()->getCurrentPlane()->getGrid()->setUseCorners(true);
    this->getScene()->getCurrentPlane()->getGrid()->setUseSurface(false);
    this->getScene()->getCurrentPlane()->getGrid()->setUseOutline(true);
    this->getScene()->getCurrentPlane()->getGrid()->setMajorColor(0.2f, 0.2f, 0.4f, 1.0f);
    this->getScene()->getCurrentPlane()->getGrid()->setMinorColor(0.3f, 0.3f, 0.5f, 1.0f);
    this->getScene()->getCurrentPlane()->getGrid()->setOutlineColor(0.2f, 0.2f, 0.4f, 1.0f);
    this->getScene()->getCurrentPlane()->getGrid()->setCornerColor(0.2f, 0.2f, 0.4f, 1.0f);
    this->getScene()->setRenderFlatShadow(true);
    this->getScene()->setShadowColor(0.2f, 0.2f, 0.4f);
    this->getScene()->setShadowPrePost(false, false);

    // Label rendering setup

    if (std::filesystem::is_directory("fonts"))
    {
        m_labelFont = ivf::BitmapFont::create("fonts/white_font.fnt");
        m_axisFont = ivf::BitmapFont::create("fonts/black_font.fnt");
        m_greenFont = ivf::BitmapFont::create("fonts/green_font.fnt");
        m_redFont = ivf::BitmapFont::create("fonts/red_font.fnt");
    }
    else
    {
        log("No font directory found.");
        this->disableRedrawTimer();
        this->quit();
    }

    m_textLayer = ivf::Composite::create();
    this->getScene()->getPostComposite()->addChild(m_textLayer);

    // Common 3D gui state variables

    log("Initializing variables.");
    this->setWorkspace(20.0);

    // Initialize Ivf++ variables

    m_selectedShape = nullptr;
    m_selectedButton = nullptr;

    // Initialize scene

    log("Initializing scene.");

    // Define node material

    log("Defining node material.");
    m_nodeMaterial = ivf::Material::create();
    m_nodeMaterial->addReference();
    m_nodeMaterial->setDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
    m_nodeMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_nodeMaterial->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);

    // Define line material

    log("Defining line material.");
    m_lineMaterial = ivf::Material::create();
    m_lineMaterial->addReference();
    m_lineMaterial->setDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
    m_lineMaterial->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
    m_lineMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Initialize beam model

    log("Setting color map path.");
    std::string colorPath = m_progPath;
    colorPath = colorPath + "maps/";

    if (!std::filesystem::is_directory(colorPath))
    {
        this->disableRedrawTimer();
        this->quit();
    }

    log("Initializing beam model.");
    m_beamModel = new vfem::BeamModel();
    m_beamModel->initialize();
    m_beamModel->setPath(colorPath);
    m_beamModel->setScene(this->getScene()->getComposite());
    m_beamModel->setNodeSize(this->getWorkspace() * m_relNodeSize);
    m_beamModel->setNodeType(ivf::Node::NT_CUBE);
    m_beamModel->setLineRadius(this->getWorkspace() * m_relLineRadius);
    m_beamModel->setLoadSize(this->getWorkspace() * m_relLoadSize);
    m_beamModel->setBeamLoadSize(this->getWorkspace() * m_relLoadSize);
    m_beamModel->setNodeMaterial(m_nodeMaterial);
    m_beamModel->setBeamMaterial(m_lineMaterial);

    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());
    m_beamModel->setShowNodeNumbers(false);

    m_beamModel->generateModel();

    // Initialise model clipboard

    m_modelClipBoard = new ofem::ModelClipBoard();

    using namespace std::placeholders;
    ofem::ModelClipboardCreateNodeFunc onCreateNode = std::bind(&FemWidget::onClipboardCreateNode, this, _1, _2, _3);
    m_modelClipBoard->assignOnCreateNode(onCreateNode);

    ofem::ModelClipboardCreateElementFunc onCreateElement = std::bind(&FemWidget::onClipboardCreateElement, this, _1, _2);
    m_modelClipBoard->assignOnCreateElement(onCreateElement);

    // m_modelClipBoard->assignOnCreateElement(this->onClipboardCreateElement());

    // Initialize color table

    log("Initializing color table.");
    auto colorTable = m_beamModel->getColorTable();
    uchar r, g, b;

    for (int i = 0; i < 256; i++)
    {
        Fl::get_color((Fl_Color)i, r, g, b);
        colorTable->setColor(i,
            (float)r / 255.0f,
            (float)g / 255.0f,
            (float)b / 255.0f);
    }

    // Initialize gle library

    log("Initializing gle library.");
    ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_EDGE | TUBE_JN_ANGLE);

    // Initialize model file name variables

    log("Setting initial file name.");
    m_fileName = "";
    this->setFileName("noname.df3");

    // Overlay management

    m_overlaySelected = false;

    m_overlayScene = SelectOrtho::create();
    m_overlayScene->setViewport(m_width, m_height);
    m_overlayScene->setUseCustomTransform(false);
    this->setUseOverlay(true);
    this->setupOverlay();

    m_coordText = "";

    // Create tactile Force icon

    log("Setting material for tactile force.");
    material = ivf::Material::create();
    material->setDiffuseColor(1.0f, 1.0f, 0.0f, 1.0f);
    material->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    material->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);

    log("Creating tactile force.");

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce = ExtrArrow::create();
    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);
    m_tactileForce->setMaterial(material);
    m_tactileForce->addReference();
    m_tactileForce->setState(Shape::OS_OFF);
    this->getScene()->addChild(m_tactileForce);

    m_nodeCursor = Sphere::create();
    m_nodeCursor->setMaterial(m_nodeMaterial);
    m_nodeCursor->setRadius(m_beamModel->getNodeSize());
    this->getScene()->setCursorShape(m_nodeCursor);

    // Create ImGui interface

    m_showStyleEditor = false;
    m_showMetricsWindow = false;
    m_showNewFileDlg = false;
    m_coordWindow = CoordWindow::create("Coord window");

    m_nodePropWindow = NodePropWindow::create("Node properties");
    m_nodePropWindow->setWidget(this);
    m_nodePropWindow->setVisible(false);

    m_newModelPopup = NewModelPopup::create("Workspace", true);
    m_messagePopup = MessagePopup::create("Message", true);

    m_nodeBCsWindow = NodeBCsWindow::create("Node BCs");
    m_nodeBCsWindow->setFemWidget(this);
    m_nodeBCsWindow->setVisible(false);

    m_bcPropPopup = BCPropPopup::create("Node BC", true);

    m_nodeLoadsWindow = NodeLoadsWindow::create("Node Loads");
    m_nodeLoadsWindow->setFemWidget(this);
    m_nodeLoadsWindow->setVisible(false);

    m_settingsWindow = SettingsWindow::create("Settings");
    m_settingsWindow->setFemWidget(this);
    m_settingsWindow->setVisible(false);

    // m_nodeLoadPropPopup = NodeLoadPropPopup::create("Node Load", true);

    m_elementLoadsWindow = ElementLoadsWindow::create("Element Loads");
    m_elementLoadsWindow->setFemWidget(this);
    m_elementLoadsWindow->setVisible(false);

    m_materialsWindow = MaterialsWindow::create("Materials");
    m_materialsWindow->setFemWidget(this);
    m_materialsWindow->setVisible(false);

    m_elementPropWindow = ElementPropWindow::create("Element properties");
    m_elementPropWindow->setWidget(this);
    m_elementPropWindow->setVisible(false);

    m_pluginWindow = PluginPropWindow::create("Plugin properties");
    m_pluginWindow->setWidget(this);
    m_pluginWindow->setVisible(false);

    // Set initial edit mode

    log("Setting initial edit mode.");
    this->setEditMode(WidgetMode::Select);
    // this->setEditMode(WidgetMode::SelectVolume);

    if (m_argc > 1)
    {
        // We have command line arguments
        log("Loading from command line:" + to_string(m_argv[1]));
        this->open(to_string(m_argv[1]));
    }
}

void FemWidget::doMouse(int x, int y)
{
    if (!m_overlaySelected)
        FltkWidget::doMouse(x, y);
}

void FemWidget::onCreateNode(double x, double y, double z, ivf::Node*& newNode)
{
    // Create a node

    this->snapShot();

    // First we create a FemNode

    ofem::Node* femNode = new ofem::Node();

    // Add it to the Fem model

    m_beamModel->getNodeSet()->addNode(femNode);
    femNode->setNumber(static_cast<long>(m_beamModel->getNodeSet()->getSize()));

    // Create Ivf representation

    vfem::Node* ivfNode = new vfem::Node();
    ivfNode->setBeamModel(m_beamModel);
    ivfNode->setFemNode(femNode);
    ivfNode->setPosition(x, y, z);
    ivfNode->setMaterial(m_nodeMaterial);
    ivfNode->nodeLabel()->setSize(float(m_beamModel->getNodeSize() * 1.5));
    ivfNode->setDirectRefresh(true);

    // We need a recalc

    m_needRecalc = true;

    newNode = ivfNode;
}

void FemWidget::onCreateLine(ivf::Node* node1, ivf::Node* node2, Shape*& newLine)
{
    if (node1 == node2)
        return;

    this->snapShot();

    // Create visual representation

    vfem::Beam* visBeam = new vfem::Beam();
    visBeam->setBeamModel(m_beamModel);

    // Create model representation

    ofem::Beam* femBeam = new ofem::Beam();

    // Extract FemNode:s from the IvfNodes

    vfem::Node* visNode1 = static_cast<vfem::Node*>(node1);
    vfem::Node* visNode2 = static_cast<vfem::Node*>(node2);

    // Add FemNodes to beam element

    femBeam->addNode(visNode1->getFemNode());
    femBeam->addNode(visNode2->getFemNode());

    // Set the material

    femBeam->setMaterial((ofem::BeamMaterial*)m_beamModel->getMaterialSet()->currentMaterial());
    /*
    femBeam->setMaterial(
            m_dlgMaterials->getCurrentMaterial());
    */

    // Add beam element to beam model

    m_beamModel->getElementSet()->addElement(femBeam);

    // Initialize the representation

    visBeam->setBeam(femBeam);
    visBeam->setNodes(static_cast<vfem::Node*>(node1), static_cast<vfem::Node*>(node2));
    visBeam->refresh();

    // We need a recalc

    m_needRecalc = true;

    // Return the finished object

    newLine = static_cast<Shape*>(visBeam);
}

void FemWidget::onSelect(Composite* selectedShapes)
{
    // Handle object selection

    if (m_customMode == CustomMode::Normal)
    {
        // Disable all dialogs

        m_nodeSelection = false;
        m_elementSelection = false;
        m_mixedSelection = false;
        m_singleNodeSelection = false;
        m_singleElementSelection = false;

        m_nodePropWindow->setNode(nullptr);
        m_nodePropWindow->setSelectedShapes(nullptr);

        m_elementPropWindow->setBeam(nullptr);

        // Update dialogs with new selection

        if (selectedShapes->getSize() > 0)
        {
            if (selectedShapes->getSize() == 1)
            {
                auto firstShape = selectedShapes->getChild(0);
                m_selectedShape = firstShape;
                if (firstShape->isClass("vfem::Node"))
                {
                    // m_dlgNodeProp->setNode(static_cast<VisFemNode*>(firstShape));
                    m_nodePropWindow->setNode(static_cast<vfem::Node*>(firstShape));
                    m_singleNodeSelection = true;
                }
                if (firstShape->isClass("vfem::Beam"))
                {
                    m_elementPropWindow->setSelectedShapes(nullptr);
                    m_elementPropWindow->setBeam(static_cast<vfem::Beam*>(firstShape));

                    m_singleElementSelection = true;
                }
            }

            if (selectedShapes->getSize() > 1)
            {
                for (auto i = 0; i < selectedShapes->getSize(); i++)
                {
                    if (selectedShapes->getChild(i)->isClass("vfem::Node"))
                        m_nodeSelection = true;
                    if (selectedShapes->getChild(i)->isClass("vfem::Beam"))
                        m_elementSelection = true;
                }

                m_nodePropWindow->setSelectedShapes(selectedShapes);
                m_elementPropWindow->setSelectedShapes(selectedShapes);
                m_elementPropWindow->setBeam(nullptr);

                m_mixedSelection = m_nodeSelection && m_elementSelection;
            }
        }
    }
    else if (m_customMode == CustomMode::Feedback)
    {
        if (selectedShapes->getSize() > 0)
        {
            auto shape = selectedShapes->getChild(0);
            if (shape->isClass("vfem::Node"))
            {
                vfem::Node* visNode = static_cast<vfem::Node*>(shape);
                m_interactionNode = visNode;
                clearSelection();
                m_customModeSet = true;
                setEditMode(WidgetMode::User);
                doFeedback();
                this->redraw();
                m_haveScaleFactor = false;
            }
        }
    }
}

bool FemWidget::onInsideVolume(ivf::Shape* shape)
{
    if (shape->isClass("vfem::Beam"))
    {
        auto beam = static_cast<vfem::Beam*>(shape);
        auto n0 = beam->getNode(0);
        auto n1 = beam->getNode(1);

        return (this->isInsideVolume(n0)) && (this->isInsideVolume(n1));
    }
    else
        return false;
}

std::string FemWidget::float2str(double value)
{
    std::stringstream coordStream;
    coordStream << std::fixed << std::setw(10) << std::setprecision(2) << value;
    return coordStream.str();
}

void FemWidget::log(std::string message)
{
    Logger::instance()->log(LogLevel::Info, message);
}

void FemWidget::onMessage(std::string message)
{
    m_logWindow->log(message + "\n");
}

void FemWidget::console(std::string message)
{
    m_consoleWindow->log(message + "\n");
}

void FemWidget::onCoordinate(double x, double y, double z)
{
    // Update coordinate display

    m_coordText = "";

    m_xCoord = float2str(x);
    m_yCoord = float2str(y);
    m_zCoord = float2str(z);

    if (m_coordWidget != nullptr)
    {
        m_coordWidget->label(m_coordText.c_str());
        m_coordWidget->redraw();
    }

    m_coordWindow->setCoord(x, y, z);
}

void FemWidget::onDeleteShape(Shape* shape, bool& doit)
{
    // Handle shape deletion

    if ((m_deleteFilter == DeleteMode::All) || (m_deleteFilter == DeleteMode::Nodes))
    {
        if (shape->isClass("vfem::Node"))
        {
            vfem::Node* visNode = static_cast<vfem::Node*>(shape);

            if (m_beamModel->getNodeSet()->removeNode(visNode->getFemNode()))
            {
                doit = true;
            }
            else
                doit = false;
        }
    }

    if ((m_deleteFilter == DeleteMode::All) || (m_deleteFilter == DeleteMode::Elements))
    {
        if (shape->isClass("vfem::Beam"))
        {
            vfem::Beam* visBeam = static_cast<vfem::Beam*>(shape);
            ofem::Beam* femBeam = visBeam->getBeam();
            ofem::BeamSet* beamSet = m_beamModel->getElementSet();

            if (beamSet->removeElement(femBeam))
            {
                doit = true;
            }
            else
                doit = false;
        }
    }

    m_needRecalc = doit;
}

void FemWidget::onMove(Composite* selectedShapes, double& dx, double& dy, double& dz, bool& doit)
{
    doit = true;
    m_needRecalc = true;
}

void FemWidget::onMoveCompleted()
{
}

void FemWidget::onUnderlay()
{
    glBegin(GL_QUADS);
    glColor4f(0.6f, 0.6f, 0.7f, 1.0f);
    glVertex2i(0, 0);
    glColor4f(0.2f, 0.2f, 0.4f, 1.0f);
    glVertex2f(0.0f, float(pixel_h()));
    glColor4f(0.2f, 0.2f, 0.4f, 1.0f);
    glVertex2f(float(pixel_w()), float(pixel_h()));
    glColor4f(0.6f, 0.6f, 0.7f, 1.0f);
    glVertex2f(float(pixel_w()), 0.0f);
    glEnd();
}

void FemWidget::onOverlay()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    // Render areas

    m_objectArea->setCoord(0, 0, pixel_h() - 80);
    m_objectArea->setCoord(1, pixel_w() / 2, pixel_h() - 80);
    m_objectArea->setCoord(2, pixel_w() / 2, pixel_h());
    m_objectArea->setCoord(3, 0, pixel_h());

    m_objectArea->render();
    m_editArea->render();

    // Update button positions

    m_objectButtons->setPosition(0.0, pixel_h() - 70, 0.0);
    m_logoButton->setPosition(pixel_w() - 80, pixel_h() - 80, 0.0);

    // Render overlay "scene"

    m_overlayScene->render();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0);
    glEnable(GL_DEPTH_TEST);
}

void FemWidget::onInitContext()
{
    FltkWidget::onInitContext();
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_LINE_SMOOTH);
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (false)
    {
        Fog::getInstance()->enable();
        Fog::getInstance()->setType(Fog::FT_LINEAR);
        Fog::getInstance()->setColor(0.4f, 0.4f, 0.4f, 1.0f);
        Fog::getInstance()->setLimits(this->getWorkspace() * 0.3, this->getWorkspace() * 2.0);
    }
}

void FemWidget::onPassiveMotion(int x, int y)
{
    unsigned int i;
    bool inside = false;
    bool needInvalidate = false;

    // Only do select inside defined areas

    this->setEditEnabled(true);

    // cursorState = getScene()->getCursor()->getState();
    m_overlaySelected = false;
    for (i = 0; i < m_areas.size(); i++)
    {
        if (m_areas[i]->inside(x, y))
        {
            inside = true;

            // getScene()->getCursor()->setState(IVF_OBJECT_OFF);
            this->setEditEnabled(false);
            needInvalidate = false;

            // PlaneButton* oldButton = m_selectedButton;

            if (m_selectedButton != nullptr)
            {
                // m_selectedButton->setHighlight(Shape::HS_OFF);
                m_selectedButton->setScale(1.0, 1.0, 1.0);
                needInvalidate = true;
                m_overlaySelected = false;
            }

            m_overlayScene->setViewport(pixel_w(), pixel_h());
            m_overlayScene->pick(x, y);
            m_selectedButton = (PlaneButton*)m_overlayScene->getSelectedShape();

            if (m_selectedButton != nullptr)
            {
                // m_selectedButton->setHighlight(Shape::HS_ON);
                m_selectedButton->setScale(1.1, 1.1, 1.1);
                needInvalidate = true;
                m_overlaySelected = true;
                if (m_selectedButton != m_prevButton)
                {
                    this->onOverButton(m_selectedButton->getId(), m_selectedButton);
                    m_prevButton = m_selectedButton;
                }
            }
        }
    }

    if (!inside)
    {
        m_overWorkspace = !inside;
        if (m_overWorkspace != m_lastOverWorkspace)
            needInvalidate = true;
        m_lastOverWorkspace = m_overWorkspace;
    }
    else
        m_overWorkspace = !inside;

    if (needInvalidate)
    {
        redraw();
    }
}

void FemWidget::onMouse(int x, int y)
{
}

void FemWidget::onMouseDown(int x, int y)
{
    m_mouseDownPos[0] = x;
    m_mouseDownPos[1] = y;
    if ((m_overlaySelected) && (m_selectedButton != nullptr))
    {
        m_interactionNode = nullptr;
        m_selectedButton->setButtonState(GenericButton::BS_PRESSED);
        this->redraw();
    }

    if ((getEditMode() == WidgetMode::User) && (!m_overlaySelected))
    {
        log("onMouseDown USER_MODE.");
        if (getCurrentMouseButton() == ButtonState::bsButton1)
        {
            if (m_saneModel)
                if (m_internalSolver != nullptr)
                    setRepresentation(RepresentationMode::Results);
        }
    }
}

void FemWidget::onMouseUp(int x, int y)
{
    if ((m_overlaySelected) && (m_selectedButton != nullptr))
    {
        this->onButton(m_selectedButton->getId(), (PlaneButton*)m_selectedButton);
        this->redraw();
    }

    m_startAlfa = m_alfa;
    m_startBeta = m_beta;

    if (getEditMode() == WidgetMode::User)
    {
        // if (getCurrentMouseButton() == IVF_BUTTON3)
        //	setCustomMode(OF_FEEDBACK);

        if (getCurrentMouseButton() == ButtonState::bsButton1)
        {
            if (m_saneModel)
                if (m_internalSolver != nullptr)
                {
                    m_internalSolver->update();
                    setRepresentation(RepresentationMode::Results);
                }
        }
    }
}

void FemWidget::onMotion(int x, int y)
{
    if ((m_customMode == CustomMode::Feedback) && (getCurrentMouseButton() == ButtonState::bsButton1))
    {
        double v[3];

        switch (this->getCurrentModifier())
        {
        case ButtonState::bsShift:
            m_alfa = m_startAlfa + (x - m_mouseDownPos[0]) * M_PI / 500.0;
            break;
        case ButtonState::bsCtrl:
            m_beta = m_startBeta + (y - m_mouseDownPos[1]) * M_PI / 500.0;
            break;
            /*
             case IVF_ALT:
             m_tactileForceValue = 100.0*sqrt(pow(x - m_mouseDownPos[0],2)+pow(y - m_mouseDownPos[1],2));
             //m_haveScaleFactor = false;
             break;
             */
        default:
            m_alfa = m_startAlfa + (x - m_mouseDownPos[0]) * M_PI / 500.0;
            m_beta = m_startBeta + (y - m_mouseDownPos[1]) * M_PI / 500.0;
            break;
        }

        v[0] = cos(m_beta) * cos(m_alfa);
        v[1] = sin(m_beta);
        v[2] = cos(m_beta) * sin(m_alfa);

        m_tactileForce->setDirection(v[0], v[1], v[2]);

        doFeedback();

        this->redraw();
    }
}

void FemWidget::onDeSelect()
{
    log("onDeSelect");
    m_nodePropWindow->setNode(nullptr);
    m_nodePropWindow->setSelectedShapes(nullptr);
    m_elementPropWindow->setBeam(nullptr);

    if (m_customMode == CustomMode::Feedback)
    {
        m_interactionNode = nullptr;
        m_tactileForce->setState(Shape::OS_OFF);
        this->redraw();
    }
}

void FemWidget::onHighlightShape(Shape* shape)
{
    if (m_customMode == CustomMode::Feedback)
    {
        if ((shape->isClass("vfem::Node")) && (m_interactionNode == nullptr))
        {
            double x, y, z;
            m_tactileForce->setState(Shape::OS_ON);
            shape->getPosition(x, y, z);
            m_tactileForce->setPosition(x, y, z);
            this->redraw();
        }
        else
        {
            if (m_interactionNode == nullptr)
                m_tactileForce->setState(Shape::OS_OFF);
        }
    }
}

void FemWidget::onSelectFilter(Shape* shape, bool& select)
{
    switch (m_selectFilter)
    {
    case SelectMode::All:
        select = true;
        break;
    case SelectMode::Nodes:
        if (shape->isClass("vfem::Node"))
            select = true;
        else
            select = false;
        break;
    case SelectMode::Elements:
        if (shape->isClass("vfem::Beam"))
            select = true;
        else
            select = false;
        break;
    case SelectMode::GroundNodes:
        if (shape->isClass("vfem::Node"))
        {
            vfem::Node* visNode = static_cast<vfem::Node*>(shape);

            double x, y, z;
            visNode->getPosition(x, y, z);
            if ((y > -0.00001) && (y < 0.00001))
                select = true;
            else
                select = false;
        }
        else
            select = false;
        break;
    default:
        select = true;
        break;
    }
}

void FemWidget::onSelectPosition(double x, double y, double z)
{
    if (m_customMode == CustomMode::Structure)
    {
        log(ofutil::to_coord_string(x, y, z));

        m_selectedPos[0] = x;
        m_selectedPos[1] = y;
        m_selectedPos[2] = z;

        this->runPlugin(m_pluginWindow->plugin());
    }

    if (m_customMode == CustomMode::Paste)
    {
        m_modelClipBoard->setOffset(x, 0.0, z);
        m_modelClipBoard->paste(m_beamModel);
    }
}

void FemWidget::onMoveStart()
{
    this->snapShot();
}

void FemWidget::updateButtonState()
{
    m_editButtons->clearChecked();
    m_objectButtons->clearChecked();

    switch (this->getEditMode())
    {
    case WidgetMode::Select:
        m_editButtons->check(0);
        break;
    case WidgetMode::BoxSelection:
        m_editButtons->check(1);
        break;
    case WidgetMode::Move:
        m_editButtons->check(2);
        break;
    case WidgetMode::CreateNode:
        m_objectButtons->check(0);
        break;
    case WidgetMode::CreateLine:
        m_objectButtons->check(1);
        break;
    default:
        break;
    }

    if ((m_customMode == CustomMode::Feedback))
    {
        m_editButtons->clearChecked();
        m_editButtons->check(5);
    }
}

void FemWidget::onButton(int objectName, PlaneButton* button)
{
    m_editButtons->clearChecked();
    m_objectButtons->clearChecked();

    switch (objectName)
    {
    case ToolbarButton::Select:
        // m_editButtons->check(0);
        this->setEditMode(WidgetMode::Select);
        break;
    case ToolbarButton::SelectBox:
        // m_editButtons->check(1);
        this->setEditMode(WidgetMode::BoxSelection);
        break;
    case ToolbarButton::Move:
        // m_editButtons->check(2);
        this->setEditMode(WidgetMode::Move);
        break;
    case ToolbarButton::CreateNode:
        // m_objectButtons->check(0);
        this->setEditMode(WidgetMode::CreateNode);
        break;
    case ToolbarButton::CreateBeam:
        // m_objectButtons->check(1);
        this->setEditMode(WidgetMode::CreateLine);
        break;
    case ToolbarButton::Feedback:
        // m_editButtons->check(5);
        this->setCustomMode(CustomMode::Feedback);
        break;
    case ToolbarButton::Run:
        // m_editButtons->check(5);
        this->executeCalc();
        break;
    case ToolbarButton::ViewZoom:
        this->setEditMode(WidgetMode::ViewZoom);
        break;
    case ToolbarButton::ViewPan:
        this->setEditMode(WidgetMode::ViewPan);
        break;
    case ToolbarButton::ViewReset:
        this->resetView();
        break;
    case ToolbarButton::Delete:
        this->setEditEnabled(true);
        this->deleteSelected();
        this->setEditEnabled(false);
        m_editButtons->recheck();
        break;
    case ToolbarButton::Inspect:
        this->showProperties();
        m_editButtons->recheck();
        break;
    case ToolbarButton::NodeLoad:
        m_nodeLoadsWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet*)m_beamModel->getNodeLoadSet());
        m_nodeLoadsWindow->setVisible(true);
        this->setNeedRecalc(true);
        break;
    case ToolbarButton::BeamLoad:
        m_elementLoadsWindow->setFemLoadSet((ofem::BeamLoadSet*)m_beamModel->getElementLoadSet());
        m_elementLoadsWindow->setVisible(true);
        this->setNeedRecalc(true);
        // this->showBeamLoads();
        // m_objectButtons->recheck();
        break;
    case ToolbarButton::Materials:
        m_materialsWindow->setFemMaterialSet((ofem::BeamMaterialSet*)m_beamModel->getMaterialSet());
        m_materialsWindow->setVisible(true);
        this->setNeedRecalc(true);
        // this->showMaterials();
        // m_objectButtons->recheck();
        break;
    case ToolbarButton::NodeBC:
        m_nodeBCsWindow->setFemNodeBCSet((ofem::BeamNodeBCSet*)m_beamModel->getNodeBCSet());
        m_nodeBCsWindow->setVisible(true);
        this->setNeedRecalc(true);
        break;
    default:
        break;
    }

    this->redraw();
}

void FemWidget::onOverButton(int objectName, PlaneButton* button)
{
    m_consoleWindow->clear();
    switch (objectName)
    {
    case ToolbarButton::Select:
        console("Select nodes and beams.");
        break;
    case ToolbarButton::Move:
        console("Move nodes.");
        break;
    case ToolbarButton::CreateNode:
        console("Create nodes");
        break;
    case ToolbarButton::CreateBeam:
        console("Create beams.");
        break;
    case ToolbarButton::Feedback:
        console("Interact with model using an movable force.");
        break;
    case ToolbarButton::Run:
        console("Execute calculation");
        break;
    case ToolbarButton::ViewZoom:
        break;
    case ToolbarButton::ViewPan:
        break;
    case ToolbarButton::ViewReset:
        break;
    case ToolbarButton::Delete:
        console("Delete nodes and beams.");
        break;
    case ToolbarButton::Inspect:
        console("Show object properties.");
        break;
    case ToolbarButton::NodeLoad:
        console("Create node loads.");
        break;
    case ToolbarButton::BeamLoad:
        console("Create element loads.");
        break;
    case ToolbarButton::Materials:
        console("Create materials.");
        break;
    case ToolbarButton::NodeBC:
        console("Create node boundary conditions.");
        break;
    default:
        break;
    }
}

void FemWidget::onShortcut(ModifierKey modifier, int key)
{
    if (key == FL_Delete)
        this->deleteSelected();

    if (key == FL_Escape)
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_editButtons->check(0);
        this->setEditMode(WidgetMode::Select);
        this->redraw();
    }

    if ((modifier == ModifierKey::mkCtrl) && (key == 'o'))
        this->open();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'n'))
        this->newModel();

    if ((modifier == ModifierKey::mkCtrl) && (key == 's'))
        this->save();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'a'))
        this->selectAll();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'c'))
        this->copy();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'v'))
        this->paste();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'z'))
        this->restoreLastSnapShot();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'y'))
        this->revertLastSnapShot();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'd'))
        m_showMetricsWindow = !m_showMetricsWindow;

    if ((modifier == ModifierKey::mkCtrl) && (key == 'r'))
        this->executeCalc();

    if ((modifier == ModifierKey::mkAlt) && (key == 's'))
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_editButtons->check(0);
        this->setEditMode(WidgetMode::Select);
        this->redraw();
    }

    if ((modifier == ModifierKey::mkAlt) && (key == 'm'))
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_editButtons->check(1);
        this->setEditMode(WidgetMode::Move);
        this->redraw();
    }

    if ((modifier == ModifierKey::mkAlt) && (key == 'n'))
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_objectButtons->check(0);
        this->setEditMode(WidgetMode::CreateNode);
        this->redraw();
    }

    if ((modifier == ModifierKey::mkAlt) && (key == 'l'))
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_objectButtons->check(1);
        this->setEditMode(WidgetMode::CreateLine);
        this->redraw();
    }
}

void FemWidget::onHighlightFilter(Shape* shape, bool& highlight)
{
    switch (m_highlightFilter)
    {
    case HighlightMode::All:
        highlight = true;
        break;
    case HighlightMode::Nodes:
        if (shape->isClass("vfem::Node"))
            highlight = true;
        else
            highlight = false;
        break;
    case HighlightMode::Elements:
        if (shape->isClass("vfem::Beam"))
            highlight = true;
        else
            highlight = false;
        break;
    default:
        highlight = true;
        break;
    }
}

void FemWidget::onKeyboard(int key)
{
    // if (key == 122)
    //     this->setEditMode(WidgetMode::ViewPan);

    if (key == ImGuiKey_Delete)
        this->deleteSelected();

    /*

    case ToolbarButton::Select:
            m_editButtons->check(0);
            this->setEditMode(WidgetMode::Select);
            break;
    case ToolbarButton::Move:
            m_editButtons->check(1);
            this->setEditMode(WidgetMode::Move);
            break;
    case ToolbarButton::CreateNode:
            m_editButtons->check(0);
            this->setEditMode(WidgetMode::CreateNode);
            break;
    case ToolbarButton::CreateBeam:
            m_objectButtons->check(1);
            this->setEditMode(WidgetMode::CreateLine);
            break;
    case ToolbarButton::Feedback:
            m_editButtons->check(4);
            this->setCustomMode(CustomMode::Feedback);

    */
}

void FemWidget::onClipboardCreateNode(double x, double y, double z)
{
    log("CB: Create node x = " + to_string(x) + ", " + to_string(y) + ", " + to_string(z));

    this->addNode(x, y, z);
}

void FemWidget::onClipboardCreateElement(int i0, int i1)
{
    log("CB: Create element i0 = " + to_string(i0) + ", " + to_string(i1));

    this->addBeam(i0, i1);
}

void FemWidget::onDrawImGui()
{
    bool openDialog = false;
    bool saveDialog = false;
    bool saveAsDialog = false;
    bool executeCalc = false;
    bool quitApplication = false;
    bool exportAsCalfem = false;
    bool runScriptDialog = false;
    bool snapShot = false;
    bool restoreLastSnapShot = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "CTRL+N"))
            {
                m_showNewFileDlg = true;
                m_newModelPopup->nodeSize(float(m_relNodeSize * 100.0f));
                m_newModelPopup->loadSize(float(m_relLoadSize * 100.0f));
                m_newModelPopup->lineRadius(float(m_relLineRadius * 100.0f));
                m_newModelPopup->modelSize(float(this->getWorkspace()));
                m_newModelPopup->show();
            }

            if (ImGui::MenuItem("Open", "CTRL+O"))
                openDialog = true;

            if (ImGui::MenuItem("Save", "Ctrl+S"))
                saveDialog = true;

            if (ImGui::MenuItem("Save as", "Ctrl+Shift+S"))
                saveAsDialog = true;

            ImGui::Separator();

            if (ImGui::MenuItem("Save as CALFEM...", ""))
                exportAsCalfem = true;

            ImGui::Separator();

            if (ImGui::MenuItem("Execute chai-script...", ""))
                runScriptDialog = true;

            ImGui::Separator();

            if (ImGui::MenuItem("Quit", "Alt+F4"))
            {
                quitApplication = true;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl-Z"))
                this->restoreLastSnapShot();

            if (ImGui::MenuItem("Redo", "Ctrl-Y"))
                this->restoreLastSnapShot();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", ""))
                this->copy();
            if (ImGui::MenuItem("Paste", ""))
                this->paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all nodes", ""))
                this->selectAllNodes();
            if (ImGui::MenuItem("Select all elements", ""))
                this->selectAllElements();

            ImGui::Separator();

            if (ImGui::MenuItem("Fix selected nodes", ""))
                this->assignNodeFixedBCSelected();
            if (ImGui::MenuItem("Fix position selected nodes", ""))
                this->assignNodePosBCSelected();

            ImGui::Separator();

            if (ImGui::MenuItem("Fix ground nodes", ""))
                this->assignNodeFixedBCGround();
            if (ImGui::MenuItem("Fix position ground nodes", ""))
                this->assignNodePosBCGround();

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Node properties...", ""))
            {
                m_nodePropWindow->align(3);
                m_nodePropWindow->setVisible(true);
            }

            if (ImGui::MenuItem("Element properties...", ""))
            {
                m_elementPropWindow->align(3);
                m_elementPropWindow->setVisible(true);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Settings...", ""))
            {
                m_settingsWindow->show();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Hints...", ""))
            {
                m_consoleWindow->show();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Log...", ""))
            {
                m_logWindow->show();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Mode"))
        {
            if (ImGui::MenuItem("Model", ""))
                this->setRepresentation(RepresentationMode::Fem);
            if (ImGui::MenuItem("Geometry", ""))
                this->setRepresentation(RepresentationMode::Geometry);
            if (ImGui::MenuItem("Results", ""))
                this->setRepresentation(RepresentationMode::Results);
            if (ImGui::MenuItem("Feedback", ""))
                this->setCustomMode(CustomMode::Feedback);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Structure"))
        {
            for (auto& p : m_plugins)
            {
                if (ImGui::MenuItem(p->name().c_str(), ""))
                {
                    this->setCustomMode(CustomMode::Structure);
                    m_pluginWindow->setPlugin(p.get());
                    m_pluginWindow->show();
                }
            }
            ImGui::EndMenu();
        }
        /*
        if (ImGui::BeginMenu("Calculation"))
        {
            if (ImGui::MenuItem("Execute", ""))
                executeCalc = true;

            ImGui::EndMenu();
        }
        */
        if (ImGui::BeginMenu("Results"))
        {
            if (ImGui::MenuItem("Normal", ""))
                this->setResultType(IVF_BEAM_N);
            if (ImGui::MenuItem("Torsion", ""))
                this->setResultType(IVF_BEAM_T);
            if (ImGui::MenuItem("Shear", ""))
                this->setResultType(IVF_BEAM_V);
            if (ImGui::MenuItem("Moment", ""))
                this->setResultType(IVF_BEAM_M);
            if (ImGui::MenuItem("Navier", ""))
                this->setResultType(IVF_BEAM_NAVIER);
            if (ImGui::MenuItem("No results", ""))
                this->setResultType(IVF_BEAM_NO_RESULT);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About...", ""))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    m_newModelPopup->draw();

    if (m_newModelPopup->closed())
    {
        if (m_newModelPopup->modalResult() == PopupResult::OK)
        {
            m_relNodeSize = m_newModelPopup->nodeSize() / 100.0;
            m_relLineRadius = m_newModelPopup->lineRadius() / 100.0;
            m_relLoadSize = m_newModelPopup->loadSize() / 100.0;
            this->setWorkspace(m_newModelPopup->modelSize());
            this->newModel();
        }
        else if (m_newModelPopup->modalResult() == PopupResult::CANCEL)
        {
            log("Cancel pressed");
        }
    }

    m_messagePopup->draw();

    if (m_messagePopup->closed())
    {
    }

    if (m_showStyleEditor)
        ImGui::ShowStyleEditor();

    if (m_showMetricsWindow)
        ImGui::ShowMetricsWindow(&m_showMetricsWindow);

    m_coordWindow->draw();
    m_nodePropWindow->draw();
    m_nodeBCsWindow->draw();
    m_nodeLoadsWindow->draw();
    m_settingsWindow->draw();
    m_elementLoadsWindow->draw();
    m_materialsWindow->draw();
    m_elementPropWindow->draw();
    m_logWindow->draw();
    m_consoleWindow->draw();
    m_pluginWindow->draw();

    ImGui::Render();

    if (openDialog)
        this->open();

    if (saveDialog)
        this->save();

    if (saveAsDialog)
        this->saveAs();

    if (executeCalc)
        this->executeCalc();

    if (exportAsCalfem)
        this->exportAsCalfem();

    if (runScriptDialog)
        this->openScript();

    if (quitApplication)
    {
        this->disableRedrawTimer();
        this->quit();
    }
}

void FemWidget::onInitImGui()
{
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("fonts/RopaSans-Regular.ttf", 22 * pixels_per_unit());

    ImGuiStyle& style = ImGui::GetStyle();

    style.FrameRounding = 4;
    style.WindowRounding = 8;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(
        style.Colors[ImGuiCol_TitleBgActive].x,
        style.Colors[ImGuiCol_TitleBgActive].y,
        style.Colors[ImGuiCol_TitleBgActive].z,
        0.8f);

    style.ScaleAllSizes(1.0);
}

void FemWidget::onPostRender()
{
}
