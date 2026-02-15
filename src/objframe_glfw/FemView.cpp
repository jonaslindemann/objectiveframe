#include "FemView.h"

#include <filesystem>
#include <functional>
#include <sstream>

#include <chaiscript/chaiscript_stdlib.hpp>
#include <chaiscript/extras/math.hpp>

#include <ivf/BitmapFont.h>
#include <ivf/CoordinateSystem.h>
#include <ivf/Fog.h>
#include <ivf/TextLabel.h>
#include <ivf/Texture.h>
#include <ivfimage/SgiImage.h>

#include <ofem/beam.h>
#include <ofem/beam_load.h>
#include <ofem/calfem_reader.h>
#include <ofem/node.h>

#include <vfem/preferences.h>

#include <ofutil/util_functions.h>

#include <logger.h>

#include <imguifd/ImGuiFileDialog.h>
#include <imguifd/ImGuiFileDialogConfig.h>

#include <ofai/structure_generator.h>

using namespace ivf;
using namespace std;
using namespace ofui;
using namespace ofsolver;

// FLTK colormap

typedef unsigned char uchar;

unsigned int fl_cmap[256] = {
    0x00000000, //   0
    0xff000000, //   1
    0x00ff0000, //   2
    0xffff0000, //   3
    0x0000ff00, //   4
    0xff00ff00, //   5
    0x00ffff00, //   6
    0xffffff00, //   7
    0x55555500, //   8
    0xc6717100, //   9
    0x71c67100, //  10
    0x8e8e3800, //  11
    0x7171c600, //  12
    0x8e388e00, //  13
    0x388e8e00, //  14
    0x00008000, //  15
    0xa8a89800, //  16
    0xe8e8d800, //  17
    0x68685800, //  18
    0x98a8a800, //  19
    0xd8e8e800, //  20
    0x58686800, //  21
    0x9c9ca800, //  22
    0xdcdce800, //  23
    0x5c5c6800, //  24
    0x9ca89c00, //  25
    0xdce8dc00, //  26
    0x5c685c00, //  27
    0x90909000, //  28
    0xc0c0c000, //  29
    0x50505000, //  30
    0xa0a0a000, //  31
    0x00000000, //  32
    0x0d0d0d00, //  33
    0x1a1a1a00, //  34
    0x26262600, //  35
    0x31313100, //  36
    0x3d3d3d00, //  37
    0x48484800, //  38
    0x55555500, //  39
    0x5f5f5f00, //  40
    0x6a6a6a00, //  41
    0x75757500, //  42
    0x80808000, //  43
    0x8a8a8a00, //  44
    0x95959500, //  45
    0xa0a0a000, //  46
    0xaaaaaa00, //  47
    0xb5b5b500, //  48
    0xc0c0c000, //  49
    0xcbcbcb00, //  50
    0xd5d5d500, //  51
    0xe0e0e000, //  52
    0xeaeaea00, //  53
    0xf5f5f500, //  54
    0xffffff00, //  55
    0x00000000, //  56
    0x00240000, //  57
    0x00480000, //  58
    0x006d0000, //  59
    0x00910000, //  60
    0x00b60000, //  61
    0x00da0000, //  62
    0x00ff0000, //  63
    0x3f000000, //  64
    0x3f240000, //  65
    0x3f480000, //  66
    0x3f6d0000, //  67
    0x3f910000, //  68
    0x3fb60000, //  69
    0x3fda0000, //  70
    0x3fff0000, //  71
    0x7f000000, //  72
    0x7f240000, //  73
    0x7f480000, //  74
    0x7f6d0000, //  75
    0x7f910000, //  76
    0x7fb60000, //  77
    0x7fda0000, //  78
    0x7fff0000, //  79
    0xbf000000, //  80
    0xbf240000, //  81
    0xbf480000, //  82
    0xbf6d0000, //  83
    0xbf910000, //  84
    0xbfb60000, //  85
    0xbfda0000, //  86
    0xbfff0000, //  87
    0xff000000, //  88
    0xff240000, //  89
    0xff480000, //  90
    0xff6d0000, //  91
    0xff910000, //  92
    0xffb60000, //  93
    0xffda0000, //  94
    0xffff0000, //  95
    0x00003f00, //  96
    0x00243f00, //  97
    0x00483f00, //  98
    0x006d3f00, //  99
    0x00913f00, // 100
    0x00b63f00, // 101
    0x00da3f00, // 102
    0x00ff3f00, // 103
    0x3f003f00, // 104
    0x3f243f00, // 105
    0x3f483f00, // 106
    0x3f6d3f00, // 107
    0x3f913f00, // 108
    0x3fb63f00, // 109
    0x3fda3f00, // 110
    0x3fff3f00, // 111
    0x7f003f00, // 112
    0x7f243f00, // 113
    0x7f483f00, // 114
    0x7f6d3f00, // 115
    0x7f913f00, // 116
    0x7fb63f00, // 117
    0x7fda3f00, // 118
    0x7fff3f00, // 119
    0xbf003f00, // 120
    0xbf243f00, // 121
    0xbf483f00, // 122
    0xbf6d3f00, // 123
    0xbf913f00, // 124
    0xbfb63f00, // 125
    0xbfda3f00, // 126
    0xbfff3f00, // 127
    0xff003f00, // 128
    0xff243f00, // 129
    0xff483f00, // 130
    0xff6d3f00, // 131
    0xff913f00, // 132
    0xffb63f00, // 133
    0xffda3f00, // 134
    0xffff3f00, // 135
    0x00007f00, // 136
    0x00247f00, // 137
    0x00487f00, // 138
    0x006d7f00, // 139
    0x00917f00, // 140
    0x00b67f00, // 141
    0x00da7f00, // 142
    0x00ff7f00, // 143
    0x3f007f00, // 144
    0x3f247f00, // 145
    0x3f487f00, // 146
    0x3f6d7f00, // 147
    0x3f917f00, // 148
    0x3fb67f00, // 149
    0x3fda7f00, // 150
    0x3fff7f00, // 151
    0xbf007f00, // 160
    0xbf247f00, // 161
    0xbf487f00, // 162
    0xbf6d7f00, // 163
    0xbf917f00, // 164
    0xbfb67f00, // 165
    0xbfda7f00, // 166
    0xbfff7f00, // 167
    0xff007f00, // 168
    0xff247f00, // 169
    0xff487f00, // 170
    0xff6d7f00, // 171
    0xff917f00, // 172
    0xffb67f00, // 173
    0xffda7f00, // 174
    0xffff7f00, // 175
    0x0000bf00, // 176
    0x0024bf00, // 177
    0x0048bf00, // 178
    0x006dbf00, // 179
    0x0091bf00, // 180
    0x00b6bf00, // 181
    0x00dabf00, // 182
    0x00ffbf00, // 183
    0x3f00bf00, // 184
    0x3f24bf00, // 185
    0x3f48bf00, // 186
    0x3f6dbf00, // 187
    0x3f91bf00, // 188
    0x3fb6bf00, // 189
    0x3fdabf00, // 190
    0x3fffbf00, // 191
    0x7f00bf00, // 192
    0x7f24bf00, // 193
    0x7f48bf00, // 194
    0x7f6dbf00, // 195
    0x7f91bf00, // 196
    0x7fb6bf00, // 197
    0x7fdabf00, // 198
    0x7fffbf00, // 199
    0xbf00bf00, // 200
    0xbf24bf00, // 201
    0xbf48bf00, // 202
    0xbf6dbf00, // 203
    0xbf91bf00, // 204
    0xbfb6bf00, // 205
    0xbfdabf00, // 206
    0xbfffbf00, // 207
    0xff00bf00, // 208
    0xff24bf00, // 209
    0xff48bf00, // 210
    0xff6dbf00, // 211
    0xff91bf00, // 212
    0xffb6bf00, // 213
    0xffdabf00, // 214
    0xffffbf00, // 215
    0x0000ff00, // 216
    0x0024ff00, // 217
    0x0048ff00, // 218
    0x006dff00, // 219
    0x0091ff00, // 220
    0x00b6ff00, // 221
    0x00daff00, // 222
    0x00ffff00, // 223
    0x3f00ff00, // 224
    0x3f24ff00, // 225
    0x3f48ff00, // 226
    0x3f6dff00, // 227
    0x3f91ff00, // 228
    0x3fb6ff00, // 229
    0x3fdaff00, // 230
    0x3fffff00, // 231
    0x7f00ff00, // 232
    0x7f24ff00, // 233
    0x7f48ff00, // 234
    0x7f6dff00, // 235
    0x7f91ff00, // 236
    0x7fb6ff00, // 237
    0x7fdaff00, // 238
    0x7fffff00, // 239
    0xbf00ff00, // 240
    0xbf24ff00, // 241
    0xbf48ff00, // 242
    0xbf6dff00, // 243
    0xbf91ff00, // 244
    0xbfb6ff00, // 245
    0xbfdaff00, // 246
    0xbfffff00, // 247
    0xff00ff00, // 248
    0xff24ff00, // 249
    0xff48ff00, // 250
    0xff6dff00, // 251
    0xff91ff00, // 252
    0xffb6ff00, // 253
    0xffdaff00, // 254
    0xffffff00  // 255};
};

// Constructor/Destructor

FemViewWindow::FemViewWindow(int width, int height, const std::string title, GLFWmonitor *monitor, GLFWwindow *shared)
    : IvfViewWindow(width, height, title, monitor, shared), m_uiScale{1.0f}, m_service{nullptr}, m_width{width},
      m_height{height}, m_tactileForce{nullptr}, m_currentSolver{nullptr}, m_relNodeSize{0.004},
      m_relLineRadius{0.0015}, m_relLoadSize{0.06}, m_customMode{CustomMode::Feedback}, m_customModeSet{false},
      m_alfa{0.0}, m_beta{0.0}, m_startAlfa{0.0}, m_startBeta{M_PI / 2.0}, m_haveScaleFactor{false}, m_needRecalc{true},
      m_selectFilter{SelectMode::All}, m_deleteFilter{DeleteMode::All}, m_highlightFilter{HighlightMode::All},
      m_overWorkspace{true}, m_lastOverWorkspace{true}, m_hintFinished{true}, m_lockScaleFactor{false},
      m_saneModel{false}, m_selectedPos{0.0, 0.0, 0.0}, m_useSphereCursor{false}, m_useBlending{false},
      m_useImGuiFileDialogs{true}, m_tactileForceValue{1000.0}, m_progPathStr{""}, m_showNodeBCsWindow{false},
      m_showBCPropPopup{false}, m_prevButton{nullptr}, m_nodeSelection{false}, m_elementSelection{false},
      m_mixedSelection{false}, m_openDialog{false}, m_saveDialog{false}, m_saveAsDialog{false},
      m_saveAsCalfemDialog{false}, m_openFromCalfemDialog{false}, m_saveScreenShot{false}, m_openScriptDialog{false},
      m_showDiagnostics{false}, m_openEditScriptDialog{false}, m_newScriptDialog{false}, m_aiApiKey{""},
      m_structureGenerator(""), m_isProcessingAiRequest{false}, m_pluginRunning{false}, m_autoRunAiScript{true},
      m_systemPromptFilename{""}
{
    this->setUseEscQuit(false);
    this->setUseCustomPick(true);

    m_aiApiKey = ofutil::get_config_value("ai_api_key", "");
}

std::shared_ptr<FemViewWindow> FemViewWindow::create(int width, int height, const std::string title,
                                                     GLFWmonitor *monitor, GLFWwindow *shared)
{
    return std::make_shared<FemViewWindow>(width, height, title, monitor, shared);
}

FemViewWindow::~FemViewWindow()
{
    ofutil::set_config_value("ai_api_key", m_aiApiKey);
    log("Destructor.");
}

void FemViewWindow::runPlugin(ScriptPlugin *plugin)
{
    this->snapShot();

    chaiscript::ChaiScript script;

    this->setupScript(script);

    chaiscript::ChaiScript::State s = script.get_state(); // get initial state
    try
    {
        m_pluginRunning = true;
        script.eval(plugin->source());
        m_pluginRunning = false;
    } catch (const chaiscript::exception::eval_error &e)
    {
        log(e.pretty_print());
    }
    script.set_state(s);

    m_beamModel->enumerate();
}

void FemViewWindow::runScript(std::string scriptFilename)
{
    this->snapShot();

    m_selectedPos[0] = 0.0;
    m_selectedPos[1] = 0.0;
    m_selectedPos[2] = 0.0;

    chaiscript::ChaiScript chai;
    this->setupScript(chai);

    auto scriptSource = ofutil::read_file(scriptFilename);

    try
    {
        chai.eval(scriptSource);
    } catch (const chaiscript::exception::eval_error &e)
    {
        log(e.pretty_print());
    }

    m_beamModel->enumerate();

    this->set_changed();
    this->redraw();
}

void FemViewWindow::runScriptFromText(std::string scriptText)
{
    m_beamModel->enumerate();

    this->snapShot();

    chaiscript::ChaiScript chai;
    this->setupScript(chai);

    try
    {
        chai.eval(scriptText);
    } catch (const chaiscript::exception::eval_error &e)
    {
        m_promptWindow->clearError();
        m_promptWindow->addError(e.pretty_print());
        log(e.pretty_print());
    }

    m_beamModel->enumerate();

    this->set_changed();
    this->redraw();
}

void FemViewWindow::makeAiRequest(const std::string &userPrompt)
{
    if (m_aiApiKey.empty())
    {
        log("No API key set for AI service.");
        return;
    }

    m_structureGenerator.setApiKey(m_aiApiKey);
    m_structureGenerator.generateStructureAsync(userPrompt, std::bind(&FemViewWindow::onGenerationComplete, this,
                                                                      std::placeholders::_1, std::placeholders::_2));

    m_isProcessingAiRequest = true;
}

void FemViewWindow::onGenerationComplete(const std::string &result, bool success)
{
    if (success)
    {
        log("AI generation successful.");
        m_promptWindow->clearOutput();
        m_promptWindow->addOutput(result);
        
        if (m_autoRunAiScript)
        {
            std::lock_guard<std::mutex> lock(m_scriptQueueMutex);
            m_pendingScripts.push(result);
            log("Script queued for execution on main thread.");
        }
        m_isProcessingAiRequest = false;
    }
    else
    {
        log("AI generation failed.");
        m_isProcessingAiRequest = false;
    }
}

bool FemViewWindow::isProcessingAiRequest() const
{
    return m_isProcessingAiRequest;
}

void FemViewWindow::setAutoRunAiScript(bool autoRun)
{
    m_autoRunAiScript = autoRun;
}

bool FemViewWindow::autoRunAiScript() const
{
    return m_autoRunAiScript;
}

ofai::PromptDatabase &FemViewWindow::getPromptDatabase()
{
    return m_promptDatabase;
}

// Get/set methods

void FemViewWindow::setFileName(const std::string &name)
{
    m_fileName = name;
}

const std::string FemViewWindow::getFileName()
{
    // Return current filename

    return m_fileName;
}

