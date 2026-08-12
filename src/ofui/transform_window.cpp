#include <ofui/transform_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

using Handler = FemViewGeometryHandler;

namespace {

// The origins the panel offers, in combo order. Mirror gets its own shorter
// list - a plane through the middle of the selection would reflect it onto
// itself and the weld would then remove the copy.

const char *originNames = "World origin\0Centroid\0Bounding box centre\0Cursor\0";
const int originValues[4] = {0, 1, 2, 3};

const char *mirrorOriginNames = "World origin\0Low face\0High face\0";
const int mirrorOriginValues[3] = {0, 4, 5};

const char *axisNames = "X\0Y\0Z\0";

Handler::Origin toOrigin(int value)
{
    switch (value)
    {
    case 1:
        return Handler::Origin::Centroid;
    case 2:
        return Handler::Origin::BoundingBoxCentre;
    case 3:
        return Handler::Origin::Cursor;
    case 4:
        return Handler::Origin::BoundingBoxLow;
    case 5:
        return Handler::Origin::BoundingBoxHigh;
    default:
        return Handler::Origin::World;
    }
}

// Translates the widget values into a transform description.

Handler::TransformParams toParams(const TransformWindow::Fields &f)
{
    Handler::TransformParams p;

    p.origin = toOrigin(originValues[f.origin]);

    switch (f.mode)
    {
    case TransformWindow::Move:
        p.kind = Handler::TransformKind::Translate;
        p.delta[0] = f.translate[0];
        p.delta[1] = f.translate[1];
        p.delta[2] = f.translate[2];
        break;

    case TransformWindow::Scale:
        p.kind = Handler::TransformKind::Scale;

        for (int i = 0; i < 3; i++)
            p.factors[i] = f.uniformScale ? f.scaleUniform : f.scale[i];

        break;

    case TransformWindow::Rotate:
        p.kind = Handler::TransformKind::Rotate;
        p.axis[0] = (f.rotAxis == 0) ? 1.0 : 0.0;
        p.axis[1] = (f.rotAxis == 1) ? 1.0 : 0.0;
        p.axis[2] = (f.rotAxis == 2) ? 1.0 : 0.0;
        p.angleDeg = f.rotAngle;
        break;

    case TransformWindow::Taper:
        p.kind = Handler::TransformKind::Taper;
        p.taperAxis = f.taperAxis;
        p.s0 = f.taperS0;
        p.s1 = f.taperS1;
        break;

    case TransformWindow::Smooth:
        p.kind = Handler::TransformKind::Smooth;
        p.iterations = f.smoothIterations;
        p.lambda = f.smoothLambda;
        p.mu = f.smoothMu;
        p.lengthWeighted = f.smoothLengthWeighted;
        break;

    default:
        break;
    }

    // Only smoothing pins by default. An explicit transform should move
    // exactly what was selected.

    if (f.mode == TransformWindow::Smooth)
    {
        p.pins.bcNodes = f.pinBC;
        p.pins.loadedNodes = f.pinLoaded;
    }

    return p;
}

} // namespace

TransformWindow::TransformWindow(const std::string name) : UiWindow(name), m_view{nullptr}
{}

TransformWindow::~TransformWindow()
{}

void TransformWindow::setView(FemViewWindow *view)
{
    m_view = view;
}

std::shared_ptr<TransformWindow> TransformWindow::create(const std::string name)
{
    return std::make_shared<TransformWindow>(name);
}

bool TransformWindow::hasChange() const
{
    switch (m_fields.mode)
    {
    case Move:
        return (m_fields.translate[0] != 0.0f) || (m_fields.translate[1] != 0.0f) || (m_fields.translate[2] != 0.0f);

    case Scale:
        if (m_fields.uniformScale)
            return m_fields.scaleUniform != 1.0f;
        return (m_fields.scale[0] != 1.0f) || (m_fields.scale[1] != 1.0f) || (m_fields.scale[2] != 1.0f);

    case Rotate:
        return m_fields.rotAngle != 0.0f;

    case Taper:
        return (m_fields.taperS0 != 1.0f) || (m_fields.taperS1 != 1.0f);

    case Smooth:
        return m_fields.smoothIterations > 0;

    default:
        return false;
    }
}

void TransformWindow::resetFields()
{
    m_fields.translate[0] = m_fields.translate[1] = m_fields.translate[2] = 0.0f;
    m_fields.scaleUniform = 1.0f;
    m_fields.scale[0] = m_fields.scale[1] = m_fields.scale[2] = 1.0f;
    m_fields.rotAngle = 0.0f;
    m_fields.taperS0 = 1.0f;
    m_fields.taperS1 = 1.0f;
}

void TransformWindow::pushPreview()
{
    if (m_view == nullptr)
        return;

    if (!m_view->isTransformPreviewActive())
    {
        // Nothing selected, or everything pinned. The handler has already
        // reported why, so leave the fields as the user typed them.

        if (!m_view->beginTransformPreview())
            return;
    }

    m_view->updateTransformPreview(toParams(m_fields));
}

