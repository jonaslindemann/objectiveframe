#include "FemWidget.h"

#include <sstream>

#include <FL/x.H>
#include <FL/gl.h>
#include <FL/fl_draw.H>

#include <FL/Fl_Native_File_Chooser.H>

#include <ivf/CoordinateSystem.h>
#include <ivf/Texture.h>
#include <ivfimage/SgiImage.h>
#include <ivf/Fog.h>

#include <FemBeam.h>
#include <FemNode.h>
#include <FemBeamLoad.h>

#include "BeamPropDlg.h"
#include "ElementLoadsDlg.h"

#include "StructureFactory.h"
#include "StatusOutput.h"

using namespace ivf;
using namespace std;

// ------------------------------------------------------------
// ------------------------------------------------------------
// IvfFemWidget feedback callback
// ------------------------------------------------------------
// ------------------------------------------------------------


void feedbackCallback(void* pointer)
{
	FemWidget* widget = (FemWidget*)pointer;
	widget->doFeedback();
	Fl::add_timeout(0.01f, feedbackCallback, widget);
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// IvfFemWidget constructor
// ------------------------------------------------------------
// ------------------------------------------------------------

FemWidget::FemWidget(int X, int Y, int W, int H, const char* L) :
	FltkWidget(X, Y, W, H, L)
{
	m_width = W;
	m_height = H;
	so_print("FemWidget: Initializing variables.");
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
	m_startBeta = M_PI / 2.0;;
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

	m_tactileForceValue = 1000.0;

	// Initialize GUI variables

	m_coordWidget = nullptr;

	m_progPath = "";

	m_showNodeBCsWindow = false;
	m_showBCPropPopup = false;
}

void FemWidget::onInit()
{
	so_print("FemWidget: Initializing FemWidget.");

	// Intialize transparent workspace plane

	auto material = Material::create();
	material->setDiffuseColor(1.0f, 1.0f, 1.0f, 0.8f);
	material->setSpecularColor(1.0f, 1.0f, 1.0f, 0.8f);
	material->setAmbientColor(0.3f, 0.3f, 0.3f, 0.8f);

	this->getScene()->getCurrentPlane()->getCursor()->setThickness(0.02);
	this->getScene()->getCurrentPlane()->getGrid()->setUseAxis(true);
	this->getScene()->getCurrentPlane()->getGrid()->setUseCorners(true);
	this->getScene()->setRenderFlatShadow(true);
	this->getScene()->setShadowColor(0.3, 0.3, 0.3);

	// Common 3D gui state variables

	so_print("FemWidget: Initializing variables.");
	this->setWorkspace(20.0);

	// Initialize Ivf++ variables

	m_selectedShape = nullptr;
	m_selectedButton = nullptr;

	// Initialize scene

	so_print("FemWidget: Initializing scene.");

	// Define node material

	so_print("FemWidget: Defining node material.");
	m_nodeMaterial = Material::create();
	m_nodeMaterial->addReference();
	m_nodeMaterial->setDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_nodeMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_nodeMaterial->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);

	// Define line material

	so_print("FemWidget: Defining line material.");
	m_lineMaterial = Material::create();
	m_lineMaterial->addReference();
	m_lineMaterial->setDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_lineMaterial->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
	m_lineMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Initialize beam model

	so_print("FemWidget: Setting color map path.");
	std::string colorPath = m_progPath;
	colorPath = colorPath + "maps/";

	so_print("FemWidget: Initializing beam model.");
	m_beamModel = new VisBeamModel();
	m_beamModel->initialize();
	m_beamModel->setPath(colorPath);
	m_beamModel->setScene(this->getScene()->getComposite());
	m_beamModel->setNodeSize(this->getWorkspace() * m_relNodeSize);
	m_beamModel->setNodeType(Node::NT_CUBE);
	m_beamModel->setLineRadius(this->getWorkspace() * m_relLineRadius);
	m_beamModel->setLoadSize(this->getWorkspace() * m_relLoadSize);
	m_beamModel->setBeamLoadSize(this->getWorkspace() * m_relLoadSize);
	m_beamModel->setNodeMaterial(m_nodeMaterial);
	m_beamModel->setBeamMaterial(m_lineMaterial);
	m_beamModel->generateModel();


	// Initialize color table

	so_print("FemWidget: Initializing color table.");
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

	so_print("FemWidget: Initializing gle library.");
	ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_EDGE | TUBE_JN_ANGLE);

	// Initialize model file name variables

	so_print("FemWidget: Setting initial file name.");
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

	so_print("FemWidget: Setting material for tactile force.");
	material = Material::create();
	material->setDiffuseColor(1.0f, 1.0f, 0.0f, 1.0f);
	material->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	material->setAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);

	so_print("FemWidget: Creating tactile force.");

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

	so_print("DialogMgr: Creating element load dialog.");
	m_dlgElementLoads = new ElementLoadsDlg();
	m_dlgElementLoads->setFemWidget((void*)this);

	so_print("DialogMgr: Creating beam prop dialog.");
	m_dlgBeamProp = new BeamPropDlg();
	m_dlgBeamProp->setFemWidget((void*)this);

	//so_print("DialogMgr: Creating structure dialog.");
	//m_dlgStructure = new StructureDlg();

	// Set initial edit mode

	so_print("FemWidget: Setting initial edit mode.");
	this->setEditMode(WidgetMode::ViewZoom);

	// Create ImGui interface

	m_showStyleEditor = false;
	m_showMetricsWindow = false;
	m_showNewFileDlg = false;
	m_coordWindow = CoordWindow::create("Coord window");
	m_nodePropWindow = NodePropWindow::create("Node properties");
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

	//m_nodeLoadPropPopup = NodeLoadPropPopup::create("Node Load", true);

	m_elementLoadsWindow = ElementLoadsWindow::create("Element Loads");
	m_elementLoadsWindow->setFemWidget(this);
	m_elementLoadsWindow->setVisible(false);

	m_materialsWindow = MaterialsWindow::create("Materials");
	m_materialsWindow->setFemWidget(this);
	m_materialsWindow->setVisible(false);

	m_elementPropWindow = ElementPropWindow::create("Element properties");
	m_elementPropWindow->setVisible(false);
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// Ivf FemWidget destructor
// ------------------------------------------------------------
// ------------------------------------------------------------

