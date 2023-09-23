#include <ofservice/ofservice.h>

#include <FemView.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <mutex>

#include <json.hpp>
using nljson = nlohmann::json;

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

ofservice::Service::Service(FemViewWindow *view)
    : m_view{view}
{
    mg_init_library(0);

    App::instance().setView(view);

    std::vector<std::string> options = {
        "document_root", ".", "listening_ports", "8081", "enable_keep_alive", "yes", "keep_alive_timeout_ms", "4000", "prespawn_threads", "4"};

    m_webServer = std::make_shared<CivetServer>(options);
    m_webServer->addHandler("/cmds/add_nodes", m_addNodesHandler);
    m_webServer->addHandler("/cmds/add_beams", m_addBeamsHandler);
    m_webServer->addHandler("/cmds/new_model", m_newModelHandler);
    m_webServer->addHandler("/cmds/mesh_selected_nodes", m_meshSelectedNodesHandler);
    m_webServer->addHandler("/cmds/surface_selected_nodes", m_surfaceSelectedNodesHandler);
    m_webServer->addHandler("/cmds/select_all", m_selectAllHandler);
    m_webServer->addHandler("/cmds/select_all_nodes", m_selectAllNodesHandler);
    m_webServer->addHandler("/cmds/clear_selection", m_clearSelectionHandler);
    m_webServer->addHandler("/cmds/assign_node_fixed_bc_ground", m_assignNodeFixedBCGroundHandler);
    m_webServer->addHandler("/cmds/assign_node_pos_bc_ground", m_assignNodePosBCGroundHandler);
    m_webServer->addHandler("/cmds/add_last_node_to_selection", m_addLastNodeToSelectionHandler);
}

ofservice::Service::~Service()
{
    mg_exit_library();
}

std::shared_ptr<ofservice::Service> ofservice::Service::create(FemViewWindow *view)
{
    return std::make_shared<ofservice::Service>(view);
}

bool ofservice::AddNodesHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    std::cout << "handlePost start ... ";
    auto req_info = mg_get_request_info(conn);

    //mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    std::string response = this->read_response(conn);

    //std::cout << "response = " << response << "\n";

    auto j = nljson::parse(response);

    float pos[3] = {0.0f, 0.0f, 0.0f};

    auto responseOK = false;

    for (auto &element : j) {

        //std::cout << element << '\n';

        bool responseOK = false;

        if (element.is_array()) {
            if (element.size() == 3) {
                auto i = 0;

                for (auto &v : element) {
                    if (v.is_number_float())
                        pos[i++] = v;
                }
                responseOK = true;
            }
        }

        if (responseOK)
            App::instance().view()->addNode(pos[0], pos[1], pos[2]);
    }

    std::cout << "completed.\n";
    return true;
}

std::string ofservice::APIHandler::read_response(mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);
    long long rlen;
    long long nlen = 0;
    long long tlen = req_info->content_length;
    char buf[1024];

    std::string response = "";

    while (nlen < tlen) {
        rlen = tlen - nlen;
        if (rlen > sizeof(buf)) {
            rlen = sizeof(buf);
        }
        rlen = mg_read(conn, buf, (size_t)rlen);

        std::string read_string(buf, buf + (size_t)rlen);

        response = response + read_string;

        if (rlen <= 0) {
            break;
        }
    }

    return response;
}

bool ofservice::AddBeamsHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    std::string response = this->read_response(conn);

    std::cout << "response = " << response << "\n";

    auto j = nljson::parse(response);

    int idx[2] = {0, 0};

    auto responseOK = false;

    for (auto &element : j) {

        std::cout << element << '\n';

        bool responseOK = false;

        if (element.is_array()) {
            if (element.size() == 2) {
                auto i = 0;

                for (auto &v : element) {
                    if (v.is_number_integer())
                        idx[i++] = v;
                }
                responseOK = true;
            }
        }

        if (responseOK)
            App::instance().view()->addBeam(idx[0], idx[1]);
    }

    return true;
}

bool ofservice::NewModelHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->newModel();

    return true;
}

bool ofservice::MeshSelectedNodesHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->meshSelectedNodes();

    return true;
}

bool ofservice::SurfaceSelectedNodesHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->surfaceSelectedNodes();

    return true;
}

bool ofservice::SelectAllHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->selectAll();

    return true;
}

bool ofservice::SelectAllNodesHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->selectAllNodes();

    return true;
}

bool ofservice::ClearSelectionHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->clearSelection();

    return true;
}

bool ofservice::AssignNodeFixedBCGroundHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->assignNodeFixedBCGround();

    return true;
}

bool ofservice::AssignNodePosBCGroundHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->assignNodePosBCGround();

    return true;
}

bool ofservice::AddLastNodeToSelectionHandler::handlePost(CivetServer *server, mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);

    // mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
    //                 "text/html\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
                    "text/html\r\n\r\n");

    App::instance().view()->addLastNodeToSelection();

    return true;
}

void ofservice::App::setView(FemViewWindow *view)
{
    m_view = view;
}

FemViewWindow *ofservice::App::view()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    return m_view;
}
