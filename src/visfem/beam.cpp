#include <vfem/beam.h>

#include <ivf/GlobalState.h>
#include <ivfmath/Vec3d.h>

#include <cmath>

using namespace ivf;
using namespace vfem;

namespace {

constexpr float kLowValueGreyR = 0.42f;
constexpr float kLowValueGreyG = 0.42f;
constexpr float kLowValueGreyB = 0.40f;

constexpr float kXrayLowValueR = 0.0f;
constexpr float kXrayLowValueG = 0.0f;
constexpr float kXrayLowValueB = 0.0f;

double clamp01(double value)
{
    if (value < 0.0)
        return 0.0;
    if (value > 1.0)
        return 1.0;
    return value;
}

double clampSigned(double value)
{
    if (value < -1.0)
        return -1.0;
    if (value > 1.0)
        return 1.0;
    return value;
}

double normalizeMagnitude(double value, double maxValue)
{
    const double denominator = std::fabs(maxValue);
    return (denominator > 0.0) ? clamp01(std::fabs(value) / denominator) : 0.0;
}

double normalizeSigned(double value, double minValue, double maxValue)
{
    const double negativeLimit = std::fabs(minValue);
    const double positiveLimit = std::fabs(maxValue);
    const double denominator = (positiveLimit > negativeLimit) ? positiveLimit : negativeLimit;
    return (denominator > 0.0) ? clampSigned(value / denominator) : 0.0;
}

double displayIntensity(double value, bool useBlending)
{
    const double t = clamp01(value);
    if (!useBlending)
        return t;

    constexpr double kXrayDeadBand = 0.03;
    if (t <= kXrayDeadBand)
        return 0.0;

    return std::pow((t - kXrayDeadBand) / (1.0 - kXrayDeadBand), 0.85);
}

void blendColor(double t, float r0, float g0, float b0, float r1, float g1, float b1, float &r, float &g, float &b)
{
    const float w = static_cast<float>(clamp01(t));
    r = r0 * (1.0f - w) + r1 * w;
    g = g0 * (1.0f - w) + g1 * w;
    b = b0 * (1.0f - w) + b1 * w;
}

void lowValueColor(bool useBlending, float &r, float &g, float &b)
{
    if (useBlending) {
        r = kXrayLowValueR;
        g = kXrayLowValueG;
        b = kXrayLowValueB;
        return;
    }

    r = kLowValueGreyR;
    g = kLowValueGreyG;
    b = kLowValueGreyB;
}

void axialResultColor(double force, double minForce, double maxForce, bool useBlending,
                       const BeamModel::ColorScale &scale, float &r, float &g, float &b)
{
    const double signedValue = normalizeSigned(force, minForce, maxForce);
    const float lowR = useBlending ? kXrayLowValueR : kLowValueGreyR;
    const float lowG = useBlending ? kXrayLowValueG : kLowValueGreyG;
    const float lowB = useBlending ? kXrayLowValueB : kLowValueGreyB;

    if (signedValue > 0.0) {
        const double t = displayIntensity(signedValue, useBlending);
        const auto *c = scale.axialPos;
        if (t < 0.25)
            blendColor(t / 0.25, lowR, lowG, lowB, c[0][0], c[0][1], c[0][2], r, g, b);
        else if (t < 0.50)
            blendColor((t - 0.25) / 0.25, c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], r, g, b);
        else if (t < 0.75)
            blendColor((t - 0.50) / 0.25, c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2], r, g, b);
        else
            blendColor((t - 0.75) / 0.25, c[2][0], c[2][1], c[2][2], c[3][0], c[3][1], c[3][2], r, g, b);
        return;
    }

    if (signedValue < 0.0) {
        const double t = displayIntensity(-signedValue, useBlending);
        const auto *c = scale.axialNeg;
        if (t < 0.25)
            blendColor(t / 0.25, lowR, lowG, lowB, c[0][0], c[0][1], c[0][2], r, g, b);
        else if (t < 0.50)
            blendColor((t - 0.25) / 0.25, c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], r, g, b);
        else if (t < 0.75)
            blendColor((t - 0.50) / 0.25, c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2], r, g, b);
        else
            blendColor((t - 0.75) / 0.25, c[2][0], c[2][1], c[2][2], c[3][0], c[3][1], c[3][2], r, g, b);
        return;
    }

    lowValueColor(useBlending, r, g, b);
}

