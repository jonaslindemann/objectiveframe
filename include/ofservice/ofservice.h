#pragma once

#include <memory>
#include <string>

#include <civetweb/CivetServer.h>

class FemViewWindow;

namespace ofservice {

class APIHandler : public CivetHandler {
private:
    FemViewWindow *m_view;

public:
    APIHandler(FemViewWindow *view);

    FemViewWindow *view();

    std::string read_response(mg_connection *conn);
};

class AddNodesHandler : public APIHandler {

public:
    AddNodesHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AddBeamsHandler : public APIHandler {

public:
    AddBeamsHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class NewModelHandler : public APIHandler {
public:
    NewModelHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class MeshSelectedNodesHandler : public APIHandler {
public:
    MeshSelectedNodesHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SurfaceSelectedNodesHandler : public APIHandler {
public:
    SurfaceSelectedNodesHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SelectAllHandler : public APIHandler {
public:
    SelectAllHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SelectAllNodesHandler : public APIHandler {
public:
    SelectAllNodesHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class ClearSelectionHandler : public APIHandler {
public:
    ClearSelectionHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AssignNodeFixedBCGroundHandler : public APIHandler {
public:
    AssignNodeFixedBCGroundHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AssignNodePosBCGroundHandler : public APIHandler {
public:
    AssignNodePosBCGroundHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AddLastNodeToSelectionHandler : public APIHandler {
public:
    AddLastNodeToSelectionHandler(FemViewWindow *view);
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

/*
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
*/

class Service {
private:
    FemViewWindow *m_view;
    std::shared_ptr<CivetServer> m_webServer;

    AddNodesHandler m_addNodesHandler;
    AddBeamsHandler m_addBeamsHandler;
    NewModelHandler m_newModelHandler;
    MeshSelectedNodesHandler m_meshSelectedNodesHandler;
    SurfaceSelectedNodesHandler m_surfaceSelectedNodesHandler;
    SelectAllHandler m_selectAllHandler;
    SelectAllNodesHandler m_selectAllNodesHandler;
    ClearSelectionHandler m_clearSelectionHandler;
    AssignNodeFixedBCGroundHandler m_assignNodeFixedBCGroundHandler;
    AssignNodePosBCGroundHandler m_assignNodePosBCGroundHandler;
    AddLastNodeToSelectionHandler m_addLastNodeToSelectionHandler;

public:
    Service(FemViewWindow *view);
    virtual ~Service();

    static std::shared_ptr<Service> create(FemViewWindow *view);
};

typedef std::shared_ptr<Service> ServicePtr;

} // namespace ofservice