void FemViewWindow::updateAxisLabels()
{
    m_textLayer->clear();

    auto axisLabelPlusX = ivf::TextLabel::create();

    axisLabelPlusX->setCamera(this->getCamera());
    axisLabelPlusX->setFont(m_redFont);
    axisLabelPlusX->setText("+X", 0.5);
    axisLabelPlusX->setAlignObject(IVF_ALIGN_CAMERA);
    axisLabelPlusX->setVector(0.0, 1.0, 0.0);
    axisLabelPlusX->setPosition(this->getWorkspace() / 2.0 + 1.0, 0.0, 0.0);
    axisLabelPlusX->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(axisLabelPlusX);

    auto axisLabelMinusX = ivf::TextLabel::create();
    axisLabelMinusX->setCamera(this->getCamera());
    axisLabelMinusX->setFont(m_redFont);
    axisLabelMinusX->setText("-X", 0.5);
    axisLabelMinusX->setAlignObject(IVF_ALIGN_CAMERA);
    axisLabelMinusX->setPosition(-this->getWorkspace() / 2.0 - 1.0, 0.0, 0.0);
    axisLabelMinusX->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(axisLabelMinusX);

    auto axisLabelPlusY = ivf::TextLabel::create();
    axisLabelPlusY->setCamera(this->getCamera());
    axisLabelPlusY->setFont(m_greenFont);
    axisLabelPlusY->setText("+Z", 0.5);
    axisLabelPlusY->setAlignObject(IVF_ALIGN_CAMERA);
    axisLabelPlusY->setPosition(0.0, 0.0, this->getWorkspace() / 2.0 + 1.0);
    axisLabelPlusY->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(axisLabelPlusY);

    auto axisLabelMinusY = ivf::TextLabel::create();
    axisLabelMinusY->setCamera(this->getCamera());
    axisLabelMinusY->setFont(m_greenFont);
    axisLabelMinusY->setText("-Z", 0.5);
    axisLabelMinusY->setAlignObject(IVF_ALIGN_CAMERA);
    axisLabelMinusY->setPosition(0.0, 0.0, -this->getWorkspace() / 2.0 - 1.0);
    axisLabelMinusY->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(axisLabelMinusY);

    auto upperLeft = ivf::TextLabel::create();
    upperLeft->setCamera(this->getCamera());
    upperLeft->setFont(m_axisFont);
    upperLeft->setText(ofutil::to_coord_string(-this->getWorkspace() / 2.0, -this->getWorkspace() / 2.0), 0.5);
    upperLeft->setAlignObject(IVF_ALIGN_CAMERA);
    upperLeft->setPosition(-this->getWorkspace() / 2.0 - 1.0, 0.0, -this->getWorkspace() / 2.0 - 1.0);
    upperLeft->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(upperLeft);

    auto upperRight = ivf::TextLabel::create();
    upperRight->setCamera(this->getCamera());
    upperRight->setFont(m_axisFont);
    upperRight->setText(ofutil::to_coord_string(this->getWorkspace() / 2.0, -this->getWorkspace() / 2.0), 0.5);
    upperRight->setAlignObject(IVF_ALIGN_CAMERA);
    upperRight->setPosition(this->getWorkspace() / 2.0 + 1.0, 0.0, -this->getWorkspace() / 2.0 - 1.0);
    upperRight->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(upperRight);

    auto lowerLeft = ivf::TextLabel::create();
    lowerLeft->setCamera(this->getCamera());
    lowerLeft->setFont(m_axisFont);
    lowerLeft->setText(ofutil::to_coord_string(-this->getWorkspace() / 2.0, this->getWorkspace() / 2.0), 0.5);
    lowerLeft->setAlignObject(IVF_ALIGN_CAMERA);
    lowerLeft->setPosition(-this->getWorkspace() / 2.0 - 1.0, 0.0, this->getWorkspace() / 2.0 + 1.0);
    lowerLeft->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(lowerLeft);

    auto lowerRight = ivf::TextLabel::create();
    lowerRight->setCamera(this->getCamera());
    lowerRight->setFont(m_axisFont);
    lowerRight->setText(ofutil::to_coord_string(this->getWorkspace() / 2.0, this->getWorkspace() / 2.0), 0.5);
    lowerRight->setAlignObject(IVF_ALIGN_CAMERA);
    lowerRight->setPosition(this->getWorkspace() / 2.0 + 1.0, 0.0, this->getWorkspace() / 2.0 + 1.0);
    lowerRight->setBillboardType(IVF_BILLBOARD_XY);
    m_textLayer->addChild(lowerRight);
}

void FemViewWindow::setWorkspace(double size, bool resetCamera)
{
    IvfViewWindow::setWorkspace(size, resetCamera);

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

void FemViewWindow::setCurrentMaterial(ofem::BeamMaterial *material)
{
    // Set current material

    m_currentMaterial = ofem::BeamMaterialPtr(material);
}

ofem::BeamMaterial *FemViewWindow::getCurrentMaterial()
{
    // Return current material

    return m_currentMaterial.get();
}

void FemViewWindow::setCurrentBeamLoad(ofem::BeamLoad *elementLoad)
{
    // Set current elementload

    m_currentElementLoad = ofem::BeamLoadPtr(elementLoad);
}

void FemViewWindow::setRepresentation(RepresentationMode repr)
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

ofem::BeamModel *FemViewWindow::getModel()
{
    return m_beamModel.get();
}

Shape *FemViewWindow::getSelectedShape()
{
    // Return currently selected shape

    return m_selectedShape;
}

ofem::BeamLoad *FemViewWindow::getCurrentBeamLoad()
{
    return m_currentElementLoad.get();
}

void FemViewWindow::setCurrentNodeLoad(ofem::BeamNodeLoad *nodeLoad)
{
    m_currentNodeLoad = ofem::BeamNodeLoadPtr(nodeLoad);
}

ofem::BeamNodeLoad *FemViewWindow::getCurrentNodeLoad()
{
    return m_currentNodeLoad.get();
}

void FemViewWindow::setEditMode(WidgetMode mode)
{
    // If setEditMode is not called in response to a setCustomMode call,
    // the custom mode is set to OF_NORMAL

    // set highlight filter

    this->getScene()->disableCursor();
    this->getScene()->disableCursorShape();
    this->setUseOverlay(true);
    m_pluginWindow->hide();

    this->hideAllDialogs();

    switch (mode)
    {
    case WidgetMode::Select:
        log("WidgetMode::Select");
        m_consoleWindow->clear();
        m_mainToolbarWindow->selectButton(0, 1);
        console("Select: Click on objects to select. Click outside to deselect.");
        setHighlightFilter(HighlightMode::All);
        setSelectFilter(SelectMode::All);
        setRepresentation(RepresentationMode::Fem);
        m_beamModel->setResultType(IVF_BEAM_NO_RESULT);
        break;
    case WidgetMode::BoxSelection:
        log("WidgetMode::BoxSelect");
        m_consoleWindow->clear();
        m_mainToolbarWindow->selectButton(1, 1);
        console("Select: Click on objects to select. Click outside to deselect.");
        setHighlightFilter(HighlightMode::All);
        setSelectFilter(SelectMode::All);
        setRepresentation(RepresentationMode::Fem);
        break;
    case WidgetMode::CreateNode:
        log("WidgetMode::CreateNode");
        m_consoleWindow->clear();
        m_editToolbarWindow->selectButton(0, 1);
        console("Create nodes: Use the cursor to create nodes. [Shift] moves up/down.");
        setHighlightFilter(HighlightMode::Nodes);
        setSelectFilter(SelectMode::Nodes);
        setRepresentation(RepresentationMode::Fem);
        break;
    case WidgetMode::CreateLine:
        log("WidgetMode::CreateLine");
        m_consoleWindow->clear();
        m_editToolbarWindow->selectButton(1, 1);
        console("Create beams: Select 2 nodes to create a beam element.");
        setHighlightFilter(HighlightMode::Nodes);
        setSelectFilter(SelectMode::Nodes);
        setRepresentation(RepresentationMode::Fem);
        break;
    case WidgetMode::Move:
        log("WidgetMode::Move");
        m_consoleWindow->clear();
        m_mainToolbarWindow->selectButton(2, 1);
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
        console(
            "Feedback mode: Click on a node to apply interactive force. Move mouse with button down to move force.");
        setHighlightFilter(HighlightMode::Nodes);
        setSelectFilter(SelectMode::Nodes);
        m_mainToolbarWindow->selectButton(5, 1);
        m_loadMixerWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet *)m_beamModel->getNodeLoadSet());
        m_loadMixerWindow->show();
        m_windowList->placeWindow(m_loadMixerWindow);
    }
    else
        m_loadMixerWindow->hide();

    if (!m_customModeSet)
    {
        m_tactileForce->setState(Shape::OS_OFF);
        m_interactionNode = nullptr;
        m_customMode = CustomMode::Normal;
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

    IvfViewWindow::setEditMode(mode);

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

void FemViewWindow::setBeamRefreshMode(ivf::LineRefreshMode mode)
{
    auto scene = this->getScene()->getComposite();
    int i;

    for (i = 0; i < scene->getSize(); i++)
    {
        auto shape = scene->getChild(i);
        if (shape->isClass("vfem::SolidLine"))
        {
            SolidLine *solidLine = static_cast<SolidLine *>(shape);
            solidLine->setRefresh(mode);
        }
    }
}

void FemViewWindow::setArguments(int argc, char **argv)
{
    m_argc = argc;
    m_argv = argv;
}

void FemViewWindow::setScalefactor(double scalefactor)
{
    m_beamModel->setScaleFactor(scalefactor);
    this->set_changed();
    this->redraw();
}

double FemViewWindow::getScalefactor()
{
    return m_beamModel->getScaleFactor();
}

void FemViewWindow::setCurrentNodeBC(ofem::BeamNodeBC *bc)
{
    m_currentNodeBC = ofem::BeamNodeBCPtr(bc);
}

void FemViewWindow::setCustomMode(CustomMode mode)
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
        m_loadMixerWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet *)m_beamModel->getNodeLoadSet());
        m_loadMixerWindow->show();
        m_windowList->placeWindow(m_loadMixerWindow);
    }
    else
        m_loadMixerWindow->hide();

    if ((m_customMode == CustomMode::Structure) || (m_customMode == CustomMode::Paste))
    {
        this->setEditMode(WidgetMode::SelectPosition);
    }
    else
    {
        m_pluginWindow->hide();
    }
}

void FemViewWindow::setSelectFilter(SelectMode filter)
{
    m_selectFilter = filter;
}

void FemViewWindow::setDeleteFilter(DeleteMode filter)
{
    m_deleteFilter = filter;
}

void FemViewWindow::setHighlightFilter(HighlightMode filter)
{
    m_highlightFilter = filter;
}

void FemViewWindow::setNeedRecalc(bool flag)
{
    m_needRecalc = flag;
}

void FemViewWindow::setRelNodeSize(double size)
{
    m_relNodeSize = size;

    if (m_beamModel != nullptr)
        m_beamModel->setNodeSize(this->getWorkspace() * m_relNodeSize);
}

void FemViewWindow::setRelLineRadius(double radius)
{
    m_relLineRadius = radius;

    if (m_beamModel != nullptr)
        m_beamModel->setLineRadius(this->getWorkspace() * m_relLineRadius);
}

void FemViewWindow::setRelLoadSize(double size)
{
    m_relLoadSize = size;

    if (m_beamModel != nullptr)
        m_beamModel->setLoadSize(this->getWorkspace() * m_relLoadSize);
}

double FemViewWindow::getRelNodeSize()
{
    return m_relNodeSize;
}

double FemViewWindow::getRelLineRadius()
{
    return m_relLineRadius;
}

double FemViewWindow::getRelLoadSize()
{
    return m_relLoadSize;
}

ofem::BeamNodeBC *FemViewWindow::getCurrentNodeBC()
{
    return m_currentNodeBC.get();
}

void FemViewWindow::setResultType(int type)
{
    m_beamModel->setResultType(type);
    this->set_changed();
    this->redraw();
}

void FemViewWindow::setProgramPath(const std::string &progPath)
{
    namespace fs = std::filesystem;

    m_progPathStr = progPath;
    m_progPath.assign(m_progPathStr);

    m_fontPath = m_progPath / fs::path("fonts");
    m_imagePath = m_progPath / fs::path("images");
    m_pluginPath = m_progPath / fs::path("plugins");
    m_mapPath = m_progPath / fs::path("maps");
    m_pythonPath = m_progPath / fs::path("python");
    m_examplePath = m_progPath / fs::path("examples");
    m_aiPath = m_progPath / fs::path("ai");
}

const std::string FemViewWindow::getProgPath()
{
    return m_progPathStr;
}

// Widget methods

void FemViewWindow::exportAsCalfem(std::string filename)
{
    auto writer = ofem::CalfemWriter::create(filename);
    writer->setPythonPath(m_pythonPath);
    writer->setFemModel(m_beamModel.get());
    writer->save();
}

void FemViewWindow::importAsCalfem(std::string filename)
{
    auto reader = ofem::CalfemReader::create(filename);
    this->openFromString(reader->df3String());
}

void FemViewWindow::snapShot()
{
    m_beamModel->snapShot();
    log("Current undo level: " + ofutil::to_string(m_beamModel->snapShotCount()));
}

void FemViewWindow::restoreLastSnapShot()
{
    auto prevEditMode = this->getEditMode();

    m_beamModel->restoreLastSnapShot();
    log("Current undo level: " + ofutil::to_string(m_beamModel->snapShotCount()));

    this->hideAllDialogs();
    this->deleteAll();

    // Initialize and open beam model

    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());

    // m_beamModel->setShowNodeNumbers(false);

    // Generate a Ivf++ representation

    m_beamModel->generateModel();

    // Update dialogs

    m_propWindow->setNode(nullptr);
    m_propWindow->setBeam(nullptr);
    m_propWindow->setSelectedShapes(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_needRecalc = true;

    m_currentSolver = nullptr;

    this->setEditMode(prevEditMode);
}

void FemViewWindow::revertLastSnapShot()
{
    auto prevEditMode = this->getEditMode();

    m_beamModel->revertLastSnapShot();
    log("Current undo level: " + ofutil::to_string(m_beamModel->snapShotCount()));

    this->hideAllDialogs();
    this->deleteAll();

    // Initialize and open beam model

    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());

    // Generate a Ivf++ representation

    m_beamModel->generateModel();

    // Update dialogs

    m_propWindow->setNode(nullptr);
    m_propWindow->setBeam(nullptr);
    m_propWindow->setSelectedShapes(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_needRecalc = true;

    m_currentSolver = nullptr;

    this->setEditMode(prevEditMode);
}

void FemViewWindow::openFromString(const std::string df3_string)
{
    this->lockSceneRendering();

    this->setFileName("noname.df3");
    this->deleteAll();

    log("Setting color map path.");

    // Initialize and open beam model

    m_beamModel->initialize();
    m_beamModel->setFileName(m_fileName);
    m_beamModel->setPath(m_mapPath.string());
    m_beamModel->openFromString(df3_string);
    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());

    // Generate a Ivf++ representation

    m_beamModel->generateModel();

    // Update dialogs

    m_propWindow->setNode(nullptr);
    m_propWindow->setBeam(nullptr);
    m_propWindow->setSelectedShapes(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_needRecalc = true;

    m_currentSolver = nullptr;

    this->setEditMode(WidgetMode::Select);

    this->setUseBlending(false);

    this->unlockSceneRendering();
}

void FemViewWindow::save(std::string filename)
{
    // Save model to file

    std::string oldName = m_beamModel->getFileName();

    m_beamModel->setFileName(filename);
    m_beamModel->save();
    m_beamModel->setFileName(oldName);
}

