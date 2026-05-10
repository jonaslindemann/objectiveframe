#pragma once

#include <memory>
#include <mutex>
#include <string>

#include <civetweb/CivetServer.h>
#include <ofservice/iapp_controller.h>

namespace ofservice {

// ── App singleton ─────────────────────────────────────────────────────────────

class App {
public:
    static App &instance()
    {
        static App m_instance;
        return m_instance;
    }

private:
    App(){};

    IAppController *m_controller{nullptr};
    std::mutex m_mutex;

public:
    App(App const &) = delete;
    void operator=(App const &) = delete;

    void setController(IAppController *controller);
    IAppController *controller();
};

// ── Base handler ──────────────────────────────────────────────────────────────

class APIHandler : public CivetHandler {
public:
    std::string read_response(mg_connection *conn);
};

// ── Model lifecycle ───────────────────────────────────────────────────────────

class NewModelHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class OpenModelHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SaveModelHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ExportModelHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ImportModelHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SnapShotHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

// ── Node / beam creation ──────────────────────────────────────────────────────

class AddNodesHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AddBeamsHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

// ── Selection ─────────────────────────────────────────────────────────────────

class SelectAllHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SelectAllNodesHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ClearSelectionHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AddLastNodeToSelectionHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SelectNodeAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SelectBeamAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

// ── Node / beam mutation ──────────────────────────────────────────────────────

class DeleteNodeAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class DeleteBeamAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SubdivideBeamAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ConnectNearNodesHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class UpdateNodePosAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class UpdateBeamAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

// ── Boundary conditions ───────────────────────────────────────────────────────

class AssignNodeFixedBCGroundHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AssignNodePosBCGroundHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AssignNodeFixedBCAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AssignNodePosBCAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class RemoveNodeBCAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

// ── Loads ─────────────────────────────────────────────────────────────────────

class ClearAllLoadsHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ClearAllBCsHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AddNodeLoadAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ClearNodeLoadAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AddBeamLoadAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ClearBeamLoadAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

// ── Mesh generation ───────────────────────────────────────────────────────────

class MeshSelectedNodesHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SurfaceSelectedNodesHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

// ── Queries ───────────────────────────────────────────────────────────────────

class NodeCountHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class BeamCountHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class NodePosAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class BeamAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class FindNodeNearHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class IsNodeFixedAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class IsNodePosBCAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class IsNodeSelectedAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class HasNodeLoadAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class HasBeamLoadAtHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class NodeLoadCountHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class BeamLoadCountHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class MaterialCountHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ModelBoundsHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

// ── Service ───────────────────────────────────────────────────────────────────

class Service {
private:
    IAppController *m_controller;
    std::shared_ptr<CivetServer> m_webServer;

    // Model lifecycle
    NewModelHandler              m_newModelHandler;
    OpenModelHandler             m_openModelHandler;
    SaveModelHandler             m_saveModelHandler;
    ExportModelHandler           m_exportModelHandler;
    ImportModelHandler           m_importModelHandler;
    SnapShotHandler              m_snapShotHandler;

    // Node / beam creation
    AddNodesHandler              m_addNodesHandler;
    AddBeamsHandler              m_addBeamsHandler;

    // Selection
    SelectAllHandler             m_selectAllHandler;
    SelectAllNodesHandler        m_selectAllNodesHandler;
    ClearSelectionHandler        m_clearSelectionHandler;
    AddLastNodeToSelectionHandler m_addLastNodeToSelectionHandler;
    SelectNodeAtHandler          m_selectNodeAtHandler;
    SelectBeamAtHandler          m_selectBeamAtHandler;

    // Node / beam mutation
    DeleteNodeAtHandler          m_deleteNodeAtHandler;
    DeleteBeamAtHandler          m_deleteBeamAtHandler;
    SubdivideBeamAtHandler       m_subdivideBeamAtHandler;
    ConnectNearNodesHandler      m_connectNearNodesHandler;
    UpdateNodePosAtHandler       m_updateNodePosAtHandler;
    UpdateBeamAtHandler          m_updateBeamAtHandler;

    // Boundary conditions
    AssignNodeFixedBCGroundHandler m_assignNodeFixedBCGroundHandler;
    AssignNodePosBCGroundHandler   m_assignNodePosBCGroundHandler;
    AssignNodeFixedBCAtHandler     m_assignNodeFixedBCAtHandler;
    AssignNodePosBCAtHandler       m_assignNodePosBCAtHandler;
    RemoveNodeBCAtHandler          m_removeNodeBCAtHandler;

    // Loads
    ClearAllLoadsHandler         m_clearAllLoadsHandler;
    ClearAllBCsHandler           m_clearAllBCsHandler;
    AddNodeLoadAtHandler         m_addNodeLoadAtHandler;
    ClearNodeLoadAtHandler       m_clearNodeLoadAtHandler;
    AddBeamLoadAtHandler         m_addBeamLoadAtHandler;
    ClearBeamLoadAtHandler       m_clearBeamLoadAtHandler;

    // Mesh generation
    MeshSelectedNodesHandler     m_meshSelectedNodesHandler;
    SurfaceSelectedNodesHandler  m_surfaceSelectedNodesHandler;

    // Queries
    NodeCountHandler             m_nodeCountHandler;
    BeamCountHandler             m_beamCountHandler;
    NodePosAtHandler             m_nodePosAtHandler;
    BeamAtHandler                m_beamAtHandler;
    FindNodeNearHandler          m_findNodeNearHandler;
    IsNodeFixedAtHandler         m_isNodeFixedAtHandler;
    IsNodePosBCAtHandler         m_isNodePosBCAtHandler;
    IsNodeSelectedAtHandler      m_isNodeSelectedAtHandler;
    HasNodeLoadAtHandler         m_hasNodeLoadAtHandler;
    HasBeamLoadAtHandler         m_hasBeamLoadAtHandler;
    NodeLoadCountHandler         m_nodeLoadCountHandler;
    BeamLoadCountHandler         m_beamLoadCountHandler;
    MaterialCountHandler         m_materialCountHandler;
    ModelBoundsHandler           m_modelBoundsHandler;

public:
    Service(IAppController *controller);
    virtual ~Service();

    static std::shared_ptr<Service> create(IAppController *controller);
};

typedef std::shared_ptr<Service> ServicePtr;

} // namespace ofservice
