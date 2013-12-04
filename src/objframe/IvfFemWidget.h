#ifndef IVFFEMWIDGET_H
#define IVFFEMWIDGET_H

#define OBJFRAME_VERSION_STRING "ObjectiveFrame 1.4.0"
#define ADVANCED_GL
#define LEAP

#include <ivf/ivfconfig.h>

#include <string>
#include <sstream>

#include <ivf/IvfBase.h>
#include <ivf/IvfShape.h>
#include <ivf/IvfMaterial.h>
#include <ivf/IvfQuadPlane.h>
#include <ivf/IvfSphere.h>
#include <ivf/IvfSelectOrtho.h>
#include <ivf/IvfComposite.h>
#include <ivf/IvfExtrArrow.h>

#include <IvfFemNode.h>
#include <IvfFemBeam.h>
#include <IvfBeamModel.h>
#include <IvfFemBeamLoad.h>
#include <IvfFemNodeLoad.h>
#include <IvfFemNodeBC.h>

#include <FemBeamLoad.h>
#include <FemBeamNodeLoad.h>
#include <FemBeamNodeBC.h>

#include <ColorMap.h>
#include <ResultInfo.h>

#ifdef ADVANCED_GL
#include "IvfArea2D.h"
#include "IvfPlaneButton.h"
#include "IvfButtonGroup.h"
#endif

#ifdef ADVANCED_GL
//#include <FTGL/FTFace.h>
//#include <GLTTFont.h>
//#include <GLTTPixmapFont.h>
#endif

#ifndef HAVE_CORBA
#include "FemInternalSolver.h"
#endif


#include "Leap.h"
using namespace Leap;
class LeapInteraction;


#include "NodePropDlg.h"
#include "BeamPropDlg.h"
#include "MaterialsDlg.h"
#include "ElementLoadsDlg.h"
#include "NodeLoadsDlg.h"
#include "NodeBCsDlg.h"
#include "ScalefactorDlg.h"
#include "StructureDlg.h"


#define FRAME_FEM      0
#define FRAME_GEOMETRY 1
#define FRAME_DISPLACEMENTS 2
#define FRAME_RESULTS 3

#define OF_NORMAL         10001
#define OF_FEEDBACK       10002

#define SF_ALL      0
#define SF_NODES	1
#define SF_ELEMENTS 2

#define DF_ALL      0
#define DF_NODES    1
#define DF_ELEMENTS 2

#define HF_ALL      0
#define HF_NODES    1
#define HF_ELEMENTS 2

#define GEST_NONE 0
#define GEST_PINCH_HAND1 1
#define GEST_PINCH_HAND2 2
#define GEST_SPREAD_HAND1 3
#define GEST_SPREAD_HAND2 4
#define GEST_SWIPE_UP 5
#define GEST_SWIPE_DOWN 6
#define GEST_SWIPE_LEFT 7
#define GEST_SWIPE_RIGHT 8

#define LEAP_LINE 2001
#define LEAP_MOVE 2002
#define LEAP_INTERACT 2003


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



template <typename T>
string to_string ( T Number )
{
    ostringstream ss;
    ss << Number;
    return ss.str();
}

#include "IvfFltkWidget.h"


class CIvfFemWidget : public CIvfFltkWidget {
private:
    std::string m_coordText;
    int     m_representation;
    std::string   m_fileName;
    std::string   m_progPath;
    bool    m_overlaySelected;
    bool    m_haveScaleFactor;
    bool    m_needRecalc;
    bool    m_overWorkspace;
    bool    m_lastOverWorkspace;
    bool    m_lockScaleFactor;

    int     m_customMode;
    bool    m_customModeSet;
    double  m_alfa;
    double  m_beta;
    double  m_startAlfa;
    double  m_startBeta;

    int     m_mouseDownPos[2];
    vector<int>     m_keysDown;

    int     m_selectFilter;
    int     m_deleteFilter;
    int		m_highlightFilter;

    int		m_width;
    int		m_height;

    double  m_relNodeSize;
    double  m_relLoadSize;
    double  m_relLineRadius;

    bool m_saneModel;

    int m_argc;
    char** m_argv;

    double m_tactileForceValue;

    Fl_Widget*        m_coordWidget;

    CFemBeamMaterialPtr m_currentMaterial;
    CFemBeamLoadPtr     m_currentElementLoad;
    CFemBeamNodeLoadPtr m_currentNodeLoad;
    CFemBeamNodeBCPtr   m_currentNodeBC;
    CIvfFemNodePtr      m_interactionNode;
    
    LeapInteraction* m_leapinteraction;

    

#ifndef HAVE_CORBA
    CFemInternalSolver* m_internalSolver;
#endif

    CIvfMaterialPtr     m_nodeMaterial;
    CIvfMaterialPtr		m_lineMaterial;
    CIvfShapePtr        m_selectedShape;
#ifdef ADVANCED_GL
    CIvfPlaneButton*	m_selectedButton;
#endif
    CIvfCompositePtr    m_beamLoads;
    CIvfBeamModelPtr    m_beamModel;
    CIvfQuadPlanePtr    m_plane;
    CIvfSpherePtr       m_sphere;
    CIvfExtrArrowPtr    m_tactileForce;

    // Overlay stuff
#ifdef ADVANCED_GL
    //FTFace* m_logoFace;
    //GLTTPixmapFont* m_logoFont;
    //FTFace* m_coordFace;
    //GLTTPixmapFont* m_coordFont;
    vector<CIvfArea2D*> m_areas;
    vector<CIvfPlaneButton*> m_buttons;
    CIvfSelectOrtho* m_overlayScene;
    CIvfButtonGroup* m_editButtons;
    CIvfButtonGroup* m_viewButtons;
    CIvfButtonGroup* m_objectButtons;
    CIvfArea2D* m_editArea;
    CIvfArea2D* m_objectArea;
    CIvfArea2D* m_viewArea;
#endif