void FemViewWindow::open(std::string filename)
{
    namespace fs = std::filesystem;

    // Change filename and erase previous model/scene

    if (!fs::exists(filename))
        return;

    this->lockSceneRendering();

    this->setFileName(filename);
    this->deleteAll();

    log("Setting color map path.");

    // Initialize and open beam model

    m_beamModel->initialize();
    m_beamModel->setFileName(m_fileName);
    m_beamModel->setPath(m_mapPath.string());
    m_beamModel->open();
    m_beamModel->setTextFont(m_labelFont);
    m_beamModel->setCamera(this->getCamera());

    // Generate a Ivf++ representation

    m_beamModel->generateModel();

    // Update dialogs

    m_propWindow->setNode(nullptr);
    m_propWindow->setBeam(nullptr);
    m_propWindow->setSelectedShapes(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_needRecalc = true;

    m_currentSolver = nullptr;

    this->setEditMode(WidgetMode::Select);

    this->setUseBlending(false);

    this->unlockSceneRendering();
}

void FemViewWindow::copy()
{
    auto selectedShapes = this->getSelectedShapes();

    m_modelClipBoard->clear();

    for (int i = 0; i < selectedShapes->getSize(); i++)
    {
        auto shape = selectedShapes->getChild(i);

        if (shape->isClass("vfem::Node"))
        {
            auto vnode = static_cast<vfem::Node *>(shape);
            auto node = vnode->getFemNode();
            m_modelClipBoard->addNode(node);
        }

        if (shape->isClass("vfem::Beam"))
        {
            auto vbeam = static_cast<vfem::Beam *>(shape);
            auto beam = vbeam->getBeam();
            m_modelClipBoard->addElement(beam);
        }
    }
}

void FemViewWindow::paste()
{
    this->setCustomMode(CustomMode::Paste);
}

void FemViewWindow::set_changed()
{}

void FemViewWindow::showProperties()
{
    // Properties for selected shape

    this->onSelect(this->getSelectedShapes());

    if (m_nodeSelection || m_singleNodeSelection)
    {
        m_propWindow->show();
        m_propWindow->setPositionFromBottom(20, 540);
        m_windowList->placeWindow(m_propWindow);
    }

    if (m_elementSelection || m_singleElementSelection)
    {
        m_propWindow->show();
        m_propWindow->setPositionFromBottom(20, 540);
        m_windowList->placeWindow(m_propWindow);
    }

    if ((!m_nodeSelection) && (!m_elementSelection))
    {
        m_consoleWindow->clear();
        console("Inspect: Please select an object(s) to inspect.");
    }

    setEditMode(WidgetMode::Select);
    this->refreshToolbars();
}

void FemViewWindow::showMaterials()
{}

void FemViewWindow::newModel()
{
    namespace fs = std::filesystem;

    this->lockSceneRendering();

    this->hideAllDialogs();

    // Delete all Ivf++ objects

    this->deleteAll();

    // Setup new beam model

    m_beamModel = vfem::BeamModel::create();

    m_beamModel->initialize();
    m_beamModel->setPath(m_mapPath.string());
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

    m_beamModel->generateModel();

    m_currentMaterial = nullptr;

    // Initialize color table

    auto colorTable = m_beamModel->getColorTable();

    unsigned int c, red, green, blue;

    for (int i = 0; i < 256; i++)
    {
        if (i & 0xffffff00)
            c = (unsigned)i;
        else
            c = fl_cmap[i];

        red = uchar(c >> 24);
        green = uchar(c >> 16);
        blue = uchar(c >> 8);

        colorTable->setColor(i, (float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f);
    }

    // Initialize dialogs

    m_propWindow->setNode(nullptr);
    m_propWindow->setBeam(nullptr);

    // Add tactile force

    this->getScene()->addChild(m_tactileForce);

    double loadSize = m_beamModel->getLoadSize();

    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    m_tactileForce->setDirection(0.0, -1.0, 0.0);
    m_tactileForce->setOffset(-loadSize * 0.7);

    m_sphereCursor = Sphere::create();
    m_sphereCursor->setMaterial(m_nodeMaterial);
    m_sphereCursor->setRadius(m_beamModel->getNodeSize());

    m_cubeCursor = Cube::create();
    m_cubeCursor->setMaterial(m_nodeMaterial);
    m_cubeCursor->setSize(m_beamModel->getNodeSize() * 1.5);

    if (vfem::Preferences::instance().useSphereNodes())
        m_nodeCursor = m_sphereCursor;
    else
        m_nodeCursor = m_cubeCursor;

    this->getScene()->setCursorShape(m_nodeCursor);

    this->getScene()->addChild(m_tactileForce);

    m_needRecalc = true;

    m_currentSolver = nullptr;

    this->setEditMode(WidgetMode::ViewZoom);

    this->setUseBlending(false);

    this->unlockSceneRendering();

    // Update screen

    this->redraw();
}

void FemViewWindow::fitWorkspaceToModel(double padding)
{
    // Calculate bounding box of all nodes in the model
    
    auto nodeSet = m_beamModel->getNodeSet();
    
    if (nodeSet->getSize() == 0)
    {
        log("No nodes in model, cannot fit workspace.");
        return;
    }
    
    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double minZ = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();
    double maxZ = std::numeric_limits<double>::lowest();
    
    // Find bounding box
    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        double x, y, z;
        node->getCoord(x, y, z);
        
        minX = std::min(minX, x);
        minY = std::min(minY, y);
        minZ = std::min(minZ, z);
        maxX = std::max(maxX, x);
        maxY = std::max(maxY, y);
        maxZ = std::max(maxZ, z);
    }
    
    // Calculate extent in each direction
    double extentX = maxX - minX;
    double extentY = maxY - minY;
    double extentZ = maxZ - minZ;
    
    // Find maximum extent (for square workspace)
    double maxExtent = std::max({extentX, extentY, extentZ});
    
    // Apply padding
    double newWorkspaceSize = maxExtent * padding;
    
    // Round up to whole meters
    newWorkspaceSize = std::ceil(newWorkspaceSize);
    
    // Ensure minimum workspace size
    if (newWorkspaceSize < 1.0)
        newWorkspaceSize = 10.0;
    
    log("Fitting workspace to model. Extent: " + std::to_string(maxExtent) + 
        ", New workspace: " + std::to_string(newWorkspaceSize));
    
    // Set the new workspace size
    this->setWorkspace(newWorkspaceSize, true);
    
    // Update beam model properties based on new workspace
    m_beamModel->setNodeSize(this->getWorkspace() * m_relNodeSize);
    m_beamModel->setLineRadius(this->getWorkspace() * m_relLineRadius);
    m_beamModel->setLoadSize(this->getWorkspace() * m_relLoadSize);
    m_beamModel->setBeamLoadSize(this->getWorkspace() * m_relLoadSize);
    
    // Update tactile force size
    double loadSize = m_beamModel->getLoadSize();
    m_tactileForce->setSize(loadSize * 0.6, loadSize * 0.6 * 0.20);
    m_tactileForce->setRadius(loadSize * 0.055, loadSize * 0.035);
    
    // Update cursor sizes
    m_sphereCursor->setRadius(m_beamModel->getNodeSize());
    m_cubeCursor->setSize(m_beamModel->getNodeSize() * 1.5);
    
    // Update axis labels
    this->updateAxisLabels();
    
    // Mark for recalculation and redraw
    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemViewWindow::assignMaterialToSelected()
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
                vfem::Beam *visBeam = static_cast<vfem::Beam *>(shape);
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

void FemViewWindow::removeMaterialFromSelected()
{
    // Remove materials from selected shapes

    auto selected = this->getSelectedShapes();
    for (int i = 0; i < selected->getSize(); i++)
    {
        auto shape = selected->getChild(i);
        if (shape->isClass("vfem::Beam"))
        {
            vfem::Beam *visBeam = static_cast<vfem::Beam *>(shape);
            visBeam->getBeam()->setMaterial(nullptr);
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemViewWindow::deleteBeamLoad(ofem::BeamLoad *elementLoad)
{
    // Delete a beam load

    // Get ivf representation from element load

    vfem::BeamLoad *visBeamLoad = static_cast<vfem::BeamLoad *>(elementLoad->getUser());

    // Remove shape from scene and delete it

    ivf::ShapePtr shape = this->getScene()->getComposite()->removeShape(visBeamLoad);

    // Remove load from beam model

    m_needRecalc = true;
    m_beamModel->getElementLoadSet()->removeLoad(elementLoad);
}

void FemViewWindow::deleteSelected()
{
    // Delete things in an orderly fashion

    this->snapShot();

    m_propWindow->setBeam(nullptr);
    m_propWindow->setNode(nullptr);

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

void FemViewWindow::addBeamLoad(ofem::BeamLoad *elementLoad)
{
    // Add a beam load

    // Create ivf represenation

    auto visLoad = vfem::BeamLoad::create();

    visLoad->setBeamModel(m_beamModel.get());
    visLoad->setBeamLoad(elementLoad);

    // Set user property of element load to point to
    // our ivf representation

    elementLoad->setUser(static_cast<void *>(visLoad));

    // Initialize ivf representation

    visLoad->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(visLoad);
}

void FemViewWindow::addNodeLoad(ofem::BeamNodeLoad *nodeLoad)
{
    // Add a node load

    // Create ivf represenation

    auto visNodeLoad = vfem::NodeLoad::create();

    visNodeLoad->setBeamModel(m_beamModel.get());
    visNodeLoad->setNodeLoad(nodeLoad);

    // Set user property of element load to point to
    // our ivf representation

    nodeLoad->setUser(static_cast<void *>(visNodeLoad));

    // Initialize ivf representation

    visNodeLoad->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(visNodeLoad);
}

void FemViewWindow::addLastNodeToSelection()
{
    int i;

    auto scene = this->getScene()->getComposite();

    for (i = scene->getSize() - 1; i >= 0; i--)
    {
        auto shape = scene->getChild(i);
        if (shape->isClass("vfem::Node"))
        {
            this->getSelectedShapes()->addChild(shape);
            break;
        }
    }

    redraw();
}

void FemViewWindow::saveScreenShot(std::string filename)
{
    BYTE *pixels = new BYTE[3 * this->width() * this->height()];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, this->width(), this->height(), GL_RGB, GL_UNSIGNED_BYTE, pixels);
    ofutil::saveImage(filename, pixels, this->width(), this->height());
    delete[] pixels;
}

void FemViewWindow::subdivideSelectedBeam()
{
    auto selectedShapes = this->getSelectedShapes();

    for (auto i = 0; i < selectedShapes->getSize(); i++)
    {
        auto shape = selectedShapes->getChild(i);

        if (shape->isClass("vfem::Beam"))
        {
            auto visBeam = static_cast<vfem::Beam *>(shape);

            auto n0 = visBeam->getNode(0)->getFemNode();
            auto n1 = visBeam->getNode(1)->getFemNode();

            double x0, y0, z0;
            double x1, y1, z1;
            double x, y, z;

            n0->getCoord(x0, y0, z0);
            n1->getCoord(x1, y1, z1);

            x = (x0 + x1) * 0.5;
            y = (y0 + y1) * 0.5;
            z = (z0 + z1) * 0.5;

            auto n2 = new ofem::Node(x, y, z);

            auto nodeSet = m_beamModel->getNodeSet();
            auto beamSet = m_beamModel->getElementSet();

            nodeSet->addNode(n2);

            auto beam0 = new ofem::Beam();
            beam0->addNode(n0);
            beam0->addNode(n2);
            beam0->setMaterial(visBeam->getBeam()->getMaterial());

            auto beam1 = new ofem::Beam();
            beam1->addNode(n2);
            beam1->addNode(n1);
            beam1->setMaterial(visBeam->getBeam()->getMaterial());

            beamSet->addElement(beam0);
            beamSet->addElement(beam1);

            auto vn0 = static_cast<vfem::Node *>(n0->getUser());
            auto vn1 = static_cast<vfem::Node *>(n1->getUser());

            auto vn2 = new vfem::Node();
            vn2->setBeamModel(m_beamModel.get());
            vn2->setFemNode(n2);
            vn2->setPosition(x, y, z);
            vn2->setMaterial(m_nodeMaterial);
            vn2->nodeLabel()->setSize(float(m_beamModel->getNodeSize() * 1.5));
            vn2->setDirectRefresh(true);
            n2->setUser(static_cast<void *>(vn2));

            vfem::Beam *vbeam0 = new vfem::Beam();
            vbeam0->setBeamModel(m_beamModel.get());
            vbeam0->setBeam(beam0);
            beam0->setUser(static_cast<void *>(vbeam0));

            vfem::Beam *vbeam1 = new vfem::Beam();
            vbeam1->setBeamModel(m_beamModel.get());
            vbeam1->setBeam(beam1);
            beam1->setUser(static_cast<void *>(vbeam1));

            // Initialize the representation

            vbeam0->setNodes(vn0, vn2);
            vbeam0->refresh();

            vbeam1->setNodes(vn2, vn1);
            vbeam1->refresh();

            // We need a recalc

            m_needRecalc = true;

            this->addToScene(vbeam0);
            this->addToScene(vbeam1);
            this->addToScene(vn2);
        }
    }
    this->setDeleteFilter(DeleteMode::Elements);
    this->deleteSelected();
    this->setDeleteFilter(DeleteMode::All);
}

void FemViewWindow::meshSelectedNodes()
{
    this->snapShot();

    m_tetMesher->clear();

    double x, y, z;

    auto selected = this->getSelectedShapes();
    for (int i = 0; i < selected->getSize(); i++)
    {
        auto shape = selected->getChild(i);
        if (shape->isClass("vfem::Node"))
        {
            vfem::Node *visNode = static_cast<vfem::Node *>(shape);
            ofem::Node *femNode = visNode->getFemNode();

            femNode->getCoord(x, y, z);
            m_tetMesher->addNode(femNode->getNumber(), x, y, z);
        }
    }

    if (m_tetMesher->nodeCount() != 0)
    {
        m_tetMesher->generate();

        for (auto i = 0; i < m_tetMesher->edges().count(); i++)
        {
            auto edge = m_tetMesher->edges().at(i);
            this->addBeam(edge.i0() - 1, edge.i1() - 1);
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemViewWindow::surfaceSelectedNodes(bool groundElements)
{
    this->snapShot();

    m_tetMesher->clear();

    double x, y, z;

    auto selected = this->getSelectedShapes();
    for (int i = 0; i < selected->getSize(); i++)
    {
        auto shape = selected->getChild(i);
        if (shape->isClass("vfem::Node"))
        {
            vfem::Node *visNode = static_cast<vfem::Node *>(shape);
            ofem::Node *femNode = visNode->getFemNode();

            femNode->getCoord(x, y, z);
            m_tetMesher->addNode(femNode->getNumber(), x, y, z);
        }
    }

    if (m_tetMesher->nodeCount() != 0)
    {
        m_tetMesher->generate();

        double x, z;
        double y0, y1, y2;

        for (auto i = 0; i < m_tetMesher->faces().count(); i++)
        {
            auto face = m_tetMesher->faces().at(i);

            if (groundElements)
            {
                this->addBeam(face.i0() - 1, face.i1() - 1);
                this->addBeam(face.i1() - 1, face.i2() - 1);
                this->addBeam(face.i2() - 1, face.i0() - 1);
            }
            else
            {
                auto n0 = m_beamModel->getNodeSet()->getNode(face.i0() - 1);
                auto n1 = m_beamModel->getNodeSet()->getNode(face.i1() - 1);
                auto n2 = m_beamModel->getNodeSet()->getNode(face.i2() - 1);

                n0->getCoord(x, y0, z);
                n1->getCoord(x, y1, z);
                n2->getCoord(x, y2, z);

                if (!(is_equal(y0, 0.0) && is_equal(y1, 0.0)))
                    this->addBeam(face.i0() - 1, face.i1() - 1);

                if (!(is_equal(y1, 0.0) && is_equal(y2, 0.0)))
                    this->addBeam(face.i1() - 1, face.i2() - 1);

                if (!(is_equal(y2, 0.0) && is_equal(y0, 0.0)))
                    this->addBeam(face.i2() - 1, face.i0() - 1);
            }
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemViewWindow::addNodeBC(ofem::BeamNodeBC *bc)
{
    // Add a node load

    // Create ivf represenation

    vfem::NodeBC *visNodeBC = new vfem::NodeBC();
    visNodeBC->setBeamModel(m_beamModel.get());
    visNodeBC->setNodeBC(bc);

    // Set user property of element load to point to
    // our ivf representation

    bc->setUser(static_cast<void *>(visNodeBC));

    // Initialize ivf representation

    visNodeBC->refresh();

    // Add representation to scene

    m_needRecalc = true;

    this->addToScene(visNodeBC);
}

void FemViewWindow::showBeamLoads()
{}

void FemViewWindow::assignBeamLoadSelected()
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
                vfem::Beam *visBeam = static_cast<vfem::Beam *>(shape);
                m_currentElementLoad->addElement((ofem::Element *)visBeam->getBeam());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

void FemViewWindow::assignNodeLoadSelected()
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
                vfem::Node *visNode = static_cast<vfem::Node *>(shape);
                m_currentNodeLoad->addNode(static_cast<ofem::Node *>(visNode->getFemNode()));
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

void FemViewWindow::deleteNodeLoad(ofem::BeamNodeLoad *nodeLoad)
{
    // Delete a node load

    // Get ivf representation from element load

    vfem::NodeLoadPtr visNodeLoad = static_cast<vfem::NodeLoad *>(nodeLoad->getUser());

    // Remove shape from scene and delete it

    this->getScene()->getComposite()->removeShape(visNodeLoad);

    // Remove load from beam model

    setCurrentNodeLoad(nullptr);

    m_beamModel->getNodeLoadSet()->removeLoad(nodeLoad);
}

void FemViewWindow::deleteNodeBC(ofem::BeamNodeBC *bc)
{
    // Delete a node load

    // Get ivf representation from element load

    vfem::NodeBC *visNodeBC = static_cast<vfem::NodeBC *>(bc->getUser());

    // Remove shape from scene and delete it

    this->getScene()->getComposite()->removeShape(visNodeBC);
    delete visNodeBC;

    // Remove load from beam model

    m_needRecalc = true;
    m_beamModel->getNodeBCSet()->removeBC(bc);
    setCurrentNodeBC(nullptr);
}

void FemViewWindow::setRotationSelected(double rotation)
{
    // Assigns a material to selected shapes

    auto selected = this->getSelectedShapes();
    for (int i = 0; i < selected->getSize(); i++)
    {
        auto shape = selected->getChild(i);
        if (shape->isClass("vfem::Beam"))
        {
            vfem::Beam *visBeam = static_cast<vfem::Beam *>(shape);
            visBeam->getBeam()->setBeamRotation(rotation);
        }
    }

    // Shapes has to be refreshed to represent the
    // the changes

    this->set_changed();
    this->redraw();
}

void FemViewWindow::setupOverlay()
{
    PlaneButtonPtr button;

    m_editArea = Area2D::create();
    m_editArea->add(0, 0);
    m_editArea->add(65, 0);
    m_editArea->add(65, 600);
    m_editArea->add(0, 600);
    m_editArea->setColor(0, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(1, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(2, 0.0f, 0.0f, 0.0f);
    m_editArea->setColor(3, 0.0f, 0.0f, 0.0f);
    m_areas.push_back(m_editArea);

    m_objectArea = Area2D::create();
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

    namespace fs = std::filesystem;

    m_editButtons = ButtonGroup::create();

    button = PlaneButton::create(ToolbarButton::Select, (m_imagePath / fs::path("tlselect.png")).string());
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 60.0, 0.0);
    button->setHint("Select nodes or elements");
    m_editButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::SelectBox, (m_imagePath / fs::path("tlselectbox.png")).string());
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 120.0, 0.0);
    button->setHint("Select nodes or elements");
    m_editButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::Move, (m_imagePath / fs::path("tlmove.png")).string());
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 200.0, 0.0);
    button->setHint("Move nodes or elements");
    m_editButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::Inspect, (m_imagePath / fs::path("tlinspect.png")).string());
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 270, 0.0);
    button->setHint("Node or element info");
    m_editButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::Delete, (m_imagePath / fs::path("tldelete.png")).string());
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 360.0, 0.0);
    button->setHint("Delete node or element");
    m_editButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::Feedback, (m_imagePath / fs::path("tlfeedback.png")).string());
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 440.0, 0.0);
    button->setHint("Feedback mode");
    m_editButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::Run, (m_imagePath / fs::path("run.png")).string());
    button->setSize(40.0, 40.0);
    button->setPosition(30.0, 520.0, 0.0);
    button->setHint("Excecute calculation");
    m_editButtons->addChild(button);

    // m_overlayScene->addChild(m_editButtons);

    //
    // Create object toolbar
    //

    m_objectButtons = ButtonGroup::create();

    button = PlaneButton::create(ToolbarButton::CreateNode, (m_imagePath / fs::path("tlnode.png")).string());
    button->setSize(50.0, 50.0);
    button->setPosition(30.0, 30.0, 0.0);
    button->setHint("Create node");
    m_objectButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::CreateBeam, (m_imagePath / fs::path("tlsolidline.png")).string());
    button->setSize(50.0, 50.0);
    button->setPosition(90.0, 30.0, 0.0);
    button->setHint("Create element");
    m_objectButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::NodeLoad, (m_imagePath / fs::path("tlnodeloads.png")).string());
    button->setSize(50.0, 50.0);
    button->setPosition(150.0, 30.0, 0.0);
    button->setHint("Show node loads");
    m_objectButtons->addChild(button);

    button = PlaneButton::create(ToolbarButton::BeamLoad, (m_imagePath / fs::path("tldload.png")).string());
    button->setSize(50.0, 50.0);
    button->setPosition(210.0, 30.0, 0.0);
    button->setHint("Show element loads");
    m_objectButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::NodeBC, (m_imagePath / fs::path("tlbc.png")).string());
    button->setSize(50.0, 50.0);
    button->setPosition(270.0, 30.0, 0.0);
    button->setHint("Show boundary conditions");
    m_objectButtons->addChild(button);

    button = new PlaneButton(ToolbarButton::Materials, (m_imagePath / fs::path("tlmaterials.png")).string());
    button->setSize(50.0, 50.0);
    button->setPosition(330.0, 30.0, 0.0);
    button->setHint("Show beam properties");
    m_objectButtons->addChild(button);

    m_logoButton = new PlaneButton(1234, (m_imagePath / fs::path("logo.png")).string());
    m_logoButton->setSize(120.0, 120.0);

    m_overlayScene->addChild(m_logoButton);
    // m_overlayScene->addChild(m_objectButtons);
}