FemWidget::~FemWidget()
{
	so_print("FemWidget: Destructor.");

	// Do some cleanup

	delete m_internalSolver;

	so_print("FemWidget: Deleting beam properties dialog.");
	delete m_dlgBeamProp;
	so_print("FemWidget: Deleting element load dialog.");
	delete m_dlgElementLoads;
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// Set/Get methods
// ------------------------------------------------------------
// ------------------------------------------------------------

// ------------------------------------------------------------

void FemWidget::setCoordWidget(Fl_Widget* widget)
{
	m_coordWidget = widget;
}

// ------------------------------------------------------------
Fl_Widget* FemWidget::getCoordWidget()
{
	return m_coordWidget;
}

// ------------------------------------------------------------
void FemWidget::setFileName(const std::string& name)
{
	m_fileName = name;
}

// ------------------------------------------------------------
const std::string FemWidget::getFileName()
{
	// Return current filename

	return m_fileName;
}

// ------------------------------------------------------------
void FemWidget::setWorkspace(double size, bool resetCamera)
{
	FltkWidget::setWorkspace(size, resetCamera);

	if (m_beamModel != nullptr)
	{
		m_beamModel->setNodeSize(this->getWorkspace() * m_relNodeSize);
		m_beamModel->setNodeType(Node::NT_CUBE);
		m_beamModel->setLineRadius(this->getWorkspace() * m_relLineRadius);
		m_beamModel->setLoadSize(this->getWorkspace() * m_relLoadSize);
		m_beamModel->setBeamLoadSize(this->getWorkspace() * m_relLoadSize);
	}

	this->set_changed();

	if (resetCamera)
	{
		this->resetView();
		this->getCamera()->setPerspective(45.0, size / 50.0, size * 4.0);
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
}

// ------------------------------------------------------------
void FemWidget::setCurrentMaterial(FemBeamMaterial* material)
{
	// Set current material

	m_currentMaterial = material;
}

// ------------------------------------------------------------
FemBeamMaterial* FemWidget::getCurrentMaterial()
{
	// Return current material

	return m_currentMaterial;
}

// ------------------------------------------------------------
void FemWidget::setCurrentBeamLoad(FemBeamLoad* elementLoad)
{
	// Set current elementload

	m_currentElementLoad = elementLoad;
}

// ------------------------------------------------------------
void FemWidget::setRepresentation(RepresentationMode repr)
{
	// Change model representation

	m_representation = repr;

	switch (m_representation) {
	case RepresentationMode::Fem:
		so_print("FemWidget: Setting representation to FRAME_FEM.");
		ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_EDGE | TUBE_JN_ANGLE);
		m_beamModel->setBeamType(IVF_BEAM_SOLID);
		m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
		break;
	case RepresentationMode::Geometry:
		so_print("FemWidget: Setting representation to FRAME_GEOMETRY.");
		ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_FACET | TUBE_JN_ANGLE);
		m_beamModel->setBeamType(IVF_BEAM_EXTRUSION);
		m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
		break;
	case RepresentationMode::Displacements:
		so_print("FemWidget: Setting representation to FRAME_DISPLACEMENTS.");
		ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_EDGE | TUBE_JN_ANGLE);
		m_beamModel->setBeamType(IVF_BEAM_SOLID);
		m_beamModel->setNodeType(IVF_NODE_DISPLACEMENT);
		break;
	case RepresentationMode::Results:
		so_print("FemWidget: Setting representation to FRAME_RESULTS.");
		ivfSetGLEJoinStyle(TUBE_JN_CAP | TUBE_NORM_EDGE | TUBE_JN_ANGLE);
		m_beamModel->setBeamType(IVF_BEAM_RESULTS);
		m_beamModel->setNodeType(IVF_NODE_DISPLACEMENT);
		break;
	default:
		m_beamModel->setBeamType(IVF_BEAM_SOLID);
		m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
		break;
	}

	// Shapes has to be refreshed to represent the
	// the changes

	this->set_changed();
	this->redraw();
}

// ------------------------------------------------------------
Shape* FemWidget::getSelectedShape()
{
	// Return currently selected shape

	return m_selectedShape;
}

// ------------------------------------------------------------
FemBeamLoad* FemWidget::getCurrentBeamLoad()
{
	return m_currentElementLoad;
}

// ------------------------------------------------------------
void FemWidget::setCurrentNodeLoad(FemBeamNodeLoad* nodeLoad)
{
	m_currentNodeLoad = nodeLoad;
}

// ------------------------------------------------------------
FemBeamNodeLoad* FemWidget::getCurrentNodeLoad()
{
	return m_currentNodeLoad;
}

