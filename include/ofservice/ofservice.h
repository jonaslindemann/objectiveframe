#pragma once

#include <memory>
#include <mutex>
#include <string>

#include <civetweb/CivetServer.h>

class FemViewWindow;

namespace ofservice {

class App {
public:
    static App &instance()
    {
        static App m_instance; // Guaranteed to be destroyed.
                               // Instantiated on first use.
        return m_instance;
    }

private:
    App(){}; // Constructor? (the {} brackets) are needed here.

    FemViewWindow *m_view;
    std::mutex m_mutex;

public:
    App(App const &) = delete;
    void operator=(App const &) = delete;

    void setView(FemViewWindow *view);

    FemViewWindow *view();
};

class APIHandler : public CivetHandler {

public:
    std::string read_response(mg_connection *conn);
};

class AddNodesHandler : public APIHandler {

public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AddBeamsHandler : public APIHandler {

public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class NewModelHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class MeshSelectedNodesHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class SurfaceSelectedNodesHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

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

class AssignNodeFixedBCGroundHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AssignNodePosBCGroundHandler : public APIHandler {
public:
    bool handlePost(CivetServer *server, struct mg_connection *conn);
};

class AddLastNodeToSelectionHandler : public APIHandler {
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
    OpenModelHandler m_openModelHandler;
    SaveModelHandler m_saveModelHandler;
    ExportModelHandler m_exportModelHandler;
    ImportModelHandler m_importModelHandler;    

public:
    Service(FemViewWindow *view);
    virtual ~Service();

    static std::shared_ptr<Service> create(FemViewWindow *view);
};

typedef std::shared_ptr<Service> ServicePtr;

} // namespace ofservice
