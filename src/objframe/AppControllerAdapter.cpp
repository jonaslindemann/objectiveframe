#include "AppControllerAdapter.h"
#include "FemView.h"

AppControllerAdapter::AppControllerAdapter(FemViewWindow &view) : m_view{view} {}

// Node / beam creation
void   AppControllerAdapter::addNode(double x, double y, double z)      { m_view.addNode(x, y, z); }
void   AppControllerAdapter::addBeam(int i0, int i1)                    { m_view.addBeam(i0, i1); }

// Model lifecycle
void   AppControllerAdapter::newModel()                                  { m_view.newModel(); }
void   AppControllerAdapter::open(const std::string &f)                  { m_view.open(f); }
void   AppControllerAdapter::save(const std::string &f)                  { m_view.save(f); }
void   AppControllerAdapter::exportAsCalfem(const std::string &f)        { m_view.exportAsCalfem(f); }
void   AppControllerAdapter::importAsCalfem(const std::string &f)        { m_view.importAsCalfem(f); }
void   AppControllerAdapter::snapShot()                                  { m_view.snapShot(); }

// Selection
void   AppControllerAdapter::selectAll()                                 { m_view.selectAll(); }
void   AppControllerAdapter::selectAllNodes()                            { m_view.selectAllNodes(); }
void   AppControllerAdapter::clearSelection()                            { m_view.clearSelection(); }
void   AppControllerAdapter::addLastNodeToSelection()                    { m_view.addLastNodeToSelection(); }
void   AppControllerAdapter::selectNodeAt(int i)                         { m_view.selectNodeAt(i); }
void   AppControllerAdapter::selectBeamAt(int i)                         { m_view.selectBeamAt(i); }

// Node / beam mutation
void   AppControllerAdapter::deleteNodeAt(int i)                         { m_view.deleteNodeAt(i); }
void   AppControllerAdapter::deleteBeamAt(int i)                         { m_view.deleteBeamAt(i); }
void   AppControllerAdapter::subdivideBeamAt(int i)                      { m_view.subdivideBeamAt(i); }
void   AppControllerAdapter::connectNearNodes(double tol)                { m_view.connectNearNodes(tol); }
void   AppControllerAdapter::updateNodePosAt(int i, double x, double y, double z)
                                                                         { m_view.updateNodePosAt(i, x, y, z); }
void   AppControllerAdapter::updateBeamAt(int i, int i0, int i1)        { m_view.updateBeamAt(i, i0, i1); }

// Boundary conditions
void   AppControllerAdapter::assignNodeFixedBCGround()                   { m_view.assignNodeFixedBCGround(); }
void   AppControllerAdapter::assignNodePosBCGround()                     { m_view.assignNodePosBCGround(); }
void   AppControllerAdapter::assignNodeFixedBCAt(int i)                  { m_view.assignNodeFixedBCAt(i); }
void   AppControllerAdapter::assignNodePosBCAt(int i)                    { m_view.assignNodePosBCAt(i); }
void   AppControllerAdapter::removeNodeBCAt(int i)                       { m_view.removeNodeBCAt(i); }

// Loads
void   AppControllerAdapter::clearAllLoads()                             { m_view.clearAllLoads(); }
void   AppControllerAdapter::clearAllBCs()                               { m_view.clearAllBCs(); }
void   AppControllerAdapter::addNodeLoadAt(int i, double fx, double fy, double fz)
                                                                         { m_view.addNodeLoadAt(i, fx, fy, fz); }
void   AppControllerAdapter::clearNodeLoadAt(int i)                      { m_view.clearNodeLoadAt(i); }
void   AppControllerAdapter::addBeamLoadAt(int i, double fx, double fy, double fz)
                                                                         { m_view.addBeamLoadAt(i, fx, fy, fz); }
void   AppControllerAdapter::clearBeamLoadAt(int i)                      { m_view.clearBeamLoadAt(i); }

// Geometry modification
void   AppControllerAdapter::arraySelection(int count, double dx, double dy, double dz, bool spanStep,
                                            bool copyLoadsAndBCs, double weldTolerance)
                                                                         { m_view.arraySelection(count, dx, dy, dz, spanStep,
                                                                                                 copyLoadsAndBCs, weldTolerance); }
void   AppControllerAdapter::polarArraySelection(int count, double ax, double ay, double az, double totalAngleDeg,
                                                 int origin, bool rotateCopies, bool fullCircle,
                                                 bool copyLoadsAndBCs, double weldTolerance)
                                                                         { m_view.polarArraySelection(count, ax, ay, az, totalAngleDeg,
                                                                                                      origin, rotateCopies, fullCircle,
                                                                                                      copyLoadsAndBCs, weldTolerance); }

void   AppControllerAdapter::planeArraySelection(int plane, int count1, double step1, int count2, double step2,
                                                 bool spanStep, bool copyLoadsAndBCs, double weldTolerance)
                                                                         { m_view.planeArraySelection(plane, count1, step1,
                                                                                                      count2, step2, spanStep,
                                                                                                      copyLoadsAndBCs, weldTolerance); }

// Mesh generation
void   AppControllerAdapter::meshSelectedNodes()                         { m_view.meshSelectedNodes(); }
void   AppControllerAdapter::surfaceSelectedNodes()                      { m_view.surfaceSelectedNodes(); }

// Queries
size_t AppControllerAdapter::nodeCount()                                 { return m_view.nodeCount(); }
size_t AppControllerAdapter::beamCount()                                 { return m_view.beamCount(); }
void   AppControllerAdapter::nodePosAt(int i, double &x, double &y, double &z)
                                                                         { m_view.nodePosAt(i, x, y, z); }
void   AppControllerAdapter::beamAt(int i, int &i0, int &i1)            { m_view.beamAt(i, i0, i1); }
int    AppControllerAdapter::findNodeNear(double x, double y, double z, double tol)
                                                                         { return m_view.findNodeNear(x, y, z, tol); }
bool   AppControllerAdapter::isNodeFixedAt(int i)                        { return m_view.isNodeFixedAt(i); }
bool   AppControllerAdapter::isNodePosBCAt(int i)                        { return m_view.isNodePosBCAt(i); }
bool   AppControllerAdapter::isNodeSelectedAt(int i)                     { return m_view.isNodeSelectedAt(i); }
bool   AppControllerAdapter::hasNodeLoadAt(int i)                        { return m_view.hasNodeLoadAt(i); }
bool   AppControllerAdapter::hasBeamLoadAt(int i)                        { return m_view.hasBeamLoadAt(i); }
size_t AppControllerAdapter::nodeLoadCount()                             { return m_view.nodeLoadCount(); }
size_t AppControllerAdapter::beamLoadCount()                             { return m_view.beamLoadCount(); }
size_t AppControllerAdapter::materialCount()                             { return m_view.materialCount(); }
void   AppControllerAdapter::modelBounds(double &xmin, double &ymin, double &zmin,
                                         double &xmax, double &ymax, double &zmax)
                                                                         { m_view.modelBounds(xmin, ymin, zmin, xmax, ymax, zmax); }

// Window control
void   AppControllerAdapter::disable()                                   { m_view.disable(); }
void   AppControllerAdapter::enable()                                    { m_view.enable(); }