    float m_hintColor[3];
    bool m_hintFinished;

    // Result visualisation

    // Private methods

    void doMouse(int x, int y);
    void setupOverlay();

    // Dialogs

    CNodePropDlg* m_dlgNodeProp;
    CBeamPropDlg* m_dlgBeamProp;
    CMaterialsDlg* m_dlgMaterials;
    CElementLoadsDlg* m_dlgElementLoads;
    CNodeLoadsDlg* m_dlgNodeLoads;
    CNodeBCsDlg* m_dlgNodeBCs;
    CScalefactorDlg* m_dlgScalefactor;
    CStructureDlg* m_dlgStructure;

public:
    CIvfFemWidget(int X, int Y, int W, int H, const char *L=0);
    virtual ~CIvfFemWidget();

    // Get set methods
    void setCoordWidget(Fl_Widget* widget);
    void setFileName(const std::string& name);
    void setCurrentMaterial(CFemBeamMaterial* material);
    void setCurrentBeamLoad(CFemBeamLoad* elementLoad);
    CIvfShape*        getSelectedShape();
    CFemBeamMaterial* getCurrentMaterial();
    Fl_Widget*        getCoordWidget();
    const std::string getFileName();
    void setSelectFilter(int filter);
    void setBeamRefreshMode(int mode);
    void setEditMode(int mode);
    void setCustomMode(int mode);
    void setScalefactor(double scalefactor);
    double getScalefactor();
    void setArguments(int argc, char**argv);
    void setCurrentNodeBC(CFemBeamNodeBC* bc);
    void setRotationSelected(double rotation);
    void setCurrentNodeLoad(CFemBeamNodeLoad* nodeLoad);
    CFemBeamNodeLoad* getCurrentNodeLoad();
    CFemBeamLoad* getCurrentBeamLoad();
    void setProgramPath(const std::string& progPath);
    const std::string getProgPath();
    void setResultType(int type);
    CFemBeamNodeBC* getCurrentNodeBC();
    void setRelLoadSize(double size);
    double getRelLoadSize();
    void setRelLineRadius(double radius);
    double getRelLineRadius();
    void setRelNodeSize(double size);
    double getRelNodeSize();
    void setNeedRecalc(bool flag);
    void setHighlightFilter(int filter);
    void setDeleteFilter(int filter);
    void setRepresentation(int repr);

    // Methods

    void hideAllDialogs();
    void removeBeamLoadsFromSelected();
    void removeNodeBCsFromSelected();
    void removeNodeLoadsFromSelected();
    void refreshToolbars();
    void onHighlightFilter(CIvfShape* shape, bool &highlight);
    void deleteSelected();
    void unlockScaleFactor();
    void lockScaleFactor();
    void disableHint();
    bool isHintFinished();
    void initiateHint();
    void doHint();

    virtual void setWorkspace(double size);
    void open();
    void save();
    void saveAs();
    void removeMaterialFromSelected();
    void assignMaterialToSelected();
    void newModel();
    void showBeamLoads();
    void addBeamLoad(CFemBeamLoad* elementLoad);
    void deleteBeamLoad(CFemBeamLoad* elementLoad);
    void assignBeamLoadSelected();
    void selectAllElements();
    void selectAllNodes();
    void showMaterials();
    void showProperties();
    void showScalefactorDlg();
    void showStructureDlg();
    void showNodeLoads();
    void executeCalc();
    void deleteNodeBC(CFemBeamNodeBC* bc);
    void addNodeBC(CFemBeamNodeBC* bc);
    void showNodeBCs();
    void assignNodeBCSelected();
    void deleteNodeLoad(CFemBeamNodeLoad* nodeLoad);
    void assignNodeLoadSelected();
    void addNodeLoad(CFemBeamNodeLoad* nodeLoad);
    void doFeedback();
    
    LeapInteraction* getLeapInteraction();
    void updateLeapController(const Controller& leapController, int* gesture);
    CIvfExtrArrowPtr getTactileForce();
    CIvfSelectOrtho* getOverlay();
    void setTactileForce(CIvfExtrArrowPtr force);
    CIvfFemNodePtr getInteractionNode();
    void setInteractionNode(CIvfFemNode* interactionNode);
    void removeMenus();
    
    // Implemented widget events
    void onCreateNode(double x, double y, double z, CIvfNode* &newNode);
    void onCreateLine(CIvfNode* node1, CIvfNode* node2, CIvfShape* &newLine);
    void onSelect(CIvfComposite* selectedShapes);
    void onCoordinate(double x, double y, double z);
    void onDeleteShape(CIvfShape* shape, bool &doit);
    void onHighlightShape(CIvfShape* shape);
    void onMouse(int x, int y);
    void onMouseDown(int x, int y);
    void onMouseUp(int x, int y);
    void onPassiveMotion(int x, int y);
    void onSelectFilter(CIvfShape* shape, bool &select);
    void onMove(CIvfComposite* selectedShapes, double &dx, double &dy, double &dz, bool &doit);
    void onMotion(int x, int y);
    void onDeSelect();
    void onKeyboard(int key);
    vector<int>* getKeysDown();
    
#ifdef ADVANCED_GL
    void onButton(int objectName, CIvfPlaneButton* button);
#endif
    virtual void onInit();
    virtual void onInitContext();
    virtual void onOverlay();

    
};

#endif