void FemViewWindow::setupScript(chaiscript::ChaiScript &script)
{
    // Add math library to script environment

    auto mathlib = chaiscript::extras::math::bootstrap();
    script.add(mathlib);

    // Bind ObjectiveFrame specific functions

    script.add(chaiscript::fun(&FemViewWindow::addNode, this), "addNode");
    script.add(chaiscript::fun(&FemViewWindow::newModel, this), "newModel");
    script.add(chaiscript::fun(&FemViewWindow::addBeam, this), "addBeam");
    script.add(chaiscript::fun(&FemViewWindow::nodeCount, this), "nodeCount");
    script.add(chaiscript::fun(&FemViewWindow::meshSelectedNodes, this), "meshSelectedNodes");
    script.add(chaiscript::fun(&FemViewWindow::surfaceSelectedNodes, this), "surfaceSelectedNodes");
    script.add(chaiscript::fun(&FemViewWindow::selectAll, this), "selectAll");
    script.add(chaiscript::fun(&FemViewWindow::selectAllNodes, this), "selectAllNodes");
    script.add(chaiscript::fun(&FemViewWindow::clearSelection, this), "clearSelection");
    script.add(chaiscript::fun(&FemViewWindow::assignNodeFixedBCGround, this), "assignNodeFixedBCGround");
    script.add(chaiscript::fun(&FemViewWindow::assignNodePosBCGround, this), "assignNodePosBCGround");
    script.add(chaiscript::fun(&FemViewWindow::addLastNodeToSelection, this), "addLastNodeToSelection");
    script.add(chaiscript::fun(&FemViewWindow::nodePos, this), "nodeCoord");
    script.add(chaiscript::fun(&FemViewWindow::updateNodePos, this), "updateNodePos");
    script.add(chaiscript::fun(&FemViewWindow::nodePosAt, this), "nodePosAt");
    script.add(chaiscript::fun(&FemViewWindow::updateNodePosAt, this), "updateNodePosAt");
    script.add(chaiscript::fun(&FemViewWindow::nodeAt, this), "nodeAt");
    script.add(chaiscript::fun(&FemViewWindow::isNodeSelected, this), "isNodeSelected");
    script.add(chaiscript::fun(&FemViewWindow::isNodeSelectedAt, this), "isNodeSelectedAt");
    script.add(chaiscript::fun(&FemViewWindow::randFloat, this), "randFloat");
    script.add(chaiscript::fun(&FemViewWindow::randInt, this), "randInt");
    script.add(chaiscript::fun(&FemViewWindow::randSeed, this), "randSeed");
    script.add(chaiscript::fun(&FemViewWindow::addNodeWithIdx, this), "addNodeWithIdx");
    script.add(chaiscript::fun(&FemViewWindow::addBeamWithIdx, this), "addBeamWithIdx");
    script.add(chaiscript::fun(&FemViewWindow::beamCount, this), "beamCount");
    script.add(chaiscript::fun(&FemViewWindow::beamAt, this), "beamAt");
    script.add(chaiscript::fun(&FemViewWindow::updateBeamAt, this), "updateBeamAt");
}

void FemViewWindow::setupPlugins()
{
    log("Setting up plugins...");

    namespace fs = std::filesystem;

    if (std::filesystem::is_directory(m_pluginPath))
    {
        for (const auto &entry : std::filesystem::directory_iterator(m_pluginPath))
        {
            auto filename = entry.path();
            auto plugin = ScriptPlugin::create(filename.string());
            m_plugins.push_back(plugin);
            log("Loading plugin - " + filename.string() + " - " + plugin->name());
        }

        std::sort(m_plugins.begin(), m_plugins.end(),
                  [](const ScriptPluginPtr a, const ScriptPluginPtr b) -> bool { return a->name() > b->name(); });
    }
    else
        log("Couldn't find load any plugins...");
}

void FemViewWindow::setupExamples()
{
    log("Setting up examples...");

    namespace fs = std::filesystem;

    if (std::filesystem::is_directory(m_examplePath))
    {
        for (const auto &entry : std::filesystem::directory_iterator(m_examplePath))
        {
            auto filename = entry.path();
            if (filename.extension() == ".df3")
            {
                m_examples.push_back(filename.string());
                log("Found example - " + filename.string());
            }
        }

        std::sort(m_examples.begin(), m_examples.end());
    }
    else
        log("Couldn't find load any examples...");
}

void FemViewWindow::setupAi()
{
    log("Setting up system prompt...");

    namespace fs = std::filesystem;

    if (std::filesystem::is_directory(m_aiPath))
    {
        auto filename = m_aiPath / fs::path("system_prompt.md");
        if (std::filesystem::exists(filename))
        {
            m_systemPromptFilename = filename.string();
            m_structureGenerator.loadSystemPromptFrom(m_systemPromptFilename);
            log("Loaded system prompt - " + m_systemPromptFilename);
        }
        else
        {
            log("Couldn't find system prompt file...");
            m_systemPromptFilename = "";
        }

        filename = m_aiPath / fs::path("prompts_db.json");
        if (std::filesystem::exists(filename))
        {
            m_promptDatabase.loadFromFile(filename.string());
            log("Loaded prompts database - " + filename.string());
        }
        else
        {
            log("Couldn't find prompts database file...");
        }
    }
    else
        log("Couldn't find AI directory");
}

void FemViewWindow::refreshUiStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImGuiIO &io = ImGui::GetIO();

    ImGuiStyle newStyle;

    newStyle.FrameRounding = 4;
    newStyle.WindowRounding = 8;
    newStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);
    newStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);
    newStyle.Colors[ImGuiCol_TitleBgActive] =
        ImVec4(newStyle.Colors[ImGuiCol_TitleBgActive].x, newStyle.Colors[ImGuiCol_TitleBgActive].y,
               newStyle.Colors[ImGuiCol_TitleBgActive].z, 0.8f);

    newStyle.ScaleAllSizes(m_uiScale);

    io.FontGlobalScale = m_uiScale;

    style = newStyle;
}

void FemViewWindow::assignNodeBCSelected()
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
                vfem::Node *visNode = static_cast<vfem::Node *>(shape);
                m_currentNodeBC->addNode((ofem::Node *)visNode->getFemNode());
            }
        }

        // Shapes has to be refreshed to represent the
        // the changes

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

void FemViewWindow::assignNodeFixedBCSelected()
{
    this->snapShot();
    this->setCurrentNodeBC(m_beamModel->defaultNodeFixedBC());
    this->assignNodeBCSelected();
    this->setCurrentNodeBC(nullptr);
}

void FemViewWindow::assignNodePosBCSelected()
{
    this->snapShot();
    this->setCurrentNodeBC(m_beamModel->defaultNodePosBC());
    this->assignNodeBCSelected();
    this->setCurrentNodeBC(nullptr);
}

void FemViewWindow::assignNodeFixedBCGround()
{
    this->snapShot();
    this->clearSelection();
    this->setSelectFilter(SelectMode::GroundNodes);
    this->selectAll();
    this->assignNodeFixedBCSelected();
}

void FemViewWindow::assignNodePosBCGround()
{
    this->snapShot();
    this->clearSelection();
    this->setSelectFilter(SelectMode::GroundNodes);
    this->selectAll();
    this->assignNodePosBCSelected();
}

void FemViewWindow::executeCalc()
{
    // m_frameSolver = FrameSolver::create();
    // m_currentSolver = m_frameSolver.get();
    m_beamSolver = BeamSolver::create();
    m_currentSolver = m_beamSolver.get();

    m_currentSolver->setBeamModel(m_beamModel.get());
    // m_currentSolver->setResultInfo(m_beamModel->getResultInfo());
    m_currentSolver->execute();

    if (m_currentSolver->modelState() != ModelState::Ok)
    {
        switch (m_currentSolver->modelState())
        {
        case ModelState::NoNodes:
            this->showMessage("No nodes defined. \nCalculation not executed.");
            break;
        case ModelState::NoElements:
            this->showMessage("No elements defined. \nCalculation not executed.");
            break;
        case ModelState::NoBC:
            this->showMessage("No boundary conditions defined. \nCalculation not executed.");
            break;
        case ModelState::NoLoads:
            this->showMessage("No loads defined. \nCalculation not executed.");
            break;
        case ModelState::Unstable:
            this->showMessage("System unstable. Try adding boundary conditions.\nCalculation not executed.");
            // Automatically compute eigenmodes for unstable structures
            this->computeEigenmodes(5);
            break;
        case ModelState::Singular:
            this->showMessage(
                "System is singular. Check for free nodes or other strange things. \nCalculation not executed.");
            break;
        case ModelState::Invalid:
            this->showMessage("This should not happen.\nCalculation not executed.");
            break;
        case ModelState::UndefinedMaterial:
            this->showMessage("Beams without materials found.");
            break;
        case ModelState::SolveFailed:
            this->showMessage("Solver failed.");
            // Try computing eigenmodes to diagnose the problem
            this->computeEigenmodes(5);
            break;
        case ModelState::RecomputeFailed:
            this->showMessage("Recomputation failed.");
            break;
        case ModelState::SetupFailed:
            this->showMessage("Solver setup failed.");
            break;
        default:
            this->showMessage("Unhandled error.\nCalculation not executed.");
            break;
        }
        m_needRecalc = true;
    }
    else
        m_needRecalc = false;

    auto maxNodeValue = m_currentSolver->getMaxNodeValue();
    auto maxReactionForce = m_currentSolver->getMaxReactionForce();
    auto maxReactionMoment = m_currentSolver->getMaxReactionMoment();

    m_beamModel->setMaxReactionForce(maxReactionForce);
    m_beamModel->setMaxReactionMoment(maxReactionMoment);

    // Calculate default scalefactor

    log("Max node value = " + std::to_string(maxNodeValue));

    if (!m_lockScaleFactor)
    {
        if (maxNodeValue > 0.0)
            m_beamModel->setScaleFactor(this->getWorkspace() * 0.005 / maxNodeValue);
        else
            m_beamModel->setScaleFactor(1.0);
    }

    m_settingsWindow->update();
    m_scaleWindow->show();
    m_scaleWindow->setPosition(100, 20);

    m_loadMixerWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet *)m_beamModel->getNodeLoadSet());
    m_loadMixerWindow->show();
    m_loadMixerWindow->setPosition(100, 240);

    // Show displacements

    this->setRepresentation(RepresentationMode::Results);
}

void FemViewWindow::recompute()
{
    // Is there a calculation ?

    if (m_needRecalc)
    {
        double maxNodeValue = 0.0;

        // m_frameSolver = FrameSolver::create();
        // m_currentSolver = m_frameSolver.get();
        m_beamSolver = BeamSolver::create();
        m_currentSolver = m_beamSolver.get();

        m_currentSolver->setBeamModel(m_beamModel.get());

        // Setup feedback force

        m_currentSolver->execute();

        // We assume the worst case

        m_saneModel = false;

        switch (m_currentSolver->modelState())
        {
        case ModelState::NoNodes:
            break;
        case ModelState::NoElements:
            break;
        case ModelState::NoBC:
            break;
        case ModelState::NoLoads:
            break;
        case ModelState::Unstable:
            break;
        case ModelState::Singular:
            break;
        case ModelState::Invalid:
            break;
        case ModelState::UndefinedMaterial:
            break;
        case ModelState::SolveFailed:
            break;
        case ModelState::RecomputeFailed:
            break;
        case ModelState::SetupFailed:
            break;
        default:
            m_saneModel = true;
            break;
        }

        maxNodeValue = m_currentSolver->getMaxNodeValue();

        auto maxReactionForce = m_currentSolver->getMaxReactionForce();
        auto maxReactionMoment = m_currentSolver->getMaxReactionMoment();

        m_beamModel->setMaxReactionForce(maxReactionForce);
        m_beamModel->setMaxReactionMoment(maxReactionMoment);

        // Only compute the scale factor at the first attempt

        if (!m_lockScaleFactor)
        {
            if (!m_haveScaleFactor)
            {
                m_beamModel->setScaleFactor(this->getWorkspace() * 0.020 / m_currentSolver->getMaxNodeValue());
                m_haveScaleFactor = true;
            }
        }

        m_needRecalc = false;

        // Show displacements

        this->setRepresentation(RepresentationMode::Results);
    }

    // Continuosly recompute solution

    if (m_saneModel)
    {
        if (m_currentSolver != nullptr)
        {

            // Setup feedback force

            // Execute calculation

            m_currentSolver->recompute();
            m_currentSolver->update(); // NEW

            auto maxReactionForce = m_currentSolver->getMaxReactionForce();
            auto maxReactionMoment = m_currentSolver->getMaxReactionMoment();

            m_beamModel->setMaxReactionForce(maxReactionForce);
            m_beamModel->setMaxReactionMoment(maxReactionMoment);

            // Only compute the scale factor at the first attempt

            if (!m_lockScaleFactor)
            {
                if (!m_haveScaleFactor)
                {
                    m_beamModel->setScaleFactor(this->getWorkspace() * 0.020 / m_currentSolver->getMaxNodeValue());
                    m_haveScaleFactor = true;
                }
            }

            // Refresh scene (Solid lines must be updated)

            this->getScene()->getComposite()->refresh();
            this->redraw();
        }
    }
}

void FemViewWindow::computeEigenmodes(int numModes)
{
    if (m_currentSolver == nullptr)
    {
        log("No solver available. Run analysis first.");
        return;
    }

    log("Computing " + std::to_string(numModes) + " eigenmodes...");

    if (m_currentSolver->computeEigenModes(numModes))
    {
        log("Successfully computed " + std::to_string(m_currentSolver->getNumEigenModes()) + " eigenmodes.");
        
        // Update eigenmode window
        m_eigenmodeWindow->setHasEigenmodes(true);
        m_eigenmodeWindow->setNumModes(m_currentSolver->getNumEigenModes());
        m_eigenmodeWindow->show();
        m_windowList->placeWindow(m_eigenmodeWindow);
        
        // Display eigenvalue information
        for (int i = 0; i < m_currentSolver->getNumEigenModes(); i++)
        {
            double eigenvalue = m_currentSolver->getEigenValue(i);
            std::string status = eigenvalue < 0 ? " (UNSTABLE)" : "";
            log("  Mode " + std::to_string(i + 1) + ": eigenvalue = " + std::to_string(eigenvalue) + status);
        }
        
        // Set visualization to first mode
        setEigenmodeVisualization(0);
        
        // Switch to results visualization mode
        this->setRepresentation(RepresentationMode::Results);
    }
    else
    {
        log("Failed to compute eigenmodes.");
        m_eigenmodeWindow->setHasEigenmodes(false);
    }
}

void FemViewWindow::clearEigenmodes()
{
    if (m_currentSolver == nullptr)
    {
        log("No solver available.");
        return;
    }

    log("Clearing eigenmodes...");
    m_currentSolver->clearEigenModes();
    m_eigenmodeWindow->setHasEigenmodes(false);
    
    // Clear node displacements
    m_beamModel->clearNodeValues();
    this->set_changed();
    this->redraw();
}

