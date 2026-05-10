#pragma once

#include <ofservice/iapp_controller.h>

class FemViewWindow;

class AppControllerAdapter : public ofservice::IAppController {
    FemViewWindow &m_view;

public:
    explicit AppControllerAdapter(FemViewWindow &view);

    // Node / beam creation
    void   addNode(double x, double y, double z) override;
    void   addBeam(int i0, int i1) override;

    // Model lifecycle
    void   newModel() override;
    void   open(const std::string &filename) override;
    void   save(const std::string &filename) override;
    void   exportAsCalfem(const std::string &filename) override;
    void   importAsCalfem(const std::string &filename) override;
    void   snapShot() override;

    // Selection
    void   selectAll() override;
    void   selectAllNodes() override;
    void   clearSelection() override;
    void   addLastNodeToSelection() override;
    void   selectNodeAt(int i) override;
    void   selectBeamAt(int i) override;

    // Node / beam mutation
    void   deleteNodeAt(int i) override;
    void   deleteBeamAt(int i) override;
    void   subdivideBeamAt(int i) override;
    void   connectNearNodes(double tolerance) override;
    void   updateNodePosAt(int i, double x, double y, double z) override;
    void   updateBeamAt(int i, int i0, int i1) override;

    // Boundary conditions
    void   assignNodeFixedBCGround() override;
    void   assignNodePosBCGround() override;
    void   assignNodeFixedBCAt(int i) override;
    void   assignNodePosBCAt(int i) override;
    void   removeNodeBCAt(int i) override;

    // Loads
    void   clearAllLoads() override;
    void   clearAllBCs() override;
    void   addNodeLoadAt(int i, double fx, double fy, double fz) override;
    void   clearNodeLoadAt(int i) override;
    void   addBeamLoadAt(int i, double fx, double fy, double fz) override;
    void   clearBeamLoadAt(int i) override;

    // Mesh generation
    void   meshSelectedNodes() override;
    void   surfaceSelectedNodes() override;

    // Queries
    size_t nodeCount() override;
    size_t beamCount() override;
    void   nodePosAt(int i, double &x, double &y, double &z) override;
    void   beamAt(int i, int &i0, int &i1) override;
    int    findNodeNear(double x, double y, double z, double tolerance) override;
    bool   isNodeFixedAt(int i) override;
    bool   isNodePosBCAt(int i) override;
    bool   isNodeSelectedAt(int i) override;
    bool   hasNodeLoadAt(int i) override;
    bool   hasBeamLoadAt(int i) override;
    size_t nodeLoadCount() override;
    size_t beamLoadCount() override;
    size_t materialCount() override;
    void   modelBounds(double &xmin, double &ymin, double &zmin,
                       double &xmax, double &ymax, double &zmax) override;

    // Window control
    void   disable() override;
    void   enable() override;
};