// ------------------------------------------------------------
void FemWidget::setEditMode(WidgetMode mode)
{
	// If setEditMode is not called in response to a setCustomMode call,
	// the custom mode is set to OF_NORMAL

	// set highlight filter

	this->getScene()->disableCursor();
	this->getScene()->disableCursorShape();

	switch (mode) {
	case WidgetMode::Select:
		setHighlightFilter(HighlightMode::All);
		setSelectFilter(SelectMode::All);
		setRepresentation(RepresentationMode::Fem);
		break;
	case WidgetMode::CreateNode:
	case WidgetMode::CreateLine:
		setHighlightFilter(HighlightMode::Nodes);
		setSelectFilter(SelectMode::Nodes);
		setRepresentation(RepresentationMode::Fem);
		break;
	default:
		setHighlightFilter(HighlightMode::All);
		setSelectFilter(SelectMode::All);
		break;
	}

	if ((m_customMode == CustomMode::Feedback) && (m_customModeSet))
	{
		setHighlightFilter(HighlightMode::Nodes);
		setSelectFilter(SelectMode::Nodes);
	}

	if (!m_customModeSet)
	{
		m_tactileForce->setState(Shape::OS_OFF);
		m_interactionNode = nullptr;
		m_customMode = CustomMode::Normal;;
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
}


// ------------------------------------------------------------
void FemWidget::setBeamRefreshMode(int mode)
{
	auto scene = this->getScene()->getComposite();
	int i;

	for (i = 0; i < scene->getSize(); i++)
	{
		auto shape = scene->getChild(i);
		if (shape->isClass("SolidLine"))
		{
			SolidLine* solidLine = static_cast<SolidLine*>(shape);
			solidLine->setRefresh(mode);
		}
	}
}

// ------------------------------------------------------------
void FemWidget::setArguments(int argc, char** argv)
{
	m_argc = argc;
	m_argv = argv;
}

// ------------------------------------------------------------
void FemWidget::setScalefactor(double scalefactor)
{
	m_beamModel->setScaleFactor(scalefactor);
	this->set_changed();
	this->redraw();
}

// ------------------------------------------------------------
double FemWidget::getScalefactor()
{
	return m_beamModel->getScaleFactor();
}

// ------------------------------------------------------------
void FemWidget::setCurrentNodeBC(FemBeamNodeBC* bc)
{
	m_currentNodeBC = bc;
}

// ------------------------------------------------------------
void FemWidget::setCustomMode(CustomMode mode)
{
	if (m_customMode == CustomMode::Feedback)
	{
		//m_tactileForce->setState(IVF_OBJECT_ON);
		m_interactionNode = nullptr;
	}
	m_customMode = mode;
	m_customModeSet = true;
	m_haveScaleFactor = false;
	this->setBeamRefreshMode(IVF_REFRESH_NODES);
	if (m_customMode != CustomMode::Normal)
	{
		m_tactileForce->setState(Shape::OS_OFF);
		m_interactionNode = nullptr;
		this->clearSelection();
		this->redraw();
		m_beamModel->clearNodeValues();
		this->setResultType(IVF_BEAM_NO_RESULT);
		this->setEditMode(WidgetMode::Select);
	}
}

// ------------------------------------------------------------
void FemWidget::setSelectFilter(SelectMode filter)
{
	m_selectFilter = filter;
}

// ------------------------------------------------------------
void FemWidget::setDeleteFilter(DeleteMode filter)
{
	m_deleteFilter = filter;
}

// ------------------------------------------------------------
void FemWidget::setHighlightFilter(HighlightMode filter)
{
	m_highlightFilter = filter;
}

// ------------------------------------------------------------
void FemWidget::setNeedRecalc(bool flag)
{
	m_needRecalc = flag;
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// Widget methods
// ------------------------------------------------------------
// ------------------------------------------------------------

// ------------------------------------------------------------
void FemWidget::makeToolWindow(Fl_Window* window)
{
#ifdef WIN32
	HWND windowHandle = fl_xid(window);
	SetWindowLong(windowHandle, GWL_EXSTYLE, WS_EX_PALETTEWINDOW);
	SetWindowPos(windowHandle, 0, 0, 0, 0, 0, (SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_FRAMECHANGED));
#endif
}

// ------------------------------------------------------------
void FemWidget::save()
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

		if (result != 0)
			return;

		std::string filename = fnfc.filename();
		this->setFileName(filename);
	}
	m_beamModel->setFileName(m_fileName);
	m_beamModel->save();

	//cout << m_beamModel->nodeSet()->toJSON().dump(4) << endl;
	//cout << m_beamModel->nodeBCSet()->toJSON().dump(4) << endl;
}

// ------------------------------------------------------------
void FemWidget::saveAs()
{
	// Save model

	Fl_Native_File_Chooser fnfc;
	fnfc.title("Save as");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fnfc.filter("ObjectiveFrame\t*.df3\n");
	fnfc.directory("");           // default directory to use

	int result = fnfc.show();

	if (result == 0)
	{
		std::string filename = fnfc.filename();
		this->setFileName(filename);
		m_beamModel->setFileName(m_fileName);
		m_beamModel->save();
	}
}

// ------------------------------------------------------------
void FemWidget::open()
{
	// Open model

	this->hideAllDialogs();

	// Prompt for a filename

	Fl_Native_File_Chooser fnfc;
	fnfc.title("Open file");
	fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
	fnfc.filter("ObjectiveFrame\t*.df3\n");
	fnfc.directory("");           // default directory to use

	int result = fnfc.show();

	//char* fname = fl_file_chooser("Open file", "*.df3", "");

	// If we have a filename we try to open.

	if (result == 0)
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

		m_dlgBeamProp->setBeam(nullptr);
		m_nodePropWindow->setNode(nullptr);

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

		this->setEditMode(WidgetMode::ViewZoom);
	}
}


// ------------------------------------------------------------
void FemWidget::showProperties()
{
	// Properties for selected shape

	if (this->getSelectedShape() != nullptr)
	{
		if (this->getSelectedShape()->isClass("VisFemNode"))
		{
			m_nodePropWindow->align(3);
			m_nodePropWindow->setVisible(true);
		}

		if (this->getSelectedShape()->isClass("VisFemBeam"))
		{
			m_elementPropWindow->setVisible(true);
			m_dlgBeamProp->show();
			makeToolWindow(m_dlgBeamProp->wndBeamProp);
		}
	}
}

// ------------------------------------------------------------
void FemWidget::showMaterials()
{
}