void FemViewWindow::setEigenmodeVisualization(int mode)
{
    if (m_currentSolver == nullptr || !m_currentSolver->hasEigenModes())
    {
        log("No eigenmodes available.");
        return;
    }

    if (mode < 0 || mode >= m_currentSolver->getNumEigenModes())
    {
        log("Invalid eigenmode index: " + std::to_string(mode));
        return;
    }

    log("Visualizing eigenmode " + std::to_string(mode + 1));

    // Get the eigenvector for this mode
    Eigen::VectorXd eigenvector;
    m_currentSolver->getEigenVector(mode, eigenvector);

    // Apply eigenvector to node displacements
    auto nodeSet = m_beamModel->getNodeSet();
    
    double maxDisplacement = 0.0;
    
    // First pass: find max displacement for scaling
    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        
        if (node->getKind() != ofem::nkNotConnected)
        {
            for (int j = 0; j < 3; j++)
            {
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        double disp = std::abs(eigenvector(dofNum));
                        if (disp > maxDisplacement)
                            maxDisplacement = disp;
                    }
                }
            }
        }
    }

    // Second pass: apply normalized displacements
    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        
        if (node->getKind() == ofem::nk6Dof)
        {
            node->setValueSize(12);
            for (int j = 0; j < 6; j++)
            {
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        double value = eigenvector(dofNum);
                        if (maxDisplacement > 0)
                            value /= maxDisplacement; // Normalize
                        node->setValue(j, value);
                    }
                    else
                        node->setValue(j, 0.0);
                }
                else
                    node->setValue(j, 0.0);
                    
                // Clear reaction forces for eigenmode visualization
                node->setValue(j + 6, 0.0);
            }
        }
        else if (node->getKind() == ofem::nk3Dof)
        {
            node->setValueSize(6);
            for (int j = 0; j < 3; j++)
            {
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        double value = eigenvector(dofNum);
                        if (maxDisplacement > 0)
                            value /= maxDisplacement; // Normalize
                        node->setValue(j, value);
                    }
                    else
                        node->setValue(j, 0.0);
                }
                else
                    node->setValue(j, 0.0);
                    
                // Clear reaction forces
                node->setValue(j + 3, 0.0);
            }
        }
    }

    // Update scale factor for better visualization
    if (!m_lockScaleFactor && maxDisplacement > 0)
    {
        double eigenScaleFactor = this->getWorkspace() * 0.1; // 10% of workspace
        m_beamModel->setScaleFactor(eigenScaleFactor);
    }

    this->set_changed();
    this->redraw();
}

void FemViewWindow::updateEigenmodeVisualization(float phase)
{
    if (m_currentSolver == nullptr || !m_currentSolver->hasEigenModes())
        return;

    int currentMode = m_eigenmodeWindow->getCurrentMode();
    double scaleFactor = std::sin(phase) * m_eigenmodeWindow->getModeScaleFactor();

    // Get the eigenvector for this mode
    Eigen::VectorXd eigenvector;
    m_currentSolver->getEigenVector(currentMode, eigenvector);

    auto nodeSet = m_beamModel->getNodeSet();
    
    double maxDisplacement = 0.0;
    
    // Find max displacement for scaling
    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        
        if (node->getKind() != ofem::nkNotConnected)
        {
            for (int j = 0; j < 3; j++)
            {
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        double disp = std::abs(eigenvector(dofNum));
                        if (disp > maxDisplacement)
                            maxDisplacement = disp;
                    }
                }
            }
        }
    }

    // Apply animated displacements
    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        
        if (node->getKind() == ofem::nk6Dof)
        {
            for (int j = 0; j < 6; j++)
            {
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        double value = eigenvector(dofNum) * scaleFactor;
                        if (maxDisplacement > 0)
                            value /= maxDisplacement;
                        node->setValue(j, value);
                    }
                }
            }
        }
        else if (node->getKind() == ofem::nk3Dof)
        {
            for (int j = 0; j < 3; j++)
            {
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        double value = eigenvector(dofNum) * scaleFactor;
                        if (maxDisplacement > 0)
                            value /= maxDisplacement;
                        node->setValue(j, value);
                    }
                }
            }
        }
    }

    this->set_changed();
    this->redraw();
}

void FemViewWindow::selectAllNodes()
{
    setSelectFilter(SelectMode::Nodes);
    selectAll();
}

void FemViewWindow::selectAllElements()
{
    setSelectFilter(SelectMode::Elements);
    selectAll();
}

void FemViewWindow::doFeedback()
{
    // Is there a calculation ?

    if (m_needRecalc)
    {
        if (m_interactionNode != nullptr)
        {
            double maxNodeValue = 0.0;

            // m_frameSolver = FrameSolver::create();
            // m_currentSolver = m_frameSolver.get();

            m_beamSolver = BeamSolver::create();
            m_currentSolver = m_beamSolver.get();

            // m_currentSolver->setResultInfo(m_beamModel->getResultInfo());

            m_currentSolver->setBeamModel(m_beamModel.get());

            double v[3];

            // Setup feedback force

            m_tactileForce->getDirection(v[0], v[1], v[2]);
            m_currentSolver->setFeedbackForce(m_interactionNode->getFemNode(), m_tactileForceValue * v[0],
                                              m_tactileForceValue * v[1], m_tactileForceValue * v[2]);

            m_currentSolver->execute();

            // We assume the worst case

            m_saneModel = false;

            switch (m_currentSolver->modelState())
            {
            case ModelState::NoNodes:
                this->showMessage("No nodes defined. \nCalculation not executed.");
                break;
            case ModelState::NoElements:
                this->showMessage("No elements defined. \nCalculation not executed.");
                break;
            case ModelState::NoBC:
                this->showMessage("No boundary conditions defined. \nCalculation not executed.");
                break;
            case ModelState::NoLoads:
                this->showMessage("No loads defined. \nCalculation not executed.");
                break;
            case ModelState::Unstable:
                this->showMessage("System unstable. Try adding boundary conditions.\nCalculation not executed.");
                break;
            case ModelState::Singular:
                this->showMessage(
                    "System is singular. Check for free nodes or other strange things. \nCalculation not executed.");
                break;
            case ModelState::Invalid:
                this->showMessage("This should not happen.\nCalculation not executed.");
                break;
            case ModelState::UndefinedMaterial:
                this->showMessage("Beams without materials found.");
                break;
            case ModelState::SolveFailed:
                this->showMessage("Solver failed.");
                break;
            case ModelState::RecomputeFailed:
                this->showMessage("Recomputation failed.");
                break;
            case ModelState::SetupFailed:
                this->showMessage("Solver setup failed.");
                break;
            default:
                m_saneModel = true;
                break;
            }

            maxNodeValue = m_currentSolver->getMaxNodeValue();

            auto maxReactionForce = m_currentSolver->getMaxReactionForce();
            auto maxReactionMoment = m_currentSolver->getMaxReactionMoment();

            m_beamModel->setMaxReactionForce(maxReactionForce);
            m_beamModel->setMaxReactionMoment(maxReactionMoment);

            // Only compute the scale factor at the first attempt

            if (!m_lockScaleFactor)
            {
                if (!m_haveScaleFactor)
                {
                    m_beamModel->setScaleFactor(this->getWorkspace() * 0.020 / m_currentSolver->getMaxNodeValue());
                    m_haveScaleFactor = true;
                }
            }

            m_needRecalc = false;

            // Show displacements

            this->setRepresentation(RepresentationMode::Results);
        }
    }

    // Continuosly recompute solution

    if (m_saneModel)
    {
        if (m_currentSolver != nullptr)
        {
            if (m_interactionNode != nullptr)
            {

                double v[3];

                // Setup feedback force

                m_tactileForce->getDirection(v[0], v[1], v[2]);
                m_currentSolver->setFeedbackForce(m_interactionNode->getFemNode(), m_tactileForceValue * v[0],
                                                  m_tactileForceValue * v[1], m_tactileForceValue * v[2]);

                // Execute calculation

                m_currentSolver->recompute();
                m_currentSolver->update(); // NEW

                auto maxReactionForce = m_currentSolver->getMaxReactionForce();
                auto maxReactionMoment = m_currentSolver->getMaxReactionMoment();

                m_beamModel->setMaxReactionForce(maxReactionForce);
                m_beamModel->setMaxReactionMoment(maxReactionMoment);

                // Only compute the scale factor at the first attempt

                if (!m_lockScaleFactor)
                {
                    if (!m_haveScaleFactor)
                    {
                        m_beamModel->setScaleFactor(this->getWorkspace() * 0.020 / m_currentSolver->getMaxNodeValue());
                        m_haveScaleFactor = true;
                    }
                }

                // Update tactile force icon

                double x, y, z;
                m_interactionNode->getDisplacedPosition(x, y, z);
                m_tactileForce->setPosition(x, y, z);

                // Refresh scene (Solid lines must be updated)

                this->getScene()->getComposite()->refresh();
                this->redraw();
            }
        }
    }
    else
    {
        setEditMode(WidgetMode::Select);
        refreshToolbars();
    }
}

vfem::Node *FemViewWindow::addNode(double x, double y, double z)
{
    // First we create a FemNode

    auto femNode = ofem::Node::create();

    // Add it to the Fem model

    m_beamModel->getNodeSet()->addNode(femNode);
    femNode->setNumber(static_cast<long>(m_beamModel->getNodeSet()->getSize()) - 1);

    // Create Ivf representation

    vfem::Node *ivfNode = new vfem::Node();
    ivfNode->setBeamModel(m_beamModel.get());
    ivfNode->setFemNode(femNode);
    ivfNode->setPosition(x + m_selectedPos[0], y + m_selectedPos[1], z + m_selectedPos[2]);
    ivfNode->setMaterial(m_nodeMaterial);
    ivfNode->setDirectRefresh(true);
    ivfNode->nodeLabel()->setSize(float(m_beamModel->getNodeSize() * 1.5));

    femNode->setUser(static_cast<void *>(ivfNode));

    this->getScene()->addChild(ivfNode);
    return ivfNode;
}

