#pragma once

#include <ivf/BitmapFont.h>
#include <ivf/Node.h>
#include <ivf/Scene.h>

#include <ofem/beam_model.h>
#include <vfem/color_table.h>

#include <string>

#define IVF_BEAM_N 0
#define IVF_BEAM_T 1
#define IVF_BEAM_V 2
#define IVF_BEAM_M 3
#define IVF_BEAM_NAVIER 4
#define IVF_BEAM_NO_RESULT -1

#define IVF_BEAM_LINESET 0
#define IVF_BEAM_SOLID 1
#define IVF_BEAM_EXTRUSION 2
#define IVF_BEAM_RESULTS 3

#define IVF_NODE_GEOMETRY 0
#define IVF_NODE_DISPLACEMENT 1

namespace vfem {

SmartPointer(BeamModel);

class BeamModel : public ofem::BeamModel {
public:
    // Colors used by the procedural result color scales in vfem::Beam. Each
    // scale is a small set of RGB stops that get blended between at render
    // time (see axialResultColor/magnitudeResultColor/utilizationResultColor
    // in beam.cpp). Default values match the previous hard-coded palette.
    struct ColorScale {
        float axialPos[4][3] = {
            {0.82f, 0.36f, 0.34f},
            {1.00f, 0.00f, 0.00f},
            {0.86f, 0.00f, 0.00f},
            {0.45f, 0.00f, 0.00f},
        };
        float axialNeg[4][3] = {
            {0.34f, 0.38f, 0.82f},
            {0.00f, 0.00f, 1.00f},
            {0.00f, 0.00f, 0.75f},
            {0.00f, 0.00f, 0.35f},
        };
        float magnitude[3][3] = {
            {0.10f, 0.55f, 0.82f},
            {1.00f, 0.72f, 0.16f},
            {0.95f, 0.18f, 0.12f},
        };
        float navier[3][3] = {
            {0.10f, 0.55f, 0.82f},
            {1.00f, 0.72f, 0.16f},
            {0.95f, 0.18f, 0.12f},
        };
        float navierOverload[3] = {0.80f, 0.00f, 0.70f};
    };

private:
    double m_nodeSize;
    double m_lineRadius;
    double m_beamPickFactor;
    int m_lineSides;
    double m_loadSize;
    double m_beamLoadSize;
    double m_scaleFactor;

    double m_maxReactionForce;
    double m_maxReactionMoment;

    int m_beamType;
    int m_resultType;
    int m_nodeType;

    ivf::Node::TNodeType m_nodeRepr;

    bool m_showNodeNumbers;
    bool m_showElementNumbers;
    bool m_showLoads;
    bool m_showReactionForces;
    bool m_useBlending;

    ivf::CompositePtr m_scene;
    ivf::MaterialPtr m_nodeMaterial;
    ivf::MaterialPtr m_beamMaterial;
    ivf::BitmapFontPtr m_textFont;
    ivf::CameraPtr m_camera;

    ColorTablePtr m_colorTable;

    ColorScale m_colorScale;

    ofem::BeamNodeBCPtr m_defaultNodePosBC;
    ofem::BeamNodeBCPtr m_defaultNodeFixedBC;

protected:
    virtual void onInitialised() override;
    virtual void onReadComplete() override;

public:
    BeamModel();
    virtual ~BeamModel();

    ClassInfo("vfem::BeamModel", ofem::BeamModel);

    static vfem::BeamModelPtr create();

    void generateModel();

    void enumerate();

    void setScene(ivf::Composite *scene);
    ivf::Composite *getScene();

    void setLineRadius(double radius);
    double getLineRadius();

    // Beam pick radius is a multiple of the visual radius, so that slim beams
    // stay easy to select. Model space, so it scales with zoom like everything else.
    void setBeamPickFactor(double factor);
    double getBeamPickFactor();

    void setNodeSize(double size);
    double getNodeSize();

    void setNodeMaterial(ivf::Material *material);
    ivf::Material *getNodeMaterial();

    int getNodeType();
    void setNodeType(int type);

    void setBeamLoadSize(double size);
    double getBeamLoadSize();

    void setLoadSize(double size);
    double getLoadSize();

    void setLineSides(int sides);
    int getLineSides();

    void setBeamMaterial(ivf::Material *material);

    ColorTable *getColorTable();

    ColorScale &colorScale();
    void resetColorScaleToDefaults();

    void setPath(const std::string &path);

    void setScaleFactor(double factor);
    double getScaleFactor();

    void setNodeRepr(ivf::Node::TNodeType type);
    ivf::Node::TNodeType getNodeRepr();

    void setUseBlending(bool flag);
    bool getUseBlending();

    void setResultType(int type);
    int getResultType();

    void setBeamType(int type);
    int getBeamType();

    void setTextFont(ivf::BitmapFont *font);
    ivf::BitmapFont *textFont();

    void setShowNodeNumbers(bool flag);
    bool showNodeNumbers();

    void setShowLoads(bool flag);
    bool showLoads();

    void setShowReactionForces(bool flag);
    bool showReactionForces();

    void setCamera(ivf::Camera *camera);
    ivf::Camera *camera();

    void setMaxReactionForce(double force);
    void setMaxReactionMoment(double moment);

    double getMaxReactionForce();
    double getMaxReactionMoment();

    ofem::BeamNodeBC *defaultNodePosBC();
    ofem::BeamNodeBC *defaultNodeFixedBC();

    ivf::Shape *pick(int sx, int sy);
};
} // namespace vfem
