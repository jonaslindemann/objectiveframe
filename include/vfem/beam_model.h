#pragma once

#include <ivf/BitmapFont.h>
#include <ivf/Node.h>
#include <ivf/Scene.h>

#include <ofem/beam_model.h>
#include <vfem/color_table.h>

#include <ColorMap.h>
// #include <ResultInfo.h>

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
private:
    double m_nodeSize;
    double m_lineRadius;
    int m_lineSides;
    double m_loadSize;
    double m_beamLoadSize;
    double m_scaleFactor;

    int m_beamType;
    int m_resultType;
    int m_nodeType;

    ivf::Node::TNodeType m_nodeRepr;

    bool m_showNodeNumbers;
    bool m_showElementNumbers;
    bool m_useBlending;

    ivf::CompositePtr m_scene;
    ivf::MaterialPtr m_nodeMaterial;
    ivf::MaterialPtr m_beamMaterial;
    ivf::BitmapFontPtr m_textFont;
    ivf::CameraPtr m_camera;

    ColorTablePtr m_colorTable;

    ColorMapPtr m_colorMapPos;
    ColorMapPtr m_colorMapNeg;
    ColorMapPtr m_colorMapStd;

    ColorMapPtr m_colorMapPosBlack;
    ColorMapPtr m_colorMapNegBlack;

    std::string m_colorMapPath;

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

    void setColorMaps(ColorMapPtr pos, ColorMapPtr neg, ColorMapPtr std);

    void setLineRadius(double radius);
    double getLineRadius();

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

    void setPath(const std::string &path);

    void setScaleFactor(double factor);
    double getScaleFactor();

    void setNodeRepr(ivf::Node::TNodeType type);
    ivf::Node::TNodeType getNodeRepr();

    void setUseBlending(bool flag);
    bool getUseBlending();

    ColorMapPtr getColorMapStd();
    ColorMapPtr getColorMapNeg();
    ColorMapPtr getColorMapPos();

    ColorMapPtr getColorMapNegBlack();
    ColorMapPtr getColorMapPosBlack();

    // ResultInfo* getResultInfo();

    void setResultType(int type);
    int getResultType();

    void setBeamType(int type);
    int getBeamType();

    void setTextFont(ivf::BitmapFont *font);
    ivf::BitmapFont *textFont();

    void setShowNodeNumbers(bool flag);
    bool showNodeNumbers();

    void setCamera(ivf::Camera *camera);
    ivf::Camera *camera();

    ofem::BeamNodeBC *defaultNodePosBC();
    ofem::BeamNodeBC *defaultNodeFixedBC();

    ivf::Shape *pick(int sx, int sy);
};
} // namespace vfem
