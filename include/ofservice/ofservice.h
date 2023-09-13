#pragma once

#include <memory>
#include <string>

#include <civetweb/CivetServer.h>

class FemViewWindow;

namespace ofservice {

class AddNodeHandler : public CivetHandler {
private:
    FemViewWindow *m_view;

public:
    AddNodeHandler(FemViewWindow *view);

	bool handleGet(CivetServer *server, struct mg_connection *conn);    
};

class Service {
private:
    FemViewWindow *m_view;
    std::shared_ptr<CivetServer> m_webServer;

    AddNodeHandler m_addNodeHandler;

public:
    Service(FemViewWindow* view);
    virtual ~Service();

    static std::shared_ptr<Service> create(FemViewWindow* view);
};

typedef std::shared_ptr<Service> ServicePtr;

} // namespace ofui