vfem::Beam *FemViewWindow::addBeam(int i0, int i1)
{
    auto nodeSet = m_beamModel->getNodeSet();
    auto beamSet = m_beamModel->getElementSet();

    auto n0 = nodeSet->getNode(i0);
    auto n1 = nodeSet->getNode(i1);

    // Check for existing beam

    bool beam_exists = false;

    for (auto i = 0; i < beamSet->getSize(); i++)
    {
        auto beam = beamSet->getElement(i);
        auto check_n0 = beam->getNode(0);
        auto check_n1 = beam->getNode(1);

        if ((n0 == check_n0) && (n1 == check_n1))
            beam_exists = true;
        if ((n0 == check_n1) && (n1 == check_n0))
            beam_exists = true;
    }

    if (beam_exists)
    {
        return nullptr;
    }

    if ((n0 != nullptr) && (n1 != nullptr) && (n0 != n1))
    {
        ofem::Beam *beam = new ofem::Beam();
        beam->addNode(n0);
        beam->addNode(n1);
        beam->setMaterial((ofem::BeamMaterial *)m_beamModel->getMaterialSet()->currentMaterial());

        beamSet->addElement(beam);

        auto vNode0 = static_cast<vfem::Node *>(n0->getUser());
        auto vNode1 = static_cast<vfem::Node *>(n1->getUser());

        vfem::Beam *vBeam = new vfem::Beam();
        vBeam->setBeamModel(m_beamModel.get());
        vBeam->setBeam(beam);
        beam->setUser(static_cast<void *>(vBeam));

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

size_t FemViewWindow::addNodeWithIdx(double x, double y, double z)
{
    auto node = addNode(x, y, z);
    return m_beamModel->getNodeSet()->getSize() - 1;
}

size_t FemViewWindow::addBeamWithIdx(int i0, int i1)
{
    auto beam = addBeam(i0, i1);
    return m_beamModel->getElementSet()->getSize() - 1;
}

size_t FemViewWindow::nodeCount()
{
    return m_beamModel->getNodeSet()->getSize();
}

size_t FemViewWindow::beamCount()
{
    return m_beamModel->getElementSet()->getSize();
}

void FemViewWindow::nodePos(vfem::Node *node, double &x, double &y, double &z)
{
    node->getPosition(x, y, z);
}

void FemViewWindow::nodePosAt(int i, double &x, double &y, double &z)
{
    if (i < m_beamModel->getNodeSet()->getSize())
    {
        auto node = m_beamModel->getNodeSet()->getNode(i);
        auto ivfNode = static_cast<vfem::Node *>(node->getUser());
        ivfNode->getPosition(x, y, z);
    }
}

void FemViewWindow::updateNodePos(vfem::Node *node, double x, double y, double z)
{
    node->setPosition(x, y, z);
}

void FemViewWindow::updateNodePosAt(int i, double x, double y, double z)
{
    if (i < m_beamModel->getNodeSet()->getSize())
    {
        auto node = m_beamModel->getNodeSet()->getNode(i);
        auto ivfNode = static_cast<vfem::Node *>(node->getUser());
        ivfNode->setPosition(x, y, z);
    }
}

void FemViewWindow::beamAt(int i, int &i0, int &i1)
{
    auto beam = m_beamModel->getElementSet()->getElement(i);

    auto n0 = beam->getNode(0);
    auto n1 = beam->getNode(1);

    i0 = m_beamModel->getNodeSet()->indexOf(n0);
    i1 = m_beamModel->getNodeSet()->indexOf(n1);
}

void FemViewWindow::updateBeamAt(int i, int i0, int i1)
{
    auto beam = m_beamModel->getElementSet()->getElement(i);
    beam->clear();
    beam->addNode(m_beamModel->getNodeSet()->getNode(i0));
    beam->addNode(m_beamModel->getNodeSet()->getNode(i1));
}

vfem::Node *FemViewWindow::nodeAt(int i)
{
    return static_cast<vfem::Node *>(m_beamModel->getNodeSet()->getNode(i)->getUser());
}

bool FemViewWindow::isNodeSelected(vfem::Node *node)
{
    return node->getSelect() == ivf::Shape::SS_ON;
}

bool FemViewWindow::isNodeSelectedAt(int i)
{
    auto node = m_beamModel->getNodeSet()->getNode(i);
    auto ivfNode = static_cast<vfem::Node *>(node->getUser());
    return ivfNode->getSelect() == ivf::Shape::SS_ON;
}

double FemViewWindow::randFloat(double min, double max)
{
    return ofutil::rand_float(min, max);
}

int FemViewWindow::randInt(int min, int max)
{
    return ofutil::rand_int(min, max);
}

void FemViewWindow::randSeed()
{
    ofutil::rand_seed();
}

size_t FemViewWindow::nodeIdx(vfem::Node *node)
{
    return node->getFemNode()->getNumber();
}

size_t FemViewWindow::beamIdx(vfem::Beam *beam)
{
    return beam->getBeam()->getNumber();
}

void FemViewWindow::startService()
{
    if (m_service == nullptr)
        m_service = ofservice::Service::create(this);
}

void FemViewWindow::stopService()
{
    if (m_service != nullptr)
        m_service = nullptr;
}

bool FemViewWindow::isServiceRunning() const
{
    return m_service != nullptr;
}

void FemViewWindow::showMessage(std::string message)
{
    m_messagePopup->setMessage(message);
    m_messagePopup->show();
}

#ifdef USE_LEAP
LeapInteraction *CIvfFemWidget::getLeapInteraction()
{
    return m_leapinteraction;
}

void CIvfFemWidget::updateLeapFrame(Frame leapFrame)
{
    m_leapinteraction->updateLeapFrame(leapFrame);
}
#endif

ExtrArrowPtr FemViewWindow::getTactileForce()
{
    return m_tactileForce;
}

void FemViewWindow::setTactileForce(ExtrArrowPtr force)
{
    m_tactileForce = force;
}

vfem::NodePtr FemViewWindow::getInteractionNode()
{
    return m_interactionNode;
}

vfem::BeamModel *FemViewWindow::getVisualBeamModel()
{
    return m_beamModel.get();
}

RepresentationMode FemViewWindow::getRepresentation()
{
    return m_representation;
}

void FemViewWindow::setSphereCursor(bool flag)
{
    vfem::Preferences::instance().setUseSphereNodes(flag);
    m_useSphereCursor = vfem::Preferences::instance().useSphereNodes();

    if (m_useSphereCursor)
    {
        m_nodeCursor = m_sphereCursor;
        m_beamModel->setNodeRepr(ivf::Node::NT_SPHERE);
    }
    else
    {
        m_nodeCursor = m_cubeCursor;
        m_beamModel->setNodeRepr(ivf::Node::NT_CUBE);
    }

    this->getScene()->setCursorShape(m_nodeCursor);
}

bool FemViewWindow::getSphereCursor()
{
    return m_useSphereCursor;
}

void FemViewWindow::setUseBlending(bool flag)
{
    m_useBlending = flag;

    if (m_useBlending)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    else
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

    if (m_useBlending)
    {
        m_beamModel->setUseBlending(true);
        this->getScene()->getCurrentPlane()->setState(ivf::Shape::OS_OFF);
        this->getScene()->setRenderFlatShadow(false);
    }
    else
    {
        m_beamModel->setUseBlending(false);
        this->getScene()->getCurrentPlane()->setState(ivf::Shape::OS_ON);
        this->getScene()->setRenderFlatShadow(true);
    }
}

bool FemViewWindow::getUseBlending()
{
    return m_useBlending;
}

float FemViewWindow::uiScale()
{
    return m_uiScale;
}

void FemViewWindow::setUseImGuiFileDialogs(bool flag)
{
    m_useImGuiFileDialogs = flag;
}

bool FemViewWindow::getUseImGuiFileDialogs()
{
    return m_useImGuiFileDialogs;
}

void FemViewWindow::setUiScale(float scale)
{
    m_uiScale = scale;

    this->refreshUiStyle();
}

void FemViewWindow::setSaveScreenShot(bool flag)
{
    m_saveScreenShot = flag;
}

bool FemViewWindow::getSaveScreenShot()
{
    return m_saveScreenShot;
}

void FemViewWindow::setAiApiKey(const std::string &apiKey)
{
    m_aiApiKey = apiKey;
}

std::string FemViewWindow::getAiApiKey()
{
    return m_aiApiKey;
}

void FemViewWindow::setInteractionNode(vfem::Node *interactionNode)
{
    m_interactionNode = interactionNode;
}

void FemViewWindow::lockScaleFactor()
{
    m_lockScaleFactor = true;
}

bool FemViewWindow::isScaleFactorLocked()
{
    return m_lockScaleFactor;
}

double FemViewWindow::autoScaleFactor()
{
    if (m_currentSolver != nullptr)
        return this->getWorkspace() * 0.005 / m_currentSolver->getMaxNodeValue();
    else
        return 1.0;
}

void FemViewWindow::unlockScaleFactor()
{
    m_lockScaleFactor = false;
}

void FemViewWindow::refreshToolbars()
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

void FemViewWindow::removeNodeLoadsFromSelected()
{
    // Remove materials from selected shapes

    ofem::BeamNodeLoad *nodeLoad = this->getCurrentNodeLoad();

    if (nodeLoad != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Node"))
            {
                vfem::Node *visNode = static_cast<vfem::Node *>(shape);
                ofem::Node *node = visNode->getFemNode();
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

void FemViewWindow::removeNodesFromNodeLoad()
{
    ofem::BeamNodeLoad *nodeLoad = this->getCurrentNodeLoad();

    if (nodeLoad != nullptr)
        nodeLoad->clearNodes();

    m_needRecalc = true;
    this->set_changed();
    this->redraw();
}

void FemViewWindow::removeNodeBCsFromSelected()
{
    // Remove materials from selected shapes

    ofem::BeamNodeBC *nodeBC = this->getCurrentNodeBC();

    if (nodeBC != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Node"))
            {
                vfem::Node *visNode = static_cast<vfem::Node *>(shape);
                ofem::Node *node = visNode->getFemNode();
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

void FemViewWindow::removeBCsFromBC()
{
    ofem::BeamNodeBC *nodeBC = this->getCurrentNodeBC();

    if (!nodeBC->isReadOnly())
    {
        if (nodeBC != nullptr)
            nodeBC->clearNodes();

        m_needRecalc = true;
        this->set_changed();
        this->redraw();
    }
}

void FemViewWindow::removeBeamLoadsFromSelected()
{
    // Remove materials from selected shapes

    ofem::BeamLoad *beamLoad = this->getCurrentBeamLoad();

    if (beamLoad != nullptr)
    {
        auto selected = this->getSelectedShapes();
        for (int i = 0; i < selected->getSize(); i++)
        {
            auto shape = selected->getChild(i);
            if (shape->isClass("vfem::Beam"))
            {
                vfem::Beam *visBeam = static_cast<vfem::Beam *>(shape);
                ofem::Beam *beam = visBeam->getBeam();
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

void FemViewWindow::hideAllDialogs()
{
    log("Hiding all dialogs.");

    m_nodeLoadsWindow->hide();
    m_nodeBCsWindow->hide();
    m_elementLoadsWindow->hide();
    m_materialsWindow->hide();
    m_loadMixerWindow->hide();
    m_scaleWindow->hide();
    m_settingsWindow->hide();
    m_aboutWindow->hide();
}

// Widget events

void FemViewWindow::onInit()
{
    // Configure gle VBO mode
    // 
    gleInitVBOCache(10000);
    gleInitTessCache();
    gleSetVBOMode(1);
    // 
    // Setup web service

    // Create log window early as it will be called by the logger.

    m_logWindow = LogWindow::create("Log window");
    m_logWindow->setSize(600, 600);
    m_logWindow->setVisible(false);

    m_consoleWindow = ConsoleWindow::create("Hints");
    m_consoleWindow->setVisible(true);
    // m_consoleWindow->centerBottom();

    using std::placeholders::_1;
    LoggerMessageFunc f = std::bind(&FemViewWindow::onMessage, this, _1);
    Logger::instance()->assignOnMessageShort(f);

    // Display version information.

    log(OBJFRAME_VERSION_STRING);
    log(OBJFRAME_RELEASE);
    log(OBJFRAME_COPYRIGHT_STRING);
    log(OBJFRAME_AUTHOR1);
    log(OBJFRAME_AUTHOR2);
    log(OBJFRAME_EXTRA1);
    log("---------------------------------------------");
    log("Program path : " + m_progPath.string());
    log("Font path    : " + m_fontPath.string());
    log("Image path   : " + m_imagePath.string());
    log("Plugin path  : " + m_pluginPath.string());
    log("Map path     : " + m_mapPath.string());
    log("Python path  : " + m_pythonPath.string());
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
    this->getScene()->getCurrentPlane()->getGrid()->setMajorColor(0.2f, 0.2f, 0.2f, 1.0f);
    this->getScene()->getCurrentPlane()->getGrid()->setMinorColor(0.3f, 0.3f, 0.3f, 1.0f);
    this->getScene()->getCurrentPlane()->getGrid()->setOutlineColor(0.2f, 0.2f, 0.2f, 1.0f);
    this->getScene()->getCurrentPlane()->getGrid()->setCornerColor(0.2f, 0.2f, 0.2f, 1.0f);
    this->getScene()->setRenderFlatShadow(true);
    this->getScene()->setShadowColor(0.2f, 0.2f, 0.2f);
    this->getScene()->setShadowPrePost(false, false);

    // Label rendering setup

    log("Loading bitmap fonts...");

    namespace fs = std::filesystem;

    if (std::filesystem::is_directory(m_fontPath))
    {
        m_labelFont = ivf::BitmapFont::create((m_fontPath / fs::path("white_font.fnt")).string());
        m_axisFont = ivf::BitmapFont::create((m_fontPath / fs::path("black_font.fnt")).string());
        m_greenFont = ivf::BitmapFont::create((m_fontPath / fs::path("green_font.fnt")).string());
        m_redFont = ivf::BitmapFont::create((m_fontPath / fs::path("red_font.fnt")).string());
    }
    else
    {
        log("No font directory found.");
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

    if (!std::filesystem::is_directory(m_mapPath))
    {
        this->quit();
    }

    log("Initializing beam model.");
    m_beamModel = vfem::BeamModel::create();
    m_beamModel->initialize();
    m_beamModel->setPath(m_mapPath.string());
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

    m_beamModel->generateModel();

    // Initialise model clipboard

    m_modelClipBoard = ofem::ModelClipBoard::create();

    using namespace std::placeholders;
    ofem::ModelClipboardCreateNodeFunc onCreateNode =
        std::bind(&FemViewWindow::onClipboardCreateNode, this, _1, _2, _3);
    m_modelClipBoard->assignOnCreateNode(onCreateNode);

    ofem::ModelClipboardCreateElementFunc onCreateElement =
        std::bind(&FemViewWindow::onClipboardCreateElement, this, _1, _2);
    m_modelClipBoard->assignOnCreateElement(onCreateElement);

    // Initialize color table

    log("Initializing color table.");
    auto colorTable = m_beamModel->getColorTable();

    unsigned int c, red, green, blue;

    for (int i = 0; i < 256; i++)
    {
        if (i & 0xffffff00)
            c = (unsigned)i;
        else
            c = fl_cmap[i];

        red = uchar(c >> 24);
        green = uchar(c >> 16);
        blue = uchar(c >> 8);

        colorTable->setColor(i, (float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f);
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

    log("Creating cursors.");

    m_sphereCursor = Sphere::create();
    m_sphereCursor->setMaterial(m_nodeMaterial);
    m_sphereCursor->setRadius(m_beamModel->getNodeSize());

    m_cubeCursor = Cube::create();
    m_cubeCursor->setMaterial(m_nodeMaterial);
    m_cubeCursor->setSize(m_beamModel->getNodeSize() * 1.5);

    if (vfem::Preferences::instance().useSphereNodes())
        m_nodeCursor = m_sphereCursor;
    else
        m_nodeCursor = m_cubeCursor;

    this->getScene()->setCursorShape(m_nodeCursor);

    // Create ImGui interface

    log("Initialising immediate mode UI.");

    m_windowList = WindowList::create();
    m_windowList->setFemView(this);

    m_windowList->add(m_logWindow);
    m_windowList->add(m_consoleWindow);

    m_showStyleEditor = false;
    m_showMetricsWindow = false;
    m_showNewFileDlg = false;
    m_openDialog = false;

    m_coordWindow = CoordWindow::create("Coord window");

    m_windowList->add(m_coordWindow);

    m_newModelPopup = NewModelPopup::create("Workspace", true);
    m_messagePopup = MessagePopup::create("Message", true);

    m_nodeBCsWindow = NodeBCsWindow::create("Node BCs");
    m_nodeBCsWindow->setFemView(this);
    m_nodeBCsWindow->setVisible(false);

    m_windowList->add(m_nodeBCsWindow);

    m_bcPropPopup = BCPropPopup::create("Node BC", true);

    m_nodeLoadsWindow = NodeLoadsWindow::create("Node Loads");
    m_nodeLoadsWindow->setFemView(this);
    m_nodeLoadsWindow->setVisible(false);

    m_windowList->add(m_nodeLoadsWindow);

    m_settingsWindow = SettingsWindow::create("Settings");
    m_settingsWindow->setFemView(this);
    m_settingsWindow->setVisible(false);

    m_windowList->add(m_settingsWindow);

    m_elementLoadsWindow = ElementLoadsWindow::create("Element Loads");
    m_elementLoadsWindow->setFemView(this);
    m_elementLoadsWindow->setVisible(false);

    m_windowList->add(m_elementLoadsWindow);

    m_materialsWindow = MaterialsWindow::create("Materials");
    m_materialsWindow->setFemView(this);
    m_materialsWindow->setVisible(false);

    m_windowList->add(m_materialsWindow);

    m_pluginWindow = PluginPropWindow::create("Plugin properties");
    m_pluginWindow->setView(this);
    m_pluginWindow->setVisible(false);

    m_windowList->add(m_pluginWindow);

    m_scaleWindow = ScaleWindow::create("Scaling");
    m_scaleWindow->setView(this);
    m_scaleWindow->setVisible(false);

    m_windowList->add(m_scaleWindow);

    m_aboutWindow = AboutWindow::create("About");
    m_aboutWindow->setVersion(OBJFRAME_VERSION_STRING);
    m_aboutWindow->setRelease(OBJFRAME_RELEASE);
    m_aboutWindow->setCopyright(OBJFRAME_COPYRIGHT_STRING);
    m_aboutWindow->setAuthor1(OBJFRAME_AUTHOR1);
    m_aboutWindow->setAuthor2(OBJFRAME_AUTHOR2);
    m_aboutWindow->setExtra1(OBJFRAME_EXTRA1);
    m_aboutWindow->setVisible(false);

    m_windowList->add(m_aboutWindow);

    m_propWindow = PropWindow::create("Properties");
    m_propWindow->setView(this);
    m_propWindow->setVisible(false);

    m_windowList->add(m_propWindow);

    m_loadMixerWindow = LoadMixerWindow::create("Load mixer");
    m_loadMixerWindow->setView(this);
    m_loadMixerWindow->setVisible(false);

    m_windowList->add(m_loadMixerWindow);

    m_scriptWindow = ScriptWindow::create("Script");
    m_scriptWindow->setView(this);
    m_scriptWindow->setVisible(false);

    m_windowList->add(m_scriptWindow);

    m_promptWindow = PromptWindow::create("Prompt");
    m_promptWindow->setView(this);
    m_promptWindow->setVisible(false);

    m_windowList->add(m_promptWindow);

    m_eigenmodeWindow = EigenmodeWindow::create("Eigenmode Analysis");
    m_eigenmodeWindow->setFemView(this);
    m_eigenmodeWindow->setVisible(false);

    m_windowList->add(m_eigenmodeWindow);

    m_startPopup = StartPopup::create("Start window", true);
    m_startPopup->setVersionString(OBJFRAME_VERSION_STRING);
    m_startPopup->setRelease(OBJFRAME_RELEASE);
    m_startPopup->setView(this);
    m_startPopup->setVisible(true);

    m_mainToolbarWindow = ToolbarWindow::create("Edit");

    m_mainToolbarWindow->setVisible(true);

    m_mainToolbarWindow->addButton("Select", OfToolbarButtonType::RadioButton,
                                   (m_imagePath / fs::path("tlselect.png")).string(), 1);
    m_mainToolbarWindow->addButton("Box select", OfToolbarButtonType::RadioButton,
                                   (m_imagePath / fs::path("tlselectbox.png")).string(), 1);
    m_mainToolbarWindow->addButton("Move", OfToolbarButtonType::RadioButton,
                                   (m_imagePath / fs::path("tlmove.png")).string(), 1);
    m_mainToolbarWindow->addSpacer();
    m_mainToolbarWindow->addButton("Inspect", OfToolbarButtonType::Button,
                                   (m_imagePath / fs::path("tlinspect.png")).string());
    m_mainToolbarWindow->addButton("Delete", OfToolbarButtonType::Button,
                                   (m_imagePath / fs::path("tldelete.png")).string());
    m_mainToolbarWindow->addSpacer();
    m_mainToolbarWindow->addButton("Feedback", OfToolbarButtonType::RadioButton,
                                   (m_imagePath / fs::path("tlfeedback.png")).string(), 1);
    m_mainToolbarWindow->addButton("Run", OfToolbarButtonType::Button, (m_imagePath / fs::path("run.png")).string());

    using std::placeholders::_1;
    m_mainToolbarWindow->assignOnButtonClicked(std::bind(&FemViewWindow::onButtonClicked, this, std::placeholders::_1));

    m_mainToolbarWindow->assignOnButtonHover(std::bind(&FemViewWindow::onButtonHover, this, std::placeholders::_1));

    m_windowList->add(m_mainToolbarWindow);

    m_editToolbarWindow = ToolbarWindow::create("Model");
    m_editToolbarWindow->setSize(400, 0);

    m_editToolbarWindow->setVisible(true);

    m_editToolbarWindow->addButton("Create node", OfToolbarButtonType::RadioButton,
                                   (m_imagePath / fs::path("tlnode.png")).string(), 1);

    m_editToolbarWindow->addButton("Create beam", OfToolbarButtonType::RadioButton,
                                   (m_imagePath / fs::path("tlsolidline.png")).string(), 1);

    m_editToolbarWindow->addSpacer();

    m_editToolbarWindow->addButton("Node loads", OfToolbarButtonType::Button,
                                   (m_imagePath / fs::path("tlnodeloads.png")).string(), 0);

    m_editToolbarWindow->addButton("Beam loads", OfToolbarButtonType::Button,
                                   (m_imagePath / fs::path("tldload.png")).string(), 0);

    m_editToolbarWindow->addButton("Node BC", OfToolbarButtonType::Button,
                                   (m_imagePath / fs::path("tlbc.png")).string(), 0);

    m_editToolbarWindow->addButton("Materials", OfToolbarButtonType::Button,
                                   (m_imagePath / fs::path("tlmaterials.png")).string(), 0);

    m_editToolbarWindow->assignOnButtonClicked(
        ButtonClickedFunc(std::bind(&FemViewWindow::onButtonClicked, this, std::placeholders::_1)));

    m_editToolbarWindow->assignOnButtonHover(
        ButtonClickedFunc(std::bind(&FemViewWindow::onButtonHover, this, std::placeholders::_1)));

    m_windowList->add(m_editToolbarWindow);

    m_mainToolbarWindow->addToolbarGroup(m_editToolbarWindow);
    m_editToolbarWindow->addToolbarGroup(m_mainToolbarWindow);

    this->setupExamples();

    for (auto &filename : m_examples)
        m_startPopup->addExample(filename, filename + ".png");

    m_startPopup->assignStartButtonClickedFunc(
        std::bind(&FemViewWindow::onStartButtonClicked, this, std::placeholders::_1));
    m_startPopup->assignExampleClickedFunc(std::bind(&FemViewWindow::onExampleClicked, this, std::placeholders::_1));

    // Tetgen

    log("Initialising tetmesh...");

    m_tetMesher = ofsolver::TetgenBeamMesher::create();
    m_tetMesher->setProgPath(m_progPathStr);

    // Set initial edit mode

    log("Setting initial edit mode...");
    this->setEditMode(WidgetMode::Select);

    this->setupPlugins();
    this->setupAi();

    if (m_argc > 1)
    {
        // We have command line arguments
        log("Loading from command line:" + to_string(m_argv[1]));
        m_startPopup->setVisible(false);
        this->open(to_string(m_argv[1]));
    }
    else
    {}
}

void FemViewWindow::doMouse(int x, int y)
{
    if (!m_overlaySelected)
        IvfViewWindow::doMouse(x, y);
}

void FemViewWindow::onCreateNode(double x, double y, double z, ivf::Node *&newNode)
{
    // Create a node

    this->snapShot();

    // First we create a FemNode

    ofem::Node *femNode = new ofem::Node();

    // Add it to the Fem model

    m_beamModel->getNodeSet()->addNode(femNode);
    femNode->setNumber(static_cast<long>(m_beamModel->getNodeSet()->getSize()));

    // Create Ivf representation

    vfem::Node *ivfNode = new vfem::Node();
    ivfNode->setBeamModel(m_beamModel.get());
    ivfNode->setFemNode(femNode);
    ivfNode->setPosition(x, y, z);
    ivfNode->setMaterial(m_nodeMaterial);
    ivfNode->nodeLabel()->setSize(float(m_beamModel->getNodeSize() * 1.5));
    ivfNode->setDirectRefresh(true);

    femNode->setUser(static_cast<void *>(ivfNode));

    // We need a recalc

    m_needRecalc = true;

    newNode = ivfNode;
}

void FemViewWindow::onCreateLine(ivf::Node *node1, ivf::Node *node2, Shape *&newLine)
{
    if (node1 == node2)
        return;

    this->snapShot();

    // Create visual representation

    vfem::Beam *visBeam = new vfem::Beam();
    visBeam->setBeamModel(m_beamModel.get());

    // Create model representation

    ofem::Beam *femBeam = new ofem::Beam();

    // Extract FemNode:s from the IvfNodes

    vfem::Node *visNode1 = static_cast<vfem::Node *>(node1);
    vfem::Node *visNode2 = static_cast<vfem::Node *>(node2);

    // Add FemNodes to beam element

    femBeam->addNode(visNode1->getFemNode());
    femBeam->addNode(visNode2->getFemNode());

    // Set the material

    femBeam->setMaterial((ofem::BeamMaterial *)m_beamModel->getMaterialSet()->currentMaterial());
    femBeam->setUser(static_cast<void *>(visBeam));

    // Add beam element to beam model

    m_beamModel->getElementSet()->addElement(femBeam);

    // Initialize the representation

    visBeam->setBeam(femBeam);
    visBeam->setNodes(static_cast<vfem::Node *>(node1), static_cast<vfem::Node *>(node2));
    visBeam->refresh();

    // We need a recalc

    m_needRecalc = true;

    // Return the finished object

    newLine = static_cast<Shape *>(visBeam);
}

void FemViewWindow::onSelect(Composite *selectedShapes)
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

        m_propWindow->setNode(nullptr);
        m_propWindow->setBeam(nullptr);
        m_propWindow->setSelectedShapes(nullptr);

        // Update dialogs with new selection

        if (selectedShapes->getSize() > 0)
        {
            if (selectedShapes->getSize() == 1)
            {
                auto firstShape = selectedShapes->getChild(0);
                m_selectedShape = firstShape;
                if (firstShape->isClass("vfem::Node"))
                {
                    m_propWindow->setNode(static_cast<vfem::Node *>(firstShape));
                    m_singleNodeSelection = true;
                }
                if (firstShape->isClass("vfem::Beam"))
                {
                    m_propWindow->setSelectedShapes(nullptr);
                    m_propWindow->setBeam(static_cast<vfem::Beam *>(firstShape));

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

                m_propWindow->setSelectedShapes(selectedShapes);
                m_propWindow->setNode(nullptr);
                m_propWindow->setBeam(nullptr);

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
                vfem::Node *visNode = static_cast<vfem::Node *>(shape);
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

bool FemViewWindow::onInsideVolume(ivf::Shape *shape)
{
    if (shape->isClass("vfem::Beam"))
    {
        auto beam = static_cast<vfem::Beam *>(shape);
        auto n0 = beam->getNode(0);
        auto n1 = beam->getNode(1);

        return (this->isInsideVolume(n0)) && (this->isInsideVolume(n1));
    }
    else
        return false;
}

std::string FemViewWindow::float2str(double value)
{
    std::stringstream coordStream;
    coordStream << std::fixed << std::setw(10) << std::setprecision(2) << value;
    return coordStream.str();
}

void FemViewWindow::log(std::string message)
{
    Logger::instance()->log(LogLevel::Info, message);
}

void FemViewWindow::onMessage(std::string message)
{
    m_logWindow->log(message + "\n");
}

void FemViewWindow::console(std::string message)
{
    m_consoleWindow->log(message + "\n");
}

void FemViewWindow::onCoordinate(double x, double y, double z)
{
    // Update coordinate display

    m_coordText = "";

    m_xCoord = float2str(x);
    m_yCoord = float2str(y);
    m_zCoord = float2str(z);

    m_coordWindow->setCoord(x, y, z);
}

void FemViewWindow::onDeleteShape(Shape *shape, bool &doit)
{
    // Handle shape deletion

    if ((m_deleteFilter == DeleteMode::All) || (m_deleteFilter == DeleteMode::Nodes))
    {
        if (shape->isClass("vfem::Node"))
        {
            vfem::Node *visNode = static_cast<vfem::Node *>(shape);

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
            vfem::Beam *visBeam = static_cast<vfem::Beam *>(shape);
            ofem::Beam *femBeam = visBeam->getBeam();
            ofem::BeamSet *beamSet = m_beamModel->getElementSet();

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

void FemViewWindow::onMove(Composite *selectedShapes, double &dx, double &dy, double &dz, bool &doit)
{
    doit = true;
    m_needRecalc = true;
}

void FemViewWindow::onMoveCompleted()
{}

void FemViewWindow::onUnderlay()
{
    if (!m_useBlending)
    {
        glBegin(GL_QUADS);
        glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
        glVertex2i(0, 0);
        glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
        glVertex2f(0.0f, float(height()));
        glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
        glVertex2f(float(width()), float(height()));
        glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
        glVertex2f(float(width()), 0.0f);
        glEnd();
    }
}

void FemViewWindow::onOverlay()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_DEPTH_TEST);

    // Render areas

#ifdef USE_OVERLAY_BUTTONS
    m_objectArea->setCoord(0, 0, height() - 80);
    m_objectArea->setCoord(1, width() / 2, height() - 80);
    m_objectArea->setCoord(2, width() / 2, height());
    m_objectArea->setCoord(3, 0, height());

    m_objectArea->render();
    m_editArea->render();

    // Update button positions

    m_objectButtons->setPosition(0.0, height() - 70, 0.0);
#endif

    m_logoButton->setPosition(width() - 80, height() - 80, 0.0);

    // Render overlay "scene"

    m_overlayScene->render();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0);
    glEnable(GL_DEPTH_TEST);
}

void FemViewWindow::onInitContext()
{
    IvfViewWindow::onInitContext();

    glEnable(GL_DEPTH_TEST);
    if (m_useBlending)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    else
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

void FemViewWindow::onPassiveMotion(int x, int y)
{
    unsigned int i;
    bool inside = false;
    bool needInvalidate = false;

    // Only do select inside defined areas

    this->setEditEnabled(true);

#ifdef USE_OVERLAY_BUTTONS
    m_overlaySelected = false;
    for (i = 0; i < m_areas.size(); i++)
    {
        if (m_areas[i]->inside(x, y))
        {
            inside = true;

            this->setEditEnabled(false);
            needInvalidate = false;

            if (m_selectedButton != nullptr)
            {
                m_selectedButton->setScale(1.0, 1.0, 1.0);
                needInvalidate = true;
                m_overlaySelected = false;
            }

            auto editButton = m_editButtons->isUnder(x, y);
            auto objectButton = this->m_objectButtons->isUnder(x, y);

            m_selectedButton = nullptr;

            if (editButton != nullptr)
                m_selectedButton = editButton;

            if (objectButton != nullptr)
                m_selectedButton = objectButton;

            if (m_selectedButton != nullptr)
            {
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
#endif

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
        draw();
    }
}

void FemViewWindow::onMouse(int x, int y)
{}

void FemViewWindow::onMouseDown(int x, int y)
{
    m_mouseDownPos[0] = x;
    m_mouseDownPos[1] = y;

#ifdef USE_OVERLAY_BUTTONS
    if ((m_overlaySelected) && (m_selectedButton != nullptr))
    {
        m_interactionNode = nullptr;
        m_selectedButton->setButtonState(GenericButton::BS_PRESSED);
        this->redraw();
    }
#endif

    if ((getEditMode() == WidgetMode::User) && (!m_overlaySelected))
    {
        log("onMouseDown USER_MODE.");
        if (getCurrentMouseButton() == ButtonState::bsButton1)
        {
            if (m_saneModel)
                if (m_currentSolver != nullptr)
                    setRepresentation(RepresentationMode::Results);
        }
    }
}

void FemViewWindow::onMouseUp(int x, int y)
{
#ifdef USE_OVERLAY_BUTTONS
    if ((m_overlaySelected) && (m_selectedButton != nullptr))
    {
        this->onButton(m_selectedButton->getId(), (PlaneButton *)m_selectedButton);
        this->redraw();
    }
#endif
    m_startAlfa = m_alfa;
    m_startBeta = m_beta;

    if (getEditMode() == WidgetMode::User)
    {
        if (getCurrentMouseButton() == ButtonState::bsButton1)
        {
            if (m_saneModel)
                if (m_currentSolver != nullptr)
                {
                    m_currentSolver->update();
                    setRepresentation(RepresentationMode::Results);
                }
        }
    }
}

void FemViewWindow::onMotion(int x, int y)
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

        this->redraw(); // Refresh
        this->draw();
    }
}

void FemViewWindow::onDeSelect()
{
    log("onDeSelect");

    m_propWindow->setNode(nullptr);
    m_propWindow->setBeam(nullptr);
    m_propWindow->setSelectedShapes(nullptr);

    if (m_customMode == CustomMode::Feedback)
    {
        m_interactionNode = nullptr;
        m_tactileForce->setState(Shape::OS_OFF);
        this->redraw();
    }
}

void FemViewWindow::onHighlightShape(Shape *shape)
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

void FemViewWindow::onSelectFilter(Shape *shape, bool &select)
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
            vfem::Node *visNode = static_cast<vfem::Node *>(shape);

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

void FemViewWindow::onSelectPosition(double x, double y, double z)
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
        m_modelClipBoard->setOffset(x, y, z);
        m_modelClipBoard->paste(m_beamModel.get());
    }
}

void FemViewWindow::onMoveStart()
{
    this->snapShot();
}

void FemViewWindow::updateButtonState()
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

void FemViewWindow::onButton(int objectName, PlaneButton *button)
{
#ifdef USE_OVERLAY_BUTTONS
    m_editButtons->clearChecked();
    m_objectButtons->clearChecked();

    switch (objectName)
    {
    case ToolbarButton::Select:
        this->setEditMode(WidgetMode::Select);
        break;
    case ToolbarButton::SelectBox:
        this->setEditMode(WidgetMode::BoxSelection);
        break;
    case ToolbarButton::Move:
        this->setEditMode(WidgetMode::Move);
        break;
    case ToolbarButton::CreateNode:
        this->setEditMode(WidgetMode::CreateNode);
        break;
    case ToolbarButton::CreateBeam:
        this->setEditMode(WidgetMode::CreateLine);
        break;
    case ToolbarButton::Feedback:
        this->setCustomMode(CustomMode::Feedback);
        break;
    case ToolbarButton::Run:
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
        m_nodeLoadsWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet *)m_beamModel->getNodeLoadSet());
        m_nodeLoadsWindow->setVisible(true);
        m_nodeLoadsWindow->setPosition(100, 20);
        this->setNeedRecalc(true);
        break;
    case ToolbarButton::BeamLoad:
        m_elementLoadsWindow->setFemLoadSet((ofem::BeamLoadSet *)m_beamModel->getElementLoadSet());
        m_elementLoadsWindow->setVisible(true);
        m_elementLoadsWindow->setPosition(100, 20);
        this->setNeedRecalc(true);
        break;
    case ToolbarButton::Materials:
        m_materialsWindow->setFemMaterialSet((ofem::BeamMaterialSet *)m_beamModel->getMaterialSet());
        m_materialsWindow->setVisible(true);
        m_materialsWindow->setPosition(100, 20);
        this->setNeedRecalc(true);
        break;
    case ToolbarButton::NodeBC:
        m_nodeBCsWindow->setFemNodeBCSet((ofem::BeamNodeBCSet *)m_beamModel->getNodeBCSet());
        m_nodeBCsWindow->setVisible(true);
        m_nodeBCsWindow->setPosition(100, 20);
        this->setNeedRecalc(true);
        break;
    }
#endif
}

void FemViewWindow::onOverButton(int objectName, PlaneButton *button)
{
#ifdef USE_OVERLAY_BUTTONS
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
#endif
}

void FemViewWindow::onShortcut(ModifierKey modifier, int key)
{
    if (m_windowList->isAnyFocused())
    {
        return;
    }
    if ((modifier == ModifierKey::mkCtrl) && (key == 'O'))
    {
        m_openDialog = true;
    }

    if ((modifier == ModifierKey::mkCtrl) && (key == 'N'))
        this->newModel();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'S'))
    {
        m_saveDialog = true;
    }

    if ((modifier == ModifierKey::mkCtrl) && (key == 'A'))
    {
        this->setSelectFilter(SelectMode::All);
        this->selectAll();
    }

    if ((modifier == ModifierKey::mkCtrl) && (key == 'C'))
        this->copy();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'V'))
        this->paste();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'Z'))
        this->restoreLastSnapShot();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'Y'))
        this->revertLastSnapShot();

    if ((modifier == ModifierKey::mkAlt) && (key == 'D'))
        m_showMetricsWindow = !m_showMetricsWindow;

    if ((modifier == ModifierKey::mkCtrl) && (key == 'R'))
        this->executeCalc();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'D'))
        this->subdivideSelectedBeam();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'D'))
        this->subdivideSelectedBeam();

    if ((modifier == ModifierKey::mkCtrl) && (key == 'M'))
        this->meshSelectedNodes();

    if ((modifier == ModifierKey::mkAlt) && (key == 'S'))
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_editButtons->check(0);
        this->setEditMode(WidgetMode::Select);
        this->redraw();
    }

    if ((modifier == ModifierKey::mkAlt) && (key == 'M'))
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_editButtons->check(1);
        this->setEditMode(WidgetMode::Move);
        this->redraw();
    }

    if ((modifier == ModifierKey::mkAlt) && (key == 'N'))
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_objectButtons->check(0);
        this->setEditMode(WidgetMode::CreateNode);
        this->redraw();
    }

    if ((modifier == ModifierKey::mkAlt) && (key == 'L'))
    {
        m_editButtons->clearChecked();
        m_objectButtons->clearChecked();
        m_objectButtons->check(1);
        this->setEditMode(WidgetMode::CreateLine);
        this->redraw();
    }

    if ((modifier == ModifierKey::mkAlt) && (key == 'X'))
    {
        if (this->getUseBlending())
            this->setUseBlending(false);
        else
            this->setUseBlending(true);
    }
}

