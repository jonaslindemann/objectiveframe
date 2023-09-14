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

class Service {
private:
    FemViewWindow *m_view;
    std::shared_ptr<CivetServer> m_webServer;

    AddNodesHandler m_addNodesHandler;
    AddBeamsHandler m_addBeamsHandler;

public:
    Service(FemViewWindow *view);
    virtual ~Service();

    static std::shared_ptr<Service> create(FemViewWindow *view);
};

typedef std::shared_ptr<Service> ServicePtr;

} // namespace ofservice