// ------------------------------------------------------------
void FemWidget::newModel()
{
	this->hideAllDialogs();

	// Delete all Ivf++ objects

	this->deleteAll();

	// Setup color map path

	std::string colorPath = m_progPath;
	colorPath = colorPath + "maps\\";

	// Setup new beam model

	m_beamModel = new VisBeamModel();
	m_beamModel->initialize();
	m_beamModel->setPath(colorPath);
	m_beamModel->setScene(this->getScene()->getComposite());
	m_beamModel->setNodeSize(this->getWorkspace() * m_relNodeSize);
	m_beamModel->setNodeType(Node::NT_CUBE);
	m_beamModel->setLineRadius(this->getWorkspace() * m_relLineRadius);
	m_beamModel->setLoadSize(this->getWorkspace() * m_relLoadSize);
	m_beamModel->setBeamLoadSize(this->getWorkspace() * m_relLoadSize);
	m_beamModel->setNodeMaterial(m_nodeMaterial);
	m_beamModel->setBeamMaterial(m_lineMaterial);
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

	m_dlgBeamProp->setBeam(nullptr);
	m_nodePropWindow->setNode(nullptr);

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

// ------------------------------------------------------------
void FemWidget::assignMaterialToSelected()
{
	// Assigns a material to selected shapes

	if (this->getCurrentMaterial() != nullptr)
	{
		auto selected = this->getSelectedShapes();
		for (int i = 0; i < selected->getSize(); i++)
		{
			auto shape = selected->getChild(i);
			if (shape->isClass("VisFemBeam"))
			{
				VisFemBeam* visBeam = static_cast<VisFemBeam*>(shape);
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

// ------------------------------------------------------------
void FemWidget::removeMaterialFromSelected()
{
	// Remove materials from selected shapes

	auto selected = this->getSelectedShapes();
	for (int i = 0; i < selected->getSize(); i++)
	{
		auto shape = selected->getChild(i);
		if (shape->isClass("VisFemBeam"))
		{
			VisFemBeam* visBeam = static_cast<VisFemBeam*>(shape);
			visBeam->getBeam()->setMaterial(nullptr);
		}
	}

	// Shapes has to be refreshed to represent the
	// the changes

	m_needRecalc = true;
	this->set_changed();
	this->redraw();
}

// ------------------------------------------------------------
void FemWidget::deleteBeamLoad(FemBeamLoad* elementLoad)
{
	// Delete a beam load


	// Get ivf representation from element load

	VisFemBeamLoad* visBeamLoad = static_cast<VisFemBeamLoad*>(elementLoad->getUser());

	// Remove shape from scene and delete it

	this->getScene()->getComposite()->removeShape(visBeamLoad);
	delete visBeamLoad;

	// Remove load from beam model

	m_needRecalc = true;
	m_beamModel->getElementLoadSet()->removeLoad(elementLoad);
}

// ------------------------------------------------------------
void FemWidget::deleteSelected()
{
	// Delete things in an orderly fashion

	setDeleteFilter(DeleteMode::Elements);
	deleteSelectedKeep();
	setDeleteFilter(DeleteMode::Nodes);
	deleteSelectedKeep();
	setDeleteFilter(DeleteMode::All);
}

// ------------------------------------------------------------
void FemWidget::addBeamLoad(FemBeamLoad* elementLoad)
{
	// Add a beam load


	// Create ivf represenation

	VisFemBeamLoad* visLoad = new VisFemBeamLoad();
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

// ------------------------------------------------------------
void FemWidget::addNodeLoad(FemBeamNodeLoad* nodeLoad)
{
	// Add a node load


	// Create ivf represenation

	VisFemNodeLoad* visNodeLoad = new VisFemNodeLoad();
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

// ------------------------------------------------------------
void FemWidget::addNodeBC(FemBeamNodeBC* bc)
{
	// Add a node load


	// Create ivf represenation

	VisFemNodeBC* visNodeBC = new VisFemNodeBC();
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

// ------------------------------------------------------------
void FemWidget::showBeamLoads()
{
	// Show beam loads dialog

	setRepresentation(RepresentationMode::Fem);
	m_dlgElementLoads->setLoadSet(m_beamModel->getElementLoadSet());
	m_dlgElementLoads->show();
	makeToolWindow(m_dlgElementLoads->wndElementLoads);
}

// ------------------------------------------------------------
void FemWidget::assignBeamLoadSelected()
{
	// Assign a beam load to selected beams

	if (m_currentElementLoad != nullptr)
	{
		auto selected = this->getSelectedShapes();
		for (int i = 0; i < selected->getSize(); i++)
		{
			auto shape = selected->getChild(i);
			if (shape->isClass("VisFemBeam"))
			{
				VisFemBeam* visBeam = static_cast<VisFemBeam*>(shape);
				m_currentElementLoad->addElement((FemElement*)visBeam->getBeam());
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
void FemWidget::assignNodeLoadSelected()
{
	// Assign a node load to selected nodes

	if (m_currentNodeLoad != nullptr)
	{
		auto selected = this->getSelectedShapes();
		for (int i = 0; i < selected->getSize(); i++)
		{
			auto shape = selected->getChild(i);
			if (shape->isClass("VisFemNode"))
			{
				VisFemNode* visNode = static_cast<VisFemNode*>(shape);
				m_currentNodeLoad->addNode(static_cast<FemNode*>(visNode->getFemNode()));
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
void FemWidget::deleteNodeLoad(FemBeamNodeLoad* nodeLoad)
{
	// Delete a node load


	// Get ivf representation from element load

	VisFemNodeLoad* visNodeLoad = static_cast<VisFemNodeLoad*>(nodeLoad->getUser());

	// Remove shape from scene and delete it

	this->getScene()->getComposite()->removeShape(visNodeLoad);
	delete visNodeLoad;

	// Remove load from beam model

	m_beamModel->getNodeLoadSet()->removeLoad(nodeLoad);
	setCurrentNodeLoad(nullptr);
}

// ------------------------------------------------------------
void FemWidget::deleteNodeBC(FemBeamNodeBC* bc)
{
	// Delete a node load


	// Get ivf representation from element load

	VisFemNodeBC* visNodeBC = static_cast<VisFemNodeBC*>(bc->getUser());

	// Remove shape from scene and delete it

	this->getScene()->getComposite()->removeShape(visNodeBC);
	delete visNodeBC;

	// Remove load from beam model

	m_needRecalc = true;
	m_beamModel->getNodeBCSet()->removeBC(bc);
	setCurrentNodeBC(nullptr);
}


// ------------------------------------------------------------
//void FemWidget::showNodeLoads()
//{
//	// Show beam loads dialog
//
//	setRepresentation(FRAME_FEM);
//	m_dlgNodeLoads->setLoadSet(m_beamModel->getNodeLoadSet());
//	m_dlgNodeLoads->show();
//	makeToolWindow(m_dlgNodeLoads->wndNodeLoads);
//}

// ------------------------------------------------------------
void FemWidget::setRotationSelected(double rotation)
{
	// Assigns a material to selected shapes

	auto selected = this->getSelectedShapes();
	for (int i = 0; i < selected->getSize(); i++)
	{
		auto shape = selected->getChild(i);
		if (shape->isClass("VisFemBeam"))
		{
			VisFemBeam* visBeam = static_cast<VisFemBeam*>(shape);
			visBeam->getBeam()->setBeamRotation(rotation);
		}
	}

	// Shapes has to be refreshed to represent the
	// the changes

	this->set_changed();
	this->redraw();
}

// ------------------------------------------------------------
void FemWidget::setupOverlay()
{
#ifdef ADVANCED_GL
	CIvfPlaneButton* button;

	m_editArea = new CIvfArea2D();
	m_editArea->add(0, 0);
	m_editArea->add(65, 0);
	m_editArea->add(65, 400);
	m_editArea->add(0, 400);
	m_editArea->setColor(0, 0.0f, 0.0f, 0.0f);
	m_editArea->setColor(1, 0.0f, 0.0f, 0.0f);
	m_editArea->setColor(2, 0.0f, 0.0f, 0.0f);
	m_editArea->setColor(3, 0.0f, 0.0f, 0.0f);
	m_areas.push_back(m_editArea);

	m_objectArea = new CIvfArea2D();
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

	m_editButtons = new CIvfButtonGroup();

	button = new CIvfPlaneButton(ToolbarButton::Select, "images/tlselect.png");
	button->setSize(40.0, 40.0);
	button->setPosition(30.0, 60.0, 0.0);
	button->setHint("Select nodes or elements");
	m_editButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::Move, "images/tlmove.png");
	button->setSize(40.0, 40.0);
	button->setPosition(30.0, 140.0, 0.0);
	button->setHint("Move nodes or elements");
	m_editButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::Inspect, "images/tlinspect.png");
	button->setSize(40.0, 40.0);
	button->setPosition(30.0, 200, 0.0);
	button->setHint("Node or element info");
	m_editButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::Delete, "images/tldelete.png");
	button->setSize(40.0, 40.0);
	button->setPosition(30.0, 270.0, 0.0);
	button->setHint("Delete node or element");
	m_editButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::Feedback, "images/tlfeedback.png");
	button->setSize(40.0, 40.0);
	button->setPosition(30.0, 360.0, 0.0);
	button->setHint("Feedback mode");
	m_editButtons->addChild(button);

	m_overlayScene->addChild(m_editButtons);

	//
	// Create object toolbar
	//

	m_objectButtons = new CIvfButtonGroup();

	button = new CIvfPlaneButton(ToolbarButton::CreateNode, "images/tlnode.png");
	button->setSize(50.0, 50.0);
	button->setPosition(30.0, 30.0, 0.0);
	button->setHint("Create node");
	m_objectButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::CreateBeam, "images/tlsolidline.png");
	button->setSize(50.0, 50.0);
	button->setPosition(90.0, 30.0, 0.0);
	button->setHint("Create element");
	m_objectButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::NodeLoad, "images/tlnodeloads.png");
	button->setSize(50.0, 50.0);
	button->setPosition(150.0, 30.0, 0.0);
	button->setHint("Show node loads");
	m_objectButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::BeamLoad, "images/tldload.png");
	button->setSize(50.0, 50.0);
	button->setPosition(210.0, 30.0, 0.0);
	button->setHint("Show element loads");
	m_objectButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::NodeBC, "images/tlbc.png");
	button->setSize(50.0, 50.0);
	button->setPosition(270.0, 30.0, 0.0);
	button->setHint("Show boundary conditions");
	m_objectButtons->addChild(button);

	button = new CIvfPlaneButton(ToolbarButton::Materials, "images/tlmaterials.png");
	button->setSize(50.0, 50.0);
	button->setPosition(330.0, 30.0, 0.0);
	button->setHint("Show beam properties");
	m_objectButtons->addChild(button);

	m_overlayScene->addChild(m_objectButtons);

#endif
}

// ------------------------------------------------------------
void FemWidget::assignNodeBCSelected()
{
	// Assign a node load to selected nodes

	if (m_currentNodeBC != nullptr)
	{
		auto selected = this->getSelectedShapes();
		for (int i = 0; i < selected->getSize(); i++)
		{
			auto shape = selected->getChild(i);
			if (shape->isClass("VisFemNode"))
			{
				VisFemNode* visNode = static_cast<VisFemNode*>(shape);
				m_currentNodeBC->addNode((FemNode*)visNode->getFemNode());
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
void FemWidget::assignNodeFixedBCSelected()
{
	this->setCurrentNodeBC(m_beamModel->defaultNodeFixedBC());
	this->assignNodeBCSelected();
	this->setCurrentNodeBC(nullptr);
}

// ------------------------------------------------------------
void FemWidget::assignNodePosBCSelected()
{
	this->setCurrentNodeBC(m_beamModel->defaultNodePosBC());
	this->assignNodeBCSelected();
	this->setCurrentNodeBC(nullptr);
}

void FemWidget::assignNodeFixedBCGround()
{
	this->setSelectFilter(SelectMode::GroundNodes);
	this->selectAll();
	this->assignNodeFixedBCSelected();
}

void FemWidget::assignNodePosBCGround()
{
	this->setSelectFilter(SelectMode::GroundNodes);
	this->selectAll();
	this->assignNodePosBCSelected();
}

// ------------------------------------------------------------
//void FemWidget::showNodeBCs()
//{
//	// Show beam loads dialog
//
//	setRepresentation(FRAME_FEM);
//	m_dlgNodeBCs->setBCSet(m_beamModel->getNodeBCSet());
//	m_dlgNodeBCs->show();
//	makeToolWindow(m_dlgNodeBCs->wndNodeBCs);
//}

// ------------------------------------------------------------
void FemWidget::executeCalc()
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
	if (m_internalSolver != nullptr)
		delete m_internalSolver;

	m_internalSolver = new CFemInternalSolver();
	m_internalSolver->setBeamModel(m_beamModel);
	m_internalSolver->setResultInfo(m_beamModel->getResultInfo());
	m_internalSolver->execute();
	if (m_internalSolver->getLastError() != BS_NO_ERROR)
	{
		switch (m_internalSolver->getLastError()) {
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
#endif


	// Calculate default scalefactor

	//so_print("Max node value = " << maxNodeValue);

	if (!m_lockScaleFactor)
	{
		if (maxNodeValue > 0.0)
			m_beamModel->setScaleFactor(this->getWorkspace() * 0.020 / maxNodeValue);
		else
			m_beamModel->setScaleFactor(1.0);
	}

	m_settingsWindow->update();

	//so_print("Scale factor = " << m_scalefactor);

	// Show displacements

	this->setRepresentation(RepresentationMode::Results);

}

// ------------------------------------------------------------
//void FemWidget::showScalefactorDlg()
//{
//	m_dlgScalefactor->setScaling(m_beamModel->getScaleFactor());
//	m_dlgScalefactor->show();
//}

// ------------------------------------------------------------
void FemWidget::selectAllNodes()
{
	setSelectFilter(SelectMode::Nodes);
	selectAll();
}

// ------------------------------------------------------------
void FemWidget::selectAllElements()
{
	setSelectFilter(SelectMode::Elements);
	selectAll();
}

// ------------------------------------------------------------
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

			double maxNodeValue;

			if (m_internalSolver != nullptr)
				delete m_internalSolver;

			m_internalSolver = new CFemInternalSolver();
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

			switch (m_internalSolver->getLastError()) {
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

			//dlg->hide();
			//delete dlg;

			// Show displacements

			this->setRepresentation(RepresentationMode::Results);
		}
	}

	// Continuosly recompute solution

	if (m_saneModel)
	{
		if (m_internalSolver != nullptr)
		{
			//Fl::check();
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

				this->redraw();         // set damage(FL_DAMAGE_ALL)
				Fl::flush();
				//Fl::check();
			}
		}
	}
	else
	{
		setEditMode(WidgetMode::Select);
		refreshToolbars();
	}
}

void FemWidget::showMessage(std::string message)
{
	m_messagePopup->setMessage(message);
	m_messagePopup->show();
}

// ------------------------------------------------------------

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

VisFemNodePtr FemWidget::getInteractionNode()
{
	return m_interactionNode;
}

void FemWidget::setInteractionNode(VisFemNode* interactionNode)
{
	m_interactionNode = interactionNode;
}

//// ------------------------------------------------------------
//void FemWidget::showStructureDlg()
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
//}

// ------------------------------------------------------------
void FemWidget::lockScaleFactor()
{
	m_lockScaleFactor = true;
}

// ------------------------------------------------------------
void FemWidget::unlockScaleFactor()
{
	m_lockScaleFactor = false;
}


// ------------------------------------------------------------
// ------------------------------------------------------------
// Widget events
// ------------------------------------------------------------
// ------------------------------------------------------------

void FemWidget::doMouse(int x, int y)
{
	if (!m_overlaySelected)
		FltkWidget::doMouse(x, y);
}


// ------------------------------------------------------------
void FemWidget::onCreateNode(double x, double y, double z, Node*& newNode)
{
	// Create a node


	// First we create a FemNode

	FemNode* femNode = new FemNode();

	// Add it to the Fem model

	m_beamModel->getNodeSet()->addNode(femNode);
	femNode->setNumber(static_cast<long>(m_beamModel->getNodeSet()->getSize()) - 1);

	// Create Ivf representation

	VisFemNode* ivfNode = new VisFemNode();
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
void FemWidget::onCreateLine(Node* node1, Node* node2, Shape*& newLine)
{
	// Create visual representation

	VisFemBeam* visBeam = new VisFemBeam();
	visBeam->setBeamModel(m_beamModel);

	// Create model representation

	FemBeam* femBeam = new FemBeam();

	// Extract FemNode:s from the IvfNodes

	VisFemNode* visNode1 = static_cast<VisFemNode*>(node1);
	VisFemNode* visNode2 = static_cast<VisFemNode*>(node2);

	if (node1 == node2)
		return;

	// Add FemNodes to beam element

	femBeam->addNode(visNode1->getFemNode());
	femBeam->addNode(visNode2->getFemNode());

	// Set the material

	femBeam->setMaterial((FemBeamMaterial*)m_beamModel->getMaterialSet()->currentMaterial());
	/*
	femBeam->setMaterial(
		m_dlgMaterials->getCurrentMaterial());
	*/

	// Add beam element to beam model

	m_beamModel->getElementSet()->addElement(femBeam);

	// Initialize the representation

	visBeam->setBeam(femBeam);
	visBeam->setNodes(static_cast<VisFemNode*>(node1), static_cast<VisFemNode*>(node2));
	visBeam->refresh();

	// We need a recalc

	m_needRecalc = true;

	// Return the finished object

	newLine = static_cast<Shape*>(visBeam);
}

// ------------------------------------------------------------
void FemWidget::onSelect(Composite* selectedShapes)
{
	// Handle object selection

	if (m_customMode == CustomMode::Normal)
	{

		// Disable all dialogs

		//m_dlgNodeProp->setNode(nullptr);
		m_dlgBeamProp->setBeam(nullptr);
		m_nodePropWindow->setNode(nullptr);

		// Update dialogs with new selection

		if (selectedShapes->getSize() > 0)
		{
			auto firstShape = selectedShapes->getChild(0);
			m_selectedShape = firstShape;
			if (firstShape->isClass("VisFemNode"))
			{
				//m_dlgNodeProp->setNode(static_cast<VisFemNode*>(firstShape));
				m_nodePropWindow->setNode(static_cast<VisFemNode*>(firstShape));
			}
			if (firstShape->isClass("VisFemBeam"))
				m_dlgBeamProp->setBeam(static_cast<VisFemBeam*>(firstShape));
		}
	}
	else
	{
		if (selectedShapes->getSize() > 0)
		{
			auto shape = selectedShapes->getChild(0);
			if (shape->isClass("VisFemNode"))
			{
				VisFemNode* visNode = static_cast<VisFemNode*>(shape);
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

std::string FemWidget::float2str(double value)
{
	std::stringstream coordStream;
	coordStream << std::fixed << std::setw(10) << std::setprecision(2) << value;
	return coordStream.str();
}

// ------------------------------------------------------------
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

// ------------------------------------------------------------
void FemWidget::onDeleteShape(Shape* shape, bool& doit)
{
	// Handle shape deletion

	if ((m_deleteFilter == DeleteMode::All) || (m_deleteFilter == DeleteMode::Nodes))
	{
		if (shape->isClass("VisFemNode"))
		{
			VisFemNode* visNode = static_cast<VisFemNode*>(shape);

			if (m_beamModel->getNodeSet()->removeNode(visNode->getFemNode()))
				doit = true;
			else
				doit = false;
		}
	}


	if ((m_deleteFilter == DeleteMode::All) || (m_deleteFilter == DeleteMode::Elements))
	{
		if (shape->isClass("VisFemBeam"))
		{
			VisFemBeam* visBeam = static_cast<VisFemBeam*>(shape);
			FemBeam* femBeam = visBeam->getBeam();
			FemBeamSet* beamSet = m_beamModel->getElementSet();

			if (beamSet->removeElement(femBeam))
				doit = true;
			else
				doit = false;
		}
	}

	m_needRecalc = doit;
}

// ------------------------------------------------------------
void FemWidget::onMove(Composite* selectedShapes, double& dx, double& dy, double& dz, bool& doit)
{
	doit = true;
	m_needRecalc = true;
}

// ------------------------------------------------------------
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

	// Render overlay "scene"

	m_overlayScene->render();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.0);
	glEnable(GL_DEPTH_TEST);
}

// ------------------------------------------------------------
void FemWidget::onInitContext()
{
	FltkWidget::onInitContext();
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LINE_SMOOTH);
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

// ------------------------------------------------------------
void FemWidget::onPassiveMotion(int x, int y)
{
#ifdef ADVANCED_GL
	unsigned int i;
	bool inside = false;
	bool needInvalidate = false;

	// Only do select inside defined areas

	this->setEditEnabled(true);

	//cursorState = getScene()->getCursor()->getState();
	m_overlaySelected = false;
	for (i = 0; i < m_areas.size(); i++)
	{
		if (m_areas[i]->inside(x, y))
		{
			inside = true;

			//getScene()->getCursor()->setState(IVF_OBJECT_OFF);
			this->setEditEnabled(false);
			needInvalidate = false;

			CIvfPlaneButton* oldButton = m_selectedButton;

			if (m_selectedButton != nullptr)
			{
				m_selectedButton->setHighlight(Shape::HS_OFF);
				m_selectedButton->setScale(1.0, 1.0, 1.0);
				needInvalidate = true;
				m_overlaySelected = false;
			}

			m_overlayScene->setViewport(pixel_w(), pixel_h());
			m_overlayScene->pick(x, y);
			m_selectedButton = (CIvfPlaneButton*)m_overlayScene->getSelectedShape();

			if (m_selectedButton != nullptr)
			{
				m_selectedButton->setHighlight(Shape::HS_ON);
				m_selectedButton->setScale(1.1, 1.1, 1.1);
				needInvalidate = true;
				m_overlaySelected = true;
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
#endif
}

// ------------------------------------------------------------
void FemWidget::onMouse(int x, int y)
{

}

// ------------------------------------------------------------
void FemWidget::onMouseDown(int x, int y)
{
	m_mouseDownPos[0] = x;
	m_mouseDownPos[1] = y;
#ifdef ADVANCED_GL
	if ((m_overlaySelected) && (m_selectedButton != nullptr))
	{
		m_interactionNode = nullptr;
		m_selectedButton->setButtonState(GenericButton::BS_PRESSED);
		this->redraw();
	}
#endif

	if ((getEditMode() == WidgetMode::User) && (!m_overlaySelected))
	{
		so_print("FemWidget: onMouseDown USER_MODE.");
		if (getCurrentMouseButton() == ButtonState::Button1)
		{
			if (m_saneModel)
				if (m_internalSolver != nullptr)
					setRepresentation(RepresentationMode::Results);
		}
	}
}

// ------------------------------------------------------------
void FemWidget::onMouseUp(int x, int y)
{
#ifdef ADVANCED_GL
	if ((m_overlaySelected) && (m_selectedButton != nullptr))
	{
		this->onButton(m_selectedButton->getId(), (CIvfPlaneButton*)m_selectedButton);
		this->redraw();
	}
#endif

	m_startAlfa = m_alfa;
	m_startBeta = m_beta;

	if (getEditMode() == WidgetMode::User)
	{
		//if (getCurrentMouseButton() == IVF_BUTTON3)
		//	setCustomMode(OF_FEEDBACK);

		if (getCurrentMouseButton() == ButtonState::Button1)
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

// ------------------------------------------------------------
void FemWidget::onMotion(int x, int y)
{
	if ((m_customMode == CustomMode::Feedback) && (getCurrentMouseButton() == ButtonState::Button1))
	{
		double v[3];

		switch (this->getCurrentModifier()) {
		case ButtonState::Shift:
			m_alfa = m_startAlfa + (x - m_mouseDownPos[0]) * M_PI / 500.0;
			break;
		case ButtonState::Ctrl:
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


// ------------------------------------------------------------
void FemWidget::onDeSelect()
{
	//m_dlgNodeProp->setNode(nullptr);
	m_dlgBeamProp->setBeam(nullptr);
	m_nodePropWindow->setNode(nullptr);

	if (m_customMode == CustomMode::Feedback)
	{
		m_interactionNode = nullptr;
		m_tactileForce->setState(Shape::OS_OFF);
		this->redraw();
	}
}

// ------------------------------------------------------------
void FemWidget::onHighlightShape(Shape* shape)
{
	if (m_customMode == CustomMode::Feedback)
	{
		if ((shape->isClass("Node")) && (m_interactionNode == nullptr))
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

// ------------------------------------------------------------
void FemWidget::onSelectFilter(Shape* shape, bool& select)
{
	switch (m_selectFilter) {
	case SelectMode::All:
		select = true;
		break;
	case SelectMode::Nodes:
		if (shape->isClass("VisFemNode"))
			select = true;
		else
			select = false;
		break;
	case SelectMode::Elements:
		if (shape->isClass("VisFemBeam"))
			select = true;
		else
			select = false;
		break;
	case SelectMode::GroundNodes:
		if (shape->isClass("VisNode"))
		{
			VisFemNode* visNode = static_cast<VisFemNode*>(shape);

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

// ------------------------------------------------------------
#ifdef ADVANCED_GL
void FemWidget::onButton(int objectName, CIvfPlaneButton* button)
{
	m_editButtons->clearChecked();
	m_objectButtons->clearChecked();
	//m_viewButtons->clearChecked();
	switch (objectName) {
	case ToolbarButton::Select:
		m_editButtons->check(0);
		this->setEditMode(WidgetMode::Select);
		break;
	case ToolbarButton::Move:
		m_editButtons->check(1);
		this->setEditMode(WidgetMode::Move);
		break;
	case ToolbarButton::CreateNode:
		m_objectButtons->check(0);
		this->setEditMode(WidgetMode::CreateNode);
		break;
	case ToolbarButton::CreateBeam:
		m_objectButtons->check(1);
		this->setEditMode(WidgetMode::CreateLine);
		break;
	case ToolbarButton::Feedback:
		m_editButtons->check(4);
		this->setCustomMode(CustomMode::Feedback);
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
		m_nodeLoadsWindow->setFemNodeLoadSet((FemBeamNodeLoadSet*)m_beamModel->getNodeLoadSet());
		m_nodeLoadsWindow->setVisible(true);
		break;
	case ToolbarButton::BeamLoad:
		m_elementLoadsWindow->setFemLoadSet((FemBeamLoadSet*)m_beamModel->getElementLoadSet());
		m_elementLoadsWindow->setVisible(true);
		//this->showBeamLoads();
		//m_objectButtons->recheck();
		break;
	case ToolbarButton::Materials:
		m_materialsWindow->setFemMaterialSet((FemBeamMaterialSet*)m_beamModel->getMaterialSet());
		m_materialsWindow->setVisible(true);
		//this->showMaterials();
		//m_objectButtons->recheck();
		break;
	case ToolbarButton::NodeBC:
		m_nodeBCsWindow->setFemNodeBCSet((FemBeamNodeBCSet*)m_beamModel->getNodeBCSet());
		m_nodeBCsWindow->setVisible(true);
		break;
	default:
		break;
	}
	this->redraw();
}
#endif

void FemWidget::onHighlightFilter(Shape* shape, bool& highlight)
{
	switch (m_highlightFilter) {
	case HighlightMode::All:
		highlight = true;
		break;
	case HighlightMode::Nodes:
		if (shape->isClass("VisFemNode"))
			highlight = true;
		else
			highlight = false;
		break;
	case HighlightMode::Elements:
		if (shape->isClass("VisFemBeam"))
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
	if (key == 122)
		this->setEditMode(WidgetMode::ViewPan);
}

void FemWidget::onDrawImGui()
{
	bool openDialog = false;
	bool saveDialog = false;
	bool saveAsDialog = false;
	bool executeCalc = false;
	bool quitApplication = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "CTRL+N"))
			{
				m_showNewFileDlg = true;
				m_newModelPopup->show();
			}


			if (ImGui::MenuItem("Open", "CTRL+O"))
				openDialog = true;

			if (ImGui::MenuItem("Save", "Ctrl+S"))
				saveDialog = true;

			if (ImGui::MenuItem("Save as", "Ctrl+Shift+S"))
				saveAsDialog = true;
			ImGui::Separator();

			if (ImGui::MenuItem("Quit", "Alt+F4"))
			{
				quitApplication = true;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
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

			if (ImGui::MenuItem("Settings...", ""))
			{
				m_settingsWindow->show();
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
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Create structure", "")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Calculation"))
		{
			if (ImGui::MenuItem("Exectute", ""))
				executeCalc = true;

			ImGui::EndMenu();
		}
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
			if (ImGui::MenuItem("About...", "")) {}
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
			cout << "Cancel pressed" << endl;
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

	ImGui::Render();

	if (openDialog)
		this->open();

	if (saveDialog)
		this->save();

	if (saveAsDialog)
		this->saveAs();

	if (executeCalc)
		this->executeCalc();

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
	ImFont* font1 = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 20*pixels_per_unit());
	//ImFont* font2 = io.Fonts->AddFontFromFileTTF("anotherfont.otf", 13);

	ImGuiStyle& style = ImGui::GetStyle();

	style.FrameRounding = 4;
	style.WindowRounding = 8;
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(
		style.Colors[ImGuiCol_TitleBgActive].x,
		style.Colors[ImGuiCol_TitleBgActive].y,
		style.Colors[ImGuiCol_TitleBgActive].z,
		0.8f
	);
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

void FemWidget::refreshToolbars()
{
#ifdef ADVANCED_GL
	m_editButtons->clearChecked();
	m_objectButtons->clearChecked();
	//m_viewButtons->clearChecked();

	switch (getEditMode()) {
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
		//m_viewButtons->check(0);
		break;
	case WidgetMode::ViewPan:
		//m_viewButtons->check(1);
		break;
	default:

		break;
	}

	this->redraw();
#endif
}

void FemWidget::removeNodeLoadsFromSelected()
{
	// Remove materials from selected shapes

	FemBeamNodeLoad* nodeLoad = this->getCurrentNodeLoad();

	if (nodeLoad != nullptr)
	{
		auto selected = this->getSelectedShapes();
		for (int i = 0; i < selected->getSize(); i++)
		{
			auto shape = selected->getChild(i);
			if (shape->isClass("VisFemNode"))
			{
				VisFemNode* visNode = static_cast<VisFemNode*>(shape);
				FemNode* node = visNode->getFemNode();
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
	FemBeamNodeLoad* nodeLoad = this->getCurrentNodeLoad();

	if (nodeLoad != nullptr)
		nodeLoad->clearNodes();

	m_needRecalc = true;
	this->set_changed();
	this->redraw();
}

void FemWidget::removeNodeBCsFromSelected()
{
	// Remove materials from selected shapes

	FemBeamNodeBC* nodeBC = this->getCurrentNodeBC();

	if (nodeBC != nullptr)
	{
		auto selected = this->getSelectedShapes();
		for (int i = 0; i < selected->getSize(); i++)
		{
			auto shape = selected->getChild(i);
			if (shape->isClass("VisFemNode"))
			{
				VisFemNode* visNode = static_cast<VisFemNode*>(shape);
				FemNode* node = visNode->getFemNode();
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
	FemBeamNodeBC* nodeBC = this->getCurrentNodeBC();

	if (nodeBC != nullptr)
		nodeBC->clearNodes();

	m_needRecalc = true;
	this->set_changed();
	this->redraw();
}

void FemWidget::removeBeamLoadsFromSelected()
{
	// Remove materials from selected shapes

	FemBeamLoad* beamLoad = this->getCurrentBeamLoad();

	if (beamLoad != nullptr)
	{
		auto selected = this->getSelectedShapes();
		for (int i = 0; i < selected->getSize(); i++)
		{
			auto shape = selected->getChild(i);
			if (shape->isClass("VisFemBeam"))
			{
				VisFemBeam* visBeam = static_cast<VisFemBeam*>(shape);
				FemBeam* beam = visBeam->getBeam();
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

FemBeamNodeBC* FemWidget::getCurrentNodeBC()
{
	return m_currentNodeBC;
}

void FemWidget::hideAllDialogs()
{
	so_print("FemWidget: Hiding all dialogs.");

	m_dlgBeamProp->hide();
	m_dlgElementLoads->hide();

	m_nodeLoadsWindow->hide();
	m_nodeBCsWindow->hide();
	m_elementLoadsWindow->hide();
	m_materialsWindow->hide();

	so_hide();
}

void FemWidget::setResultType(int type)
{
	m_beamModel->setResultType(type);
	this->set_changed();
	this->redraw();
}


// ------------------------------------------------------------
void FemWidget::setProgramPath(const std::string& progPath)
{
	m_progPath = progPath;
}

// ------------------------------------------------------------
const std::string FemWidget::getProgPath()
{
	return m_progPath;
}