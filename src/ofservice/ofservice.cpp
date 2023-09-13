#include <ofservice/ofservice.h>

#include <FemView.h>

#include <iostream>
#include <iomanip>
#include <sstream>

std::string float2str(double value)
{
    std::stringstream coordStream;
    coordStream << std::fixed << std::setw(10) << std::setprecision(2) << value;
    return coordStream.str();
}

float str2float(std::string str)
{
    std::stringstream ss(str);
    float value = 0.0;

    ss >> value;
    
    return value;
}

ofservice::Service::Service(FemViewWindow *view) : m_view{view}, m_addNodeHandler{view}
{
    mg_init_library(0);

    std::vector<std::string> options = {"document_root", ".", "listening_ports", "8081"};

    m_webServer = std::make_shared<CivetServer>(options);
    m_webServer->addHandler("/cmds/addnode", m_addNodeHandler);
}

ofservice::Service::~Service()
{
    mg_exit_library();
}

std::shared_ptr<ofservice::Service> ofservice::Service::create(FemViewWindow *view)
{
    return std::make_shared<ofservice::Service>(view);
}

ofservice::AddNodeHandler::AddNodeHandler(FemViewWindow *view) : m_view{view}
{
}

bool ofservice::AddNodeHandler::handleGet(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);
    std::string s;
    float x, y, z;

    bool paramsOK = true;

    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\nConnection: close\r\n\r\n");

    mg_printf(conn, "<html><body>");

    if (CivetServer::getParam(conn, "x", s))
        x = str2float(s);
    else
        paramsOK = false;
 
    if (CivetServer::getParam(conn, "y", s))
        y = str2float(s);
    else
        paramsOK = false;

    if (CivetServer::getParam(conn, "z", s))
        z = str2float(s);
    else
        paramsOK = false;

    if (paramsOK) {
        m_view->addNode(x, y, z);
        mg_printf(conn, "Node added at (%f, %f, %f)\n", x, y, z);
    }
    else
        mg_printf(conn, "Invalid parameters.");

    mg_printf(conn, "</body></html>\n");
    return true;

    return true;
}