void magnitudeResultColor(double magnitude, bool useBlending, const BeamModel::ColorScale &scale, float &r, float &g,
                           float &b)
{
    const double t = displayIntensity(magnitude, useBlending);
    const float lowR = useBlending ? kXrayLowValueR : kLowValueGreyR;
    const float lowG = useBlending ? kXrayLowValueG : kLowValueGreyG;
    const float lowB = useBlending ? kXrayLowValueB : kLowValueGreyB;
    const auto *c = scale.magnitude;

    if (t < 0.35)
        blendColor(t / 0.35, lowR, lowG, lowB, c[0][0], c[0][1], c[0][2], r, g, b);
    else if (t < 0.75)
        blendColor((t - 0.35) / 0.40, c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], r, g, b);
    else
        blendColor((t - 0.75) / 0.25, c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2], r, g, b);
}

void utilizationResultColor(double utilization, bool useBlending, const BeamModel::ColorScale &scale, float &r,
                             float &g, float &b)
{
    const auto *c = scale.navier;
    const auto *o = scale.navierOverload;

    if (utilization > 1.0) {
        blendColor((utilization - 1.0) / 0.25, c[2][0], c[2][1], c[2][2], o[0], o[1], o[2], r, g, b);
        return;
    }

    const double t = displayIntensity(utilization, useBlending);
    const float lowR = useBlending ? kXrayLowValueR : kLowValueGreyR;
    const float lowG = useBlending ? kXrayLowValueG : kLowValueGreyG;
    const float lowB = useBlending ? kXrayLowValueB : kLowValueGreyB;

    if (t < 0.70)
        blendColor(t / 0.70, lowR, lowG, lowB, c[0][0], c[0][1], c[0][2], r, g, b);
    else if (t < 0.90)
        blendColor((t - 0.70) / 0.20, c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], r, g, b);
    else
        blendColor((t - 0.90) / 0.10, c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2], r, g, b);
}

void unsupportedResultColor(bool useBlending, float &r, float &g, float &b)
{
    float grey = useBlending ? 0.10f : 0.70f;
    r = grey;
    g = grey;
    b = grey;
}

bool supportsResultType(ofem::Beam *beam, int resultType)
{
    if (beam == nullptr || resultType == IVF_BEAM_NO_RESULT)
        return false;

    if (beam->beamType() == ofem::btBar)
        return resultType == IVF_BEAM_N;

    return true;
}

void resetResultLineGeometry(vfem::BeamSolidLine *solidLine)
{
    if (solidLine == nullptr)
        return;

    solidLine->setUseOffsetConeCap(false);
    solidLine->setStartOffsets(0.0, 0.0);
    solidLine->setOffsets(0.0, 0.0);
}

} // namespace

Beam::Beam() : Composite()
{
    m_femBeam = nullptr;

    m_nodes[0] = nullptr;
    m_nodes[1] = nullptr;

    m_beamModel = nullptr;

    m_beamMaterial = ivf::Material::create();
    m_beamMaterial->setDiffuseColor(0.0f, 0.5f, 0.75f, 1.0f);
    m_beamMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Set up the solid line

    m_solidLine = BeamSolidLine::create();
    m_solidLine->setMaterial(m_beamMaterial);
    m_solidLine->setUseName(false);
    m_solidLine->setUseSelectShape(false);

    // Cache the swept tube in a display list. Without this, GLE re-tessellates the
    // extrusion and re-emits it in immediate mode on every frame, for every beam.
    // The list is invalidated by the Extrusion setters and by refresh() below, and
    // is bypassed automatically while the beam is selected or highlighted.
    //
    // Under OF_SWEPT_EXTRUSION this is a no-op: that implementation keeps the
    // mesh in a vertex buffer, which is already the cache a display list would
    // be wrapping. The invalidation calls below still apply either way.
    m_solidLine->setUselist(true);

    this->addChild(m_solidLine);

    // Set up line set

    m_lineSet = SimpleLineSet::create();
    m_lineSet->setCoordSize(2);
    m_lineSet->setLineSize(3);
    m_lineSet->setColorSize(2);
    m_lineSet->setLineIndex(0, 0);
    m_lineSet->setLineIndex(1, 1);
    m_lineSet->setLineIndex(2, -1);
    m_lineSet->setColor(0, 1.0, 1.0, 0.0);
    m_lineSet->setColor(1, 1.0, 1.0, 0.0);
    m_lineSet->setState(Shape::OS_OFF);
    m_lineSet->setUseName(false);
    m_lineSet->setUseSelectShape(false);
    this->addChild(m_lineSet);

    // Set up extrusion

    m_extrusion = BeamExtrusion::create();
    m_extrusion->setState(Shape::OS_OFF);
    m_extrusion->setMaterial(m_beamMaterial);
    m_extrusion->setUseName(false);
    m_extrusion->setUseSelectShape(false);
    this->addChild(m_extrusion);

    // Create a line indicating z-axis

    this->setUseName(true);
    this->setUseSelectShape(true);
}

Beam::~Beam()
{
    for (int i = 0; i < 2; i++) {
        if (m_nodes[i] != nullptr) {
            m_nodes[i]->deleteReference();
            if (!m_nodes[i]->referenced())
                delete m_nodes[i];
        }
    }
}

void Beam::setBeam(ofem::Beam *beam)
{
    m_femBeam = beam;
}

ofem::Beam *Beam::getBeam()
{
    return m_femBeam;
}

void Beam::refresh()
{
    if (m_femBeam != nullptr) {
        // Every visual property of the beam is (re)assigned from here, including
        // the material colours. The Extrusion setters invalidate the solid line's
        // display list on their own, but a material mutated through m_beamMaterial
        // is invisible to them, so invalidate up front and cover both.

        m_solidLine->markListDirty();
        m_extrusion->markListDirty();

        double x1, y1, z1;
        double x2, y2, z2;
        ofem::Node *node1 = m_nodes[0]->getFemNode();
        ofem::Node *node2 = m_nodes[1]->getFemNode();
        node1->getCoord(x1, y1, z1);
        node2->getCoord(x2, y2, z2);

        m_lineSet->setState(Shape::OS_OFF);
        m_solidLine->setState(Shape::OS_OFF);
        m_extrusion->setState(Shape::OS_OFF);

        if (m_beamModel != nullptr) {

            if (m_beamModel->getColorTable() == nullptr) {
                if (m_femBeam->getMaterial() != nullptr) {
                    m_beamMaterial->setDiffuseColor(1.0f, 1.0f, 0.0f, 1.0f);
                    m_beamMaterial->setSpecularColor(0.0f, 0.0f, 0.0f, 0.0f);
                }
                else {
                    m_beamMaterial->setDiffuseColor(0.5f, 0.5f, 0.5f, 1.0f);
                    m_beamMaterial->setAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
                }
            }
            else {
                if (m_femBeam->getMaterial() != nullptr) {
                    m_beamModel->getColorTable()->assignColor(m_femBeam->getMaterial()->getColor(), m_beamMaterial);
                }
                else {
                    m_beamMaterial->setDiffuseColor(0.5f, 0.5f, 0.5f, 1.0f);
                    m_beamMaterial->setAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
                }
            }

            m_lineSet->setState(Shape::OS_OFF);
            m_extrusion->setState(Shape::OS_OFF);
            m_solidLine->setState(Shape::OS_OFF);

            switch (m_beamModel->getBeamType()) {
            case IVF_BEAM_LINESET:
                m_lineSet->setCoord(0, x1, y1, z1);
                m_lineSet->setCoord(1, x2, y2, z2);
                m_lineSet->setState(Shape::OS_ON);
                break;
            case IVF_BEAM_SOLID:
                if (m_beamModel != nullptr)
                    m_solidLine->setRadius(m_beamModel->getLineRadius());

                m_solidLine->setUseColor(false);
                m_solidLine->setNodes(m_nodes[0], m_nodes[1]);
                m_solidLine->setState(Shape::OS_ON);

                m_solidLine->setUseOffsetConeCap(true);
                m_solidLine->setOffsetConeCapRadius(0.0);
                m_solidLine->setOffsets(-m_beamModel->getNodeSize() * 2.0, -m_beamModel->getNodeSize() * 2.0);
                m_solidLine->setStartOffsets(-m_beamModel->getNodeSize(), -m_beamModel->getNodeSize());

                if (m_solidLine->getSides() != m_beamModel->getLineSides())
                    m_solidLine->setSides(m_beamModel->getLineSides());

                if (this->getBeam()->beamType() == ofem::btBar) {
                    m_solidLine->setOffsets(-m_beamModel->getNodeSize() * 2.0, -m_beamModel->getNodeSize() * 2.0);
                    m_solidLine->setStartOffsets(-m_beamModel->getNodeSize(), -m_beamModel->getNodeSize());
                }
                else
                    m_solidLine->setOffsets(0.0, 0.0);

                m_solidLine->setTextureMode(0);
                break;
            case IVF_BEAM_RESULTS:
                if (m_beamModel != nullptr) {
                    if (supportsResultType(m_femBeam, m_beamModel->getResultType())) {
                        resetResultLineGeometry(m_solidLine);
                        m_solidLine->setRadius(m_beamModel->getLineRadius());
                        if (m_solidLine->getSides() != m_beamModel->getLineSides())
                            m_solidLine->setSides(m_beamModel->getLineSides());
                        m_solidLine->setNodes(m_nodes[0], m_nodes[1]);
                        m_solidLine->setState(Shape::OS_ON);
                        m_solidLine->setTextureMode(0);
                        m_beamMaterial->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
                        m_beamMaterial->setAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
                        m_beamMaterial->setSpecularColor(0.0f, 0.0f, 0.0f, 0.0f);
                        m_beamMaterial->setEmissionColor(0.0f, 0.0f, 0.0f, 1.0f);
                        initResultColors();
                    }
                    else {
                        resetResultLineGeometry(m_solidLine);
                        m_solidLine->setRadius(m_beamModel->getLineRadius());
                        m_solidLine->setNodes(m_nodes[0], m_nodes[1]);
                        m_solidLine->setState(Shape::OS_ON);
                        m_solidLine->setUseColor(false);
                        if (m_beamModel->getResultType() != IVF_BEAM_NO_RESULT) {
                            float grey = m_beamModel->getUseBlending() ? 0.10f : 0.70f;
                            m_beamMaterial->setDiffuseColor(grey, grey, grey, 1.0f);
                            m_beamMaterial->setAmbientColor(grey, grey, grey, 1.0f);
                        }
                        m_solidLine->setTextureMode(0);
                    }
                }
                break;
            case IVF_BEAM_EXTRUSION:
                if (m_femBeam != nullptr) {
                    if (m_femBeam->getMaterial() != nullptr) {
                        initExtrusion();
                        m_extrusion->setState(Shape::OS_ON);
                    }
                }
                break;
            default:

                break;
            }
        }
    }
}

void Beam::doCreateGeometry()
{
    if (m_femBeam != nullptr) {
        bool hasResultBeam = (m_beamModel != nullptr) &&
            (m_beamModel->getBeamType() == IVF_BEAM_RESULTS) &&
            (m_beamModel->getResultType() != IVF_BEAM_NO_RESULT) &&
            supportsResultType(m_femBeam, m_beamModel->getResultType());
        auto globalState = GlobalState::getInstance();
        bool renderResultColors = hasResultBeam && globalState->isColorOutputEnabled();
        bool suppressResultColors = hasResultBeam && !renderResultColors;
        bool oldUseResultColors = false;

        // glPushAttrib(GL_ALL_ATTRIB_BITS) saves the entire GL state vector and is
        // one of the most expensive calls in the legacy API -- far too costly to
        // pay once per beam per frame. Only a handful of enables actually change
        // here, so set and restore exactly those.
        //
        // These are set unconditionally rather than probed with glIsEnabled():
        // state queries can force a driver flush, which is a worse deal than the
        // redundant glEnable/glDisable pairs they would save. Textures are left
        // disabled afterwards because that is the scene's resting state -- shapes
        // that use a texture enable and disable it around themselves in
        // Shape::doBeginTransform/doEndTransform.

        bool useBlending = false;

        if (renderResultColors) {
            useBlending = m_beamModel->getUseBlending();

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_1D);
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

            // glColorMaterial() changes material state without going through
            // ivf::Material, so its redundancy cache can no longer be trusted.
            ivf::Material::invalidateStateCache();

            if (useBlending) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glDisable(GL_DEPTH_TEST);
            }
        }
        else if (suppressResultColors) {
            oldUseResultColors = m_solidLine->getUseColor();

            // These two toggles are reverted immediately after the draw, which
            // would leave the display list permanently dirty and recompiling every
            // frame. Draw directly instead for as long as the suppression lasts.
            m_solidLine->setDynamic(true);
            m_solidLine->setUseColor(false);
            m_solidLine->setTextureMode(0);
        }

        Composite::doCreateGeometry();

        if (renderResultColors) {
            if (useBlending) {
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
            }

            glDisable(GL_COLOR_MATERIAL);

            ivf::Material::invalidateStateCache();
        }
        else if (suppressResultColors) {
            m_solidLine->setUseColor(oldUseResultColors);
            m_solidLine->setDynamic(false);
        }
    }
}

void Beam::setLineRefreshMode(ivf::LineRefreshMode mode)
{
    m_solidLine->setRefresh(mode);
}

void Beam::setDynamicGeometry(bool flag)
{
    m_solidLine->setDynamic(flag);
    m_extrusion->setDynamic(flag);
}

bool Beam::resultColorAtEvaluationPoint(int idx, float &red, float &green, float &blue)
{
    if ((m_beamModel == nullptr) || (m_femBeam == nullptr))
        return false;

    double value = 0.0;
    double v1 = 0.0;
    double v2 = 0.0;
    double N = 0.0;
    double My = 0.0;
    double Mz = 0.0;
    const bool useBlending = m_beamModel->getUseBlending();
    const BeamModel::ColorScale &colorScale = m_beamModel->colorScale();

    red = green = blue = 0.0f;

    if (m_femBeam->beamType() == ofem::btBeam) {
        switch (m_beamModel->getResultType()) {
        case IVF_BEAM_N:
            value = m_femBeam->getValue(0 + 6 * idx);
            axialResultColor(value, m_beamModel->minN(), m_beamModel->maxN(), useBlending, colorScale, red, green,
                              blue);
            break;
        case IVF_BEAM_T:
            value = m_femBeam->getValue(1 + 6 * idx);
            value = normalizeMagnitude(value, m_beamModel->maxT());
            magnitudeResultColor(value, useBlending, colorScale, red, green, blue);
            break;
        case IVF_BEAM_V:
            v1 = m_femBeam->getValue(2 + 6 * idx);
            v2 = m_femBeam->getValue(3 + 6 * idx);
            value = std::sqrt(v1 * v1 + v2 * v2);
            value = normalizeMagnitude(value, m_beamModel->maxV());
            magnitudeResultColor(value, useBlending, colorScale, red, green, blue);
            break;
        case IVF_BEAM_M:
            v1 = m_femBeam->getValue(4 + 6 * idx);
            v2 = m_femBeam->getValue(5 + 6 * idx);
            value = std::sqrt(v1 * v1 + v2 * v2);
            value = normalizeMagnitude(value, m_beamModel->maxM());
            magnitudeResultColor(value, useBlending, colorScale, red, green, blue);
            break;
        case IVF_BEAM_NAVIER:
            N = m_femBeam->getValue(0 + 6 * idx);
            My = m_femBeam->getValue(4 + 6 * idx);
            Mz = m_femBeam->getValue(5 + 6 * idx);
            value = calcNavier(N, My, Mz);
            value = normalizeMagnitude(value, m_beamModel->maxNavier());
            utilizationResultColor(value, useBlending, colorScale, red, green, blue);
            break;
        default:
            unsupportedResultColor(useBlending, red, green, blue);
            break;
        }
    }
    else {
        switch (m_beamModel->getResultType()) {
        case IVF_BEAM_N:
            value = m_femBeam->getValue(0 + 6 * idx);
            axialResultColor(value, m_beamModel->minN(), m_beamModel->maxN(), useBlending, colorScale, red, green,
                              blue);
            break;
        default:
            unsupportedResultColor(useBlending, red, green, blue);
            break;
        }
    }

    return true;
}

void Beam::initResultColors()
{
    if ((m_beamModel == nullptr) || (m_femBeam == nullptr) || (m_nodes[0] == nullptr) || (m_nodes[1] == nullptr))
        return;

    const int n = m_femBeam->getEvaluationPoints();
    if (n <= 0)
        return;

    if (m_femBeam->getMaterial() != nullptr) {
        m_femBeam->getMaterial()->getProperties(m_E, m_G, m_A, m_Iy, m_Iz, m_Kv);
        if (m_femBeam->getMaterial()->getSection() != nullptr) {
            m_femBeam->getMaterial()->getSection()->getExcY(m_eyMax, m_eyMin);
            m_femBeam->getMaterial()->getSection()->getExcZ(m_ezMax, m_ezMin);
        }
    }

    double x1, y1, z1;
    double x2, y2, z2;
    m_nodes[0]->getPosition(x1, y1, z1);
    m_nodes[1]->getPosition(x2, y2, z2);

    const double vx = x2 - x1;
    const double vy = y2 - y1;
    const double vz = z2 - z1;
    const double length = std::sqrt(vx * vx + vy * vy + vz * vz);
    if (length <= 0.0)
        return;

    constexpr double capLength = 0.1;
    const int spineSize = n + 2;

    m_solidLine->setTextureMode(0);
    m_solidLine->setUseColor(true);
    m_solidLine->setSpineSize(spineSize);

    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;

    for (int i = 0; i < spineSize; i++) {
        int resultIdx = i - 1;
        double t = 0.0;

        if (i == 0) {
            resultIdx = 0;
            t = -capLength / length;
        }
        else if (i == spineSize - 1) {
            resultIdx = n - 1;
            t = 1.0 + capLength / length;
        }
        else {
            resultIdx = i - 1;
            t = (n > 1) ? static_cast<double>(resultIdx) / static_cast<double>(n - 1) : 0.0;
        }

        if (!resultColorAtEvaluationPoint(resultIdx, red, green, blue))
            unsupportedResultColor(m_beamModel->getUseBlending(), red, green, blue);

        m_solidLine->setSpineCoord(i, x1 + vx * t, y1 + vy * t, z1 + vz * t);
        m_solidLine->setSpineColor(i, red, green, blue);
        m_solidLine->setSpineScale(i, 1.0, 1.0);
    }
}

void Beam::setNodes(vfem::Node *node1, vfem::Node *node2)
{
    for (int i = 0; i < 2; i++) {
        if (m_nodes[i] != nullptr) {
            m_nodes[i]->deleteReference();
            if (!m_nodes[i]->referenced())
                delete m_nodes[i];
        }
    }
    m_nodes[0] = node1;
    m_nodes[1] = node2;
    m_nodes[0]->addReference();
    m_nodes[1]->addReference();
}

vfem::Node *vfem::Beam::getNode(int idx)
{
    if ((idx >= 0) && (idx < 2))
        return m_nodes[idx];
    else
        return nullptr;
}

void Beam::doCreateSelect()
{
    if (m_beamModel != nullptr) {
        switch (m_beamModel->getBeamType()) {
        case IVF_BEAM_LINESET:

            break;
        case IVF_BEAM_SOLID:
        case IVF_BEAM_RESULTS:
            m_solidLine->setSelect(Shape::SS_ON);
            m_solidLine->render();
            m_solidLine->setSelect(Shape::SS_OFF);

            break;
        case IVF_BEAM_EXTRUSION:
            if (m_femBeam->getMaterial() != nullptr) {
                if (m_extrusion->getState() == Shape::OS_ON) {
                    // Shape::useDisplayList() already declines to replay a cached
                    // list while the select state is on, so this draws live without
                    // having to destroy and recompile the list around it.
                    m_extrusion->setSelect(Shape::SS_ON);
                    m_extrusion->render();
                    m_extrusion->setSelect(Shape::SS_OFF);
                }
                else {
                    m_solidLine->setSelect(Shape::SS_ON);
                    m_solidLine->render();
                    m_solidLine->setSelect(Shape::SS_OFF);
                }
            }
            break;
        default:

            break;
        }
    }
}