void FemViewWindow::onButtonClicked(ofui::OfToolbarButton &button)
{
    log("onButtonClicked: " + button.name());

    hideAllDialogs();

    if (button.name() == "Select")
    {
        this->setEditMode(WidgetMode::Select);
    }

    if (button.name() == "Box select")
    {
        this->setEditMode(WidgetMode::BoxSelection);
    }

    if (button.name() == "Move")
    {
        this->setEditMode(WidgetMode::Move);
    }

    if (button.name() == "Inspect")
    {
        this->showProperties();
    }

    if (button.name() == "Delete")
    {
        this->setEditEnabled(true);
        this->deleteSelected();
        this->setEditEnabled(false);
        m_editButtons->recheck();
    }

    if (button.name() == "Feedback")
    {
        this->setCustomMode(CustomMode::Feedback);
    }

    if (button.name() == "Run")
    {
        this->executeCalc();
    }

    if (button.name() == "View zoom")
    {
        this->setEditMode(WidgetMode::ViewZoom);
    }

    if (button.name() == "View pan")
    {
        this->setEditMode(WidgetMode::ViewPan);
    }

    if (button.name() == "View reset")
    {
        this->resetView();
    }

    if (button.name() == "Create node")
    {
        this->setEditMode(WidgetMode::CreateNode);
    }

    if (button.name() == "Create beam")
    {
        this->setEditMode(WidgetMode::CreateLine);
    }

    if (button.name() == "Node loads")
    {
        m_nodeLoadsWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet *)m_beamModel->getNodeLoadSet());
        m_nodeLoadsWindow->setVisible(true);
        m_nodeLoadsWindow->setPosition(100, 20);
        this->setNeedRecalc(true);
    }

    if (button.name() == "Beam loads")
    {
        m_elementLoadsWindow->setFemLoadSet((ofem::BeamLoadSet *)m_beamModel->getElementLoadSet());
        m_elementLoadsWindow->setVisible(true);
        m_elementLoadsWindow->setPosition(100, 20);
        this->setNeedRecalc(true);
    }

    if (button.name() == "Materials")
    {
        m_materialsWindow->setFemMaterialSet((ofem::BeamMaterialSet *)m_beamModel->getMaterialSet());
        m_materialsWindow->setVisible(true);
        m_materialsWindow->setPosition(100, 20);
        this->setNeedRecalc(true);
    }

    if (button.name() == "Node BC")
    {
        m_nodeBCsWindow->setFemNodeBCSet((ofem::BeamNodeBCSet *)m_beamModel->getNodeBCSet());
        m_nodeBCsWindow->setVisible(true);
        m_nodeBCsWindow->setPosition(100, 20);
        this->setNeedRecalc(true);
    }
}