void TransformWindow::doDraw()
{
    if (m_view == nullptr)
        return;

    ImGui::Dummy(ImVec2(320.0f, 0.0f));

    int previousMode = m_fields.mode;

    if (ImGui::BeginTabBar("TransformTabs"))
    {
        if (ImGui::BeginTabItem("Move"))
        {
            m_fields.mode = Move;

            if (ImGui::DragFloat3("Delta", m_fields.translate, 0.01f))
                pushPreview();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Scale"))
        {
            m_fields.mode = Scale;

            if (ImGui::Checkbox("Uniform", &m_fields.uniformScale))
                pushPreview();

            if (m_fields.uniformScale)
            {
                if (ImGui::DragFloat("Factor", &m_fields.scaleUniform, 0.005f, 0.01f, 100.0f))
                    pushPreview();
            }
            else
            {
                if (ImGui::DragFloat3("Factors", m_fields.scale, 0.005f, 0.01f, 100.0f))
                    pushPreview();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rotate"))
        {
            m_fields.mode = Rotate;

            if (ImGui::Combo("Axis", &m_fields.rotAxis, axisNames))
                pushPreview();

            if (ImGui::DragFloat("Angle", &m_fields.rotAngle, 0.5f, -360.0f, 360.0f, "%.1f deg"))
                pushPreview();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Taper"))
        {
            m_fields.mode = Taper;

            if (ImGui::Combo("Along", &m_fields.taperAxis, axisNames))
                pushPreview();

            if (ImGui::DragFloat("Low end", &m_fields.taperS0, 0.005f, 0.01f, 10.0f))
                pushPreview();

            if (ImGui::DragFloat("High end", &m_fields.taperS1, 0.005f, 0.01f, 10.0f))
                pushPreview();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Smooth"))
        {
            m_fields.mode = Smooth;

            if (ImGui::SliderInt("Iterations", &m_fields.smoothIterations, 0, 100))
                pushPreview();

            if (ImGui::SliderFloat("Lambda", &m_fields.smoothLambda, 0.0f, 1.0f))
                pushPreview();

            if (ImGui::SliderFloat("Mu", &m_fields.smoothMu, -1.0f, 0.0f))
                pushPreview();

            if (m_fields.smoothMu == 0.0f)
                ImGui::TextUnformatted("Mu 0 is plain Laplacian - it shrinks.");

            if (ImGui::Checkbox("Weight by 1/length", &m_fields.smoothLengthWeighted))
                pushPreview();

            ImGui::Separator();

            if (ImGui::Checkbox("Hold fixed nodes", &m_fields.pinBC))
                pushPreview();

            if (ImGui::Checkbox("Hold loaded nodes", &m_fields.pinLoaded))
                pushPreview();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Mirror"))
        {
            m_fields.mode = Mirror;
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    // Switching tabs abandons whatever the previous tab was previewing. This
    // has to happen after the tab bar, because that is where the new mode
    // becomes known.

    bool modeChanged = (m_fields.mode != previousMode);

    if (modeChanged)
    {
        m_view->cancelTransformPreview();
        resetFields();

        // Smoothing has no neutral setting to sit at - opening the tab is
        // itself a request to see what it would do. Seeded once on entry, so
        // Reset still leaves the preview off until something is edited.

        if (m_fields.mode == Smooth)
            pushPreview();
    }

    ImGui::Separator();

    if (m_fields.mode == Mirror)
    {
        ImGui::Combo("Plane normal", &m_fields.mirrorAxis, axisNames);
        ImGui::Combo("About", &m_fields.mirrorOrigin, mirrorOriginNames);
        ImGui::DragFloat("Weld tolerance", &m_fields.weldTolerance, 0.0005f, 0.0f, 1.0f, "%.4f");

        ImGui::TextUnformatted("Copies geometry and materials.");
        ImGui::TextUnformatted("BCs and loads are not copied.");

        ImGui::Separator();

        if (ImGui::Button("Mirror"))
            m_view->mirrorSelection(m_fields.mirrorAxis, mirrorOriginValues[m_fields.mirrorOrigin],
                                    m_fields.weldTolerance);

        return;
    }

    // Move has no origin - a delta means the same thing wherever it is
    // measured from - and smoothing is defined by the topology rather than by
    // any point in space.

    if ((m_fields.mode != Move) && (m_fields.mode != Smooth))
    {
        if (ImGui::Combo("About", &m_fields.origin, originNames))
            pushPreview();
    }

    ImGui::Separator();

    bool active = m_view->isTransformPreviewActive();

    ImGui::BeginDisabled(!active || !hasChange());

    if (ImGui::Button("Apply"))
    {
        m_view->applyTransformPreview(toParams(m_fields));
        resetFields();
    }

    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(!active);

    if (ImGui::Button("Reset"))
    {
        m_view->cancelTransformPreview();
        resetFields();
    }

    ImGui::EndDisabled();

    if (active)
    {
        ImGui::SameLine();
        ImGui::TextUnformatted("previewing");
    }
}