void Beam::initExtrusion()
{
    Vec3d p1, p2;
    Vec3d v1;
    double z, ex, ey, ez;

    // Create section

    m_extrusion->setUseTwist(true);

    if ((m_femBeam != nullptr) && (m_femBeam->getMaterial() != nullptr) &&
        (m_femBeam->getMaterial()->getSection() != nullptr)) {
        ofem::Section *section = m_femBeam->getMaterial()->getSection();
        double x, y;
        m_extrusion->setSectionSize(static_cast<int>(section->getSize()));
        for (unsigned int i = 0; i < section->getSize(); i++) {
            section->getCoord(i, x, y);
            m_extrusion->setSectionCoord(i, x, y);
            if (i < section->getSize() - 1) {
                section->getNormal(i, x, y);
                m_extrusion->setSectionNormal(i, x, y);
            }
        }
        // Set topolgy

        m_extrusion->setSpineSize(4);
        m_nodes[0]->getPosition(x, y, z);
        m_extrusion->setSpineCoord(1, x, y, z);
        m_nodes[1]->getPosition(x, y, z);
        m_extrusion->setSpineCoord(2, x, y, z);
        m_extrusion->setSpineTwist(1, m_femBeam->getBeamRotation());
        m_extrusion->setSpineTwist(2, m_femBeam->getBeamRotation());

        // Calculate start and endpoint

        m_extrusion->getSpineCoord(1, x, y, z);
        p1.setComponents(x, y, z);
        m_extrusion->getSpineCoord(2, x, y, z);
        p2.setComponents(x, y, z);

        v1.setFromPoints(p1, p2);
        v1.normalize();
        v1.getComponents(ex, ey, ez);

        if ((ex == 0) && (ez == 0) && (ey != 0))
            m_extrusion->setUpVector(1.0, 0.0, 0.0);
        else
            m_extrusion->setUpVector(0.0, 1.0, 0.0);

        // First point

        p1.getComponents(x, y, z);
        x = -0.1 * ex + x;
        y = -0.1 * ey + y;
        z = -0.1 * ez + z;
        m_extrusion->setSpineCoord(0, x, y, z);

        m_extrusion->getSpineCoord(m_extrusion->getSpineSize() - 3, x, y, z);
        p1.setComponents(x, y, z);
        m_extrusion->getSpineCoord(m_extrusion->getSpineSize() - 2, x, y, z);
        p2.setComponents(x, y, z);

        v1.setFromPoints(p1, p2);
        v1.normalize();
        v1.getComponents(ex, ey, ez);

        // First point

        p1.getComponents(x, y, z);
        x = 0.1 * ex + x;
        y = 0.1 * ey + y;
        z = 0.1 * ez + z;
        m_extrusion->setSpineCoord(m_extrusion->getSpineSize() - 1, x, y, z);

        // Cache the (expensive) GLE tessellation in a display list. The list is
        // recompiled here whenever the extrusion geometry is (re)initialized, i.e.
        // whenever the beam is refreshed due to structural edits, load changes or
        // a calculation run (see BeamModel::generateModel / FemViewWindow::refreshBeamModelVisuals).
        // This avoids re-tessellating the extrusion via GLE on every single frame.
        m_extrusion->setUselist(true);
    }
}

void Beam::setBeamModel(BeamModel *beamModel)
{
    m_beamModel = beamModel;
}

double Beam::calcNavier(double N, double My, double Mz)
{
    double sig[4];
    double sigN;
    double maxSig;
    int i;

    sigN = N / m_A;

    for (i = 0; i < 4; i++)
        sig[i] = sigN;

    sig[0] += Mz * m_ezMax / m_Iz;
    sig[1] += Mz * m_ezMax / m_Iz;
    sig[2] -= Mz * m_ezMin / m_Iz;
    sig[3] -= Mz * m_ezMin / m_Iz;

    sig[0] += My * m_eyMax / m_Iy;
    sig[1] -= My * m_eyMin / m_Iy;
    sig[2] += My * m_eyMax / m_Iy;
    sig[3] -= My * m_eyMax / m_Iy;

    maxSig = -1.0e300;

    for (i = 0; i < 4; i++)
        if (fabs(sig[i]) > maxSig)
            maxSig = fabs(sig[i]);

    return maxSig;
}