void FemViewWindow::onButtonHover(ofui::OfToolbarButton &button)
{
    m_consoleWindow->clear();
    if (button.name() == "Select")
        console("Select nodes and beams.");
    if (button.name() == "Move")
        console("Move nodes.");
    if (button.name() == "Create node")
        console("Create nodes");
    if (button.name() == "Create beam")
        console("Create beams.");
    if (button.name() == "Feedback")
        console("Interact with model using an movable force.");
    if (button.name() == "Run")
        console("Execute calculation");
    if (button.name() == "View zoom")
        console("Zoom view");
    if (button.name() == "View pan")
        console("Pan view");
    if (button.name() == "View reset")
        console("Reset view");
    if (button.name() == "Delete")
        console("Delete nodes and beams.");
    if (button.name() == "Inspect")
        console("Show object properties.");
    if (button.name() == "Node loads")
        console("Create node loads.");
    if (button.name() == "Beam loads")
        console("Create element loads.");
    if (button.name() == "Materials")
        console("Create materials.");
    if (button.name() == "Node BC")
        console("Create node boundary conditions.");
}

void FemViewWindow::onStartButtonClicked(ofui::OfStartButton &button)
{
    if (button == OfStartButton::NewModel)
    {
        this->newModel();
    }
    if (button == OfStartButton::OpenModel)
    {
        m_openDialog = true;
    }
    if (button == OfStartButton::OpenAIPrompt)
    {
        m_promptWindow->show();
    }
    if (button == OfStartButton::OpenPythonModel)
    {
        m_openFromCalfemDialog = true;
    }
    if (button == OfStartButton::StartAPI)
    {
        if (this->isServiceRunning())
            this->stopService();
        else
            this->startService();
    }
    if (button == OfStartButton::Documentation)
    {
#ifdef WIN32
        ShellExecuteW(0, 0, L"https://jonaslindemann.github.io/objectiveframe/", 0, 0, SW_SHOW);
#endif
    }
}

void FemViewWindow::onExampleClicked(std::string &filename)
{
    this->open(filename);
    this->setFileName("noname.df3");
}

void FemViewWindow::onHighlightFilter(Shape *shape, bool &highlight)
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

void FemViewWindow::onKeyboard(int key)
{
    // log("onKeyboard: " + std::to_string(key));

    if ((!m_windowList->isAnyFocused()) && (!m_scriptWindow->isEditorActive()))
    {
        if (key == 256)
        {
            m_editButtons->clearChecked();
            m_objectButtons->clearChecked();
            m_editButtons->check(0);
            this->setEditMode(WidgetMode::Select);
            this->setUseBlending(false);
            this->redraw();
        }

        if (key == 78)
            this->setEditMode(WidgetMode::CreateNode);

        if (key == 77)
            this->setEditMode(WidgetMode::Move);

        if (key == 66)
            this->setEditMode(WidgetMode::CreateLine);

        if (key == 261)
            this->deleteSelected();
    }
    else
        log("windows active / script active");
}

ivf::Shape *FemViewWindow::onPick(int x, int y)
{
    return m_beamModel->pick(x, y);
}

void FemViewWindow::onClipboardCreateNode(double x, double y, double z)
{
    log("CB: Create node x = " + to_string(x) + ", " + to_string(y) + ", " + to_string(z));

    this->addNode(x, y, z);
}

void FemViewWindow::onClipboardCreateElement(int i0, int i1)
{
    log("CB: Create element i0 = " + to_string(i0) + ", " + to_string(i1));

    this->addBeam(i0, i1);
}

void FemViewWindow::onDrawImGui()
{
    bool executeCalc = false;
    bool quitApplication = false;
    bool exportAsCalfem = false;
    bool importAsCalfem = false;
    bool runScriptDialog = false;
    bool snapShot = false;
    bool restoreLastSnapShot = false;

    {
        std::lock_guard<std::mutex> lock(m_scriptQueueMutex);

        if (!m_pendingScripts.empty())
        {
            auto scriptFunc = m_pendingScripts.front();
            m_pendingScripts.pop();
            this->runScriptFromText(scriptFunc);
            
            // Auto-fit workspace after AI-generated script execution
            this->fitWorkspaceToModel(1.2);
        }
    }


    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Start page", ""))
            {
                m_startPopup->show();
            }

            ImGui::Separator();

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
                m_openDialog = true;

            if (ImGui::MenuItem("Save", "Ctrl+S"))
                m_saveDialog = true;

            if (ImGui::MenuItem("Save as", "Ctrl+Shift+S"))
                m_saveAsDialog = true;

            ImGui::Separator();

            if (ImGui::MenuItem("Save as CALFEM...", ""))
                m_saveAsCalfemDialog = true;

            if (ImGui::MenuItem("Open from CALFEM...", ""))
                m_openFromCalfemDialog = true;

            ImGui::Separator();

            if (ImGui::MenuItem("New script...", ""))
            {
                m_scriptWindow->newScript();
                m_scriptWindow->show();
            }

            if (ImGui::MenuItem("Open script...", ""))
                m_openEditScriptDialog = true;

            if (ImGui::MenuItem("Run script...", ""))
                m_openScriptDialog = true;

            ImGui::Separator();

            if (ImGui::MenuItem("Preferences...", ""))
            {
                m_settingsWindow->show();
                m_settingsWindow->center();
            }

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

            if (ImGui::MenuItem("Copy", "Ctrl-C"))
                this->copy();
            if (ImGui::MenuItem("Paste", "Ctlr-V"))
                this->paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", "Ctrl-A"))
            {
                this->setSelectFilter(SelectMode::All);
                this->selectAllNodes();
            }
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

            ImGui::Separator();

            if (ImGui::MenuItem("Subdivide element", "Ctrl-D"))
                this->subdivideSelectedBeam();

            ImGui::Separator();

            if (ImGui::MenuItem("Mesh selected", "Ctrl-M"))
                this->meshSelectedNodes();
            if (ImGui::MenuItem("Surface selected no ground", ""))
                this->surfaceSelectedNodes(false);
            if (ImGui::MenuItem("Surface selected with ground", ""))
                this->surfaceSelectedNodes(true);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Properties...", ""))
            {
                m_propWindow->setVisible(true);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Node loads...", ""))
            {
                m_nodeLoadsWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet *)m_beamModel->getNodeLoadSet());
                m_nodeLoadsWindow->setVisible(true);
                this->setNeedRecalc(true);
            }

            if (ImGui::MenuItem("Element loads...", ""))
            {
                m_elementLoadsWindow->setFemLoadSet((ofem::BeamLoadSet *)m_beamModel->getElementLoadSet());
                m_elementLoadsWindow->setVisible(true);
                this->setNeedRecalc(true);
            }

            if (ImGui::MenuItem("Materials...", ""))
            {
                m_materialsWindow->setFemMaterialSet((ofem::BeamMaterialSet *)m_beamModel->getMaterialSet());
                m_materialsWindow->setVisible(true);
                this->setNeedRecalc(true);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Script editor...", ""))
            {
                m_scriptWindow->show();
                m_scriptWindow->center();
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
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Create using AI", ""))
            {
                m_promptWindow->show();
            }
            ImGui::Separator();
            for (auto &p : m_plugins)
            {
                if (ImGui::MenuItem(p->name().c_str(), ""))
                {
                    this->setCustomMode(CustomMode::Structure);
                    m_pluginWindow->setPlugin(p.get());
                    m_pluginWindow->center();
                    m_pluginWindow->show();
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Calc"))
        {
            if (ImGui::MenuItem("Execute", "Ctrl-R"))
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

            ImGui::Separator();

            if (ImGui::MenuItem("X-ray mode", "Alt-X"))
            {
                if (getUseBlending())
                    this->setUseBlending(false);
                else
                    this->setUseBlending(true);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Scaling settings...", ""))
            {
                m_scaleWindow->show();
                m_scaleWindow->setPosition(100, 20);
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About...", ""))
            {
                m_aboutWindow->show();
                m_aboutWindow->center();
            }
            if (ImGui::MenuItem("Homepage...", ""))
            {
#ifdef WIN32
                ShellExecuteW(0, 0, L"https://jonaslindemann.github.io/objectiveframe/", 0, 0, SW_SHOW);
#endif
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Log...", ""))
            {
                m_logWindow->show();
                m_logWindow->center();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Diagnostics", ""))
                m_showDiagnostics = !m_showDiagnostics;

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
    {}

    m_startPopup->draw();

    if (m_startPopup->closed())
    {}

    if (m_showStyleEditor)
        ImGui::ShowStyleEditor();

    if (m_showMetricsWindow)
        ImGui::ShowMetricsWindow(&m_showMetricsWindow);

    if (m_showDiagnostics)
        ImGui::ShowDemoWindow(&m_showDiagnostics);

    if (m_useImGuiFileDialogs)
    {
        if (m_openDialog)
        {
            IGFD::FileDialogConfig config;
            config.path = ofutil::get_config_value("last_dir", ofutil::samples_folder());

            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

            ImGuiFileDialog::Instance()->OpenDialog("Open model", "Choose File", ".df3", config);
        }

        if (ImGuiFileDialog::Instance()->Display("Open model", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                ofutil::set_config_value("last_dir", filePath);
                this->open(filePathName);
            }

            ImGuiFileDialog::Instance()->Close();
            m_openDialog = false;
        }

        if (m_saveDialog)
        {
            IGFD::FileDialogConfig config;

            if (m_fileName == "noname.df3")
            {
                config.path = ofutil::get_config_value("last_dir", ofutil::samples_folder());

                auto viewport = ImGui::GetMainViewport();
                ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
                ImVec2 work_size = viewport->WorkSize;
                ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

                ImGuiFileDialog::Instance()->OpenDialog("Save model", "Choose File", ".df3", config);
            }
            else
            {
                m_beamModel->save();
                if (m_saveScreenShot)
                    this->saveScreenShot(m_fileName + ".png");
                m_saveDialog = false;
            }
        }

        if (m_saveAsDialog)
        {
            IGFD::FileDialogConfig config;
            config.path = ofutil::get_config_value("last_dir", ofutil::samples_folder());

            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

            ImGuiFileDialog::Instance()->OpenDialog("Save model", "Choose File", ".df3", config);
        }

        if (m_saveAsCalfemDialog)
        {
            IGFD::FileDialogConfig config;
            config.path = ofutil::get_config_value("last_calfem_dir", ofutil::samples_folder());

            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

            ImGuiFileDialog::Instance()->OpenDialog("Save as CALFEM", "Choose File", ".py", config);
        }

        if (m_openFromCalfemDialog)
        {
            IGFD::FileDialogConfig config;
            config.path = ofutil::get_config_value("last_calfem_dir", ofutil::samples_folder());

            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

            ImGuiFileDialog::Instance()->OpenDialog("Open from CALFEM", "Choose File", ".py", config);
        }

        if (m_openScriptDialog)
        {
            IGFD::FileDialogConfig config;
            config.path = ofutil::get_config_value("last_script_dir", ofutil::samples_folder());

            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

            ImGuiFileDialog::Instance()->OpenDialog("Open script", "Choose File", ".chai", config);
        }

        if (m_openEditScriptDialog)
        {
            IGFD::FileDialogConfig config;
            config.path = ofutil::get_config_value("last_script_dir", ofutil::samples_folder());

            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

            ImGuiFileDialog::Instance()->OpenDialog("Edit script", "Choose File", ".chai", config);
        }

        if (m_newScriptDialog)
        {
            IGFD::FileDialogConfig config;
            config.path = ofutil::get_config_value("last_script_dir", ofutil::samples_folder());

            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

            ImGuiFileDialog::Instance()->OpenDialog("New script", "Choose File", ".chai", config);
        }

        if (ImGuiFileDialog::Instance()->Display("Save model", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                ofutil::set_config_value("last_dir", filePath);

                if (filePathName != "")
                {
                    this->setFileName(filePathName);
                    m_beamModel->setFileName(m_fileName);
                    m_beamModel->save();
                    if (m_saveScreenShot)
                        this->saveScreenShot(m_fileName + ".png");
                }
            }

            ImGuiFileDialog::Instance()->Close();
            m_saveAsDialog = false;
            m_saveDialog = false;
        }

        if (ImGuiFileDialog::Instance()->Display("Save as CALFEM", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                ofutil::set_config_value("last_calfem_dir", filePath);

                if (filePathName != "")
                {
                    this->exportAsCalfem(filePathName);
                }
            }

            ImGuiFileDialog::Instance()->Close();
            m_saveAsCalfemDialog = false;
        }

        if (ImGuiFileDialog::Instance()->Display("Open from CALFEM", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                ofutil::set_config_value("last_calfem_dir", filePath);

                if (filePathName != "")
                {
                    this->importAsCalfem(filePathName);
                }
            }

            ImGuiFileDialog::Instance()->Close();
            m_openFromCalfemDialog = false;
        }

        if (ImGuiFileDialog::Instance()->Display("Open script", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                ofutil::set_config_value("last_script_dir", filePath);

                if (filePathName != "")
                {
                    std::thread([this, filePathName]() {
                        try
                        {
                            // Lock rendering to prevent race conditions
                            this->lockSceneRendering();

                            // Execute script on background thread
                            this->runScript(filePathName);

                            // Unlock rendering
                            this->unlockSceneRendering();

                            // Queue a redraw
                            this->redraw();
                        } catch (const std::exception &e)
                        {
                            this->unlockSceneRendering();
                            log("Script execution error: " + std::string(e.what()));
                        }
                    }).detach();

                    // this->runScript(filePathName);
                }
            }

            ImGuiFileDialog::Instance()->Close();
            m_openScriptDialog = false;
        }

        if (ImGuiFileDialog::Instance()->Display("Edit script", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                ofutil::set_config_value("last_script_dir", filePath);

                if (filePathName != "")
                {
                    m_scriptWindow->open(filePathName);
                    m_scriptWindow->show();
                    m_scriptWindow->center();
                }
            }

            ImGuiFileDialog::Instance()->Close();
            m_openEditScriptDialog = false;
        }

        if (ImGuiFileDialog::Instance()->Display("New script", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                ofutil::set_config_value("last_script_dir", filePath);

                if (filePathName != "")
                {
                    m_scriptWindow->open(filePathName);
                    m_scriptWindow->show();
                    m_scriptWindow->center();
                }
            }

            ImGuiFileDialog::Instance()->Close();
            m_newScriptDialog = false;
        }
    }

    m_windowList->draw();

    ImGui::Render();

    if (executeCalc)
        this->executeCalc();

    if (quitApplication)
    {
        this->quit();
    }
}

void FemViewWindow::onInitImGui()
{
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    namespace fs = std::filesystem;

    fs::path filename = m_fontPath / fs::path("RopaSans-Regular.ttf");

    io.Fonts->AddFontFromFileTTF(filename.string().c_str(), 22);

    fs::path monoFilename = m_fontPath / fs::path("JetBrainsMono-Regular.ttf");

    io.Fonts->AddFontFromFileTTF(monoFilename.string().c_str(), 20);

    this->refreshUiStyle();
}

void FemViewWindow::onPostRender()
{
    // Update eigenmode animation
    if (m_eigenmodeWindow != nullptr && m_eigenmodeWindow->isAnimate())
    {
        // Update animation phase with delta time
        float deltaTime = ImGui::GetIO().DeltaTime;
        m_eigenmodeWindow->updateAnimationPhase(deltaTime);
        
        // Update visualization with current phase
        updateEigenmodeVisualization(m_eigenmodeWindow->getAnimationPhase());
    }
}

void FemViewWindow::onGlfwResize(int width, int height)
{
    if (m_mainToolbarWindow != nullptr)
        m_mainToolbarWindow->setPosition(20, 20);

    if (m_editToolbarWindow != nullptr)
        m_editToolbarWindow->setPosition(20, this->height() - 140);

    if (m_consoleWindow != nullptr)
        m_consoleWindow->setPosition(this->width() / 2.0 - m_consoleWindow->width() / 2.0, this->height() - 120);
}
