#pragma once

#include <string>

namespace ofservice {

class IAppController {
public:
    virtual ~IAppController() = default;

    // ── Node / beam creation ──────────────────────────────────────────────────
    virtual void   addNode(double x, double y, double z) = 0;
    virtual void   addBeam(int i0, int i1) = 0;

    // ── Model lifecycle ───────────────────────────────────────────────────────
    virtual void   newModel() = 0;
    virtual void   open(const std::string &filename) = 0;
    virtual void   save(const std::string &filename) = 0;
    virtual void   exportAsCalfem(const std::string &filename) = 0;
    virtual void   importAsCalfem(const std::string &filename) = 0;
    virtual void   snapShot() = 0;

    // ── Selection ─────────────────────────────────────────────────────────────
    virtual void   selectAll() = 0;
    virtual void   selectAllNodes() = 0;
    virtual void   clearSelection() = 0;
    virtual void   addLastNodeToSelection() = 0;
    virtual void   selectNodeAt(int i) = 0;
    virtual void   selectBeamAt(int i) = 0;

    // ── Node / beam mutation ──────────────────────────────────────────────────
    virtual void   deleteNodeAt(int i) = 0;
    virtual void   deleteBeamAt(int i) = 0;
    virtual void   subdivideBeamAt(int i) = 0;
    virtual void   connectNearNodes(double tolerance) = 0;
    virtual void   updateNodePosAt(int i, double x, double y, double z) = 0;
    virtual void   updateBeamAt(int i, int i0, int i1) = 0;

    // ── Boundary conditions ───────────────────────────────────────────────────
    virtual void   assignNodeFixedBCGround() = 0;
    virtual void   assignNodePosBCGround() = 0;
    virtual void   assignNodeFixedBCAt(int i) = 0;
    virtual void   assignNodePosBCAt(int i) = 0;
    virtual void   removeNodeBCAt(int i) = 0;

    // ── Loads ─────────────────────────────────────────────────────────────────
    virtual void   clearAllLoads() = 0;
    virtual void   clearAllBCs() = 0;
    virtual void   addNodeLoadAt(int i, double fx, double fy, double fz) = 0;
    virtual void   clearNodeLoadAt(int i) = 0;
    virtual void   addBeamLoadAt(int i, double fx, double fy, double fz) = 0;
    virtual void   clearBeamLoadAt(int i) = 0;

    // ── Mesh generation ───────────────────────────────────────────────────────
    virtual void   meshSelectedNodes() = 0;
    virtual void   surfaceSelectedNodes() = 0;

    // ── Queries ───────────────────────────────────────────────────────────────
    virtual size_t nodeCount() = 0;
    virtual size_t beamCount() = 0;
    virtual void   nodePosAt(int i, double &x, double &y, double &z) = 0;
    virtual void   beamAt(int i, int &i0, int &i1) = 0;
    virtual int    findNodeNear(double x, double y, double z, double tolerance) = 0;
    virtual bool   isNodeFixedAt(int i) = 0;
    virtual bool   isNodePosBCAt(int i) = 0;
    virtual bool   isNodeSelectedAt(int i) = 0;
    virtual bool   hasNodeLoadAt(int i) = 0;
    virtual bool   hasBeamLoadAt(int i) = 0;
    virtual size_t nodeLoadCount() = 0;
    virtual size_t beamLoadCount() = 0;
    virtual size_t materialCount() = 0;
    virtual void   modelBounds(double &xmin, double &ymin, double &zmin,
                               double &xmax, double &ymax, double &zmax) = 0;

    // ── Window control (internal) ─────────────────────────────────────────────
    virtual void   disable() = 0;
    virtual void   enable() = 0;
};

} // namespace ofservice
