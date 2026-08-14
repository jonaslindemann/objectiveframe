#include <ofservice/ofservice.h>

#include <mutex>
#include <string>

#include <nlohmann/json.hpp>
using nljson = nlohmann::json;

// ── Helpers ───────────────────────────────────────────────────────────────────

static void write_ok(mg_connection *conn)
{
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
}

static void write_json(mg_connection *conn, const nljson &j)
{
    std::string body = j.dump();
    std::string header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
    mg_write(conn, header.c_str(), header.size());
    mg_write(conn, body.c_str(), body.size());
}

std::string ofservice::APIHandler::read_response(mg_connection *conn)
{
    auto req_info = mg_get_request_info(conn);
    long long rlen, nlen = 0, tlen = req_info->content_length;
    char buf[1024];
    std::string response;

    while (nlen < tlen) {
        rlen = tlen - nlen;
        if (rlen > (long long)sizeof(buf))
            rlen = sizeof(buf);
        rlen = mg_read(conn, buf, (size_t)rlen);
        if (rlen <= 0)
            break;
        response += std::string(buf, buf + (size_t)rlen);
        nlen += rlen;
    }
    return response;
}

// ── App singleton ─────────────────────────────────────────────────────────────

void ofservice::App::setController(IAppController *controller)
{
    m_controller = controller;
}

ofservice::IAppController *ofservice::App::controller()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    return m_controller;
}

// ── Service lifecycle ─────────────────────────────────────────────────────────

ofservice::Service::Service(IAppController *controller)
    : m_controller{controller}
{
    mg_init_library(0);
    App::instance().setController(controller);

    std::vector<std::string> options = {
        "document_root", ".", "listening_ports", "8081",
        "enable_keep_alive", "no", "keep_alive_timeout_ms", "4000",
        "prespawn_threads", "4"};

    m_webServer = std::make_shared<CivetServer>(options);

    // Model lifecycle
    m_webServer->addHandler("/cmds/new_model",                    m_newModelHandler);
    m_webServer->addHandler("/cmds/open_model",                   m_openModelHandler);
    m_webServer->addHandler("/cmds/save_model",                   m_saveModelHandler);
    m_webServer->addHandler("/cmds/export_model",                 m_exportModelHandler);
    m_webServer->addHandler("/cmds/import_model",                 m_importModelHandler);
    m_webServer->addHandler("/cmds/snap_shot",                    m_snapShotHandler);

    // Node / beam creation
    m_webServer->addHandler("/cmds/add_nodes",                    m_addNodesHandler);
    m_webServer->addHandler("/cmds/add_beams",                    m_addBeamsHandler);

    // Selection
    m_webServer->addHandler("/cmds/select_all",                   m_selectAllHandler);
    m_webServer->addHandler("/cmds/select_all_nodes",             m_selectAllNodesHandler);
    m_webServer->addHandler("/cmds/clear_selection",              m_clearSelectionHandler);
    m_webServer->addHandler("/cmds/add_last_node_to_selection",   m_addLastNodeToSelectionHandler);
    m_webServer->addHandler("/cmds/select_node_at",               m_selectNodeAtHandler);
    m_webServer->addHandler("/cmds/select_beam_at",               m_selectBeamAtHandler);

    // Node / beam mutation
    m_webServer->addHandler("/cmds/delete_node_at",               m_deleteNodeAtHandler);
    m_webServer->addHandler("/cmds/delete_beam_at",               m_deleteBeamAtHandler);
    m_webServer->addHandler("/cmds/subdivide_beam_at",            m_subdivideBeamAtHandler);
    m_webServer->addHandler("/cmds/connect_near_nodes",           m_connectNearNodesHandler);
    m_webServer->addHandler("/cmds/update_node_pos_at",           m_updateNodePosAtHandler);
    m_webServer->addHandler("/cmds/update_beam_at",               m_updateBeamAtHandler);

    // Boundary conditions
    m_webServer->addHandler("/cmds/assign_node_fixed_bc_ground",  m_assignNodeFixedBCGroundHandler);
    m_webServer->addHandler("/cmds/assign_node_pos_bc_ground",    m_assignNodePosBCGroundHandler);
    m_webServer->addHandler("/cmds/assign_node_fixed_bc_at",      m_assignNodeFixedBCAtHandler);
    m_webServer->addHandler("/cmds/assign_node_pos_bc_at",        m_assignNodePosBCAtHandler);
    m_webServer->addHandler("/cmds/remove_node_bc_at",            m_removeNodeBCAtHandler);

    // Loads
    m_webServer->addHandler("/cmds/clear_all_loads",              m_clearAllLoadsHandler);
    m_webServer->addHandler("/cmds/clear_all_bcs",                m_clearAllBCsHandler);
    m_webServer->addHandler("/cmds/add_node_load_at",             m_addNodeLoadAtHandler);
    m_webServer->addHandler("/cmds/clear_node_load_at",           m_clearNodeLoadAtHandler);
    m_webServer->addHandler("/cmds/add_beam_load_at",             m_addBeamLoadAtHandler);
    m_webServer->addHandler("/cmds/clear_beam_load_at",           m_clearBeamLoadAtHandler);

    // Geometry modification
    m_webServer->addHandler("/cmds/array_selection",               m_arraySelectionHandler);
    m_webServer->addHandler("/cmds/polar_array_selection",         m_polarArraySelectionHandler);
    m_webServer->addHandler("/cmds/plane_array_selection",         m_planeArraySelectionHandler);

    // Mesh generation
    m_webServer->addHandler("/cmds/mesh_selected_nodes",          m_meshSelectedNodesHandler);
    m_webServer->addHandler("/cmds/surface_selected_nodes",       m_surfaceSelectedNodesHandler);

    // Queries
    m_webServer->addHandler("/cmds/node_count",                   m_nodeCountHandler);
    m_webServer->addHandler("/cmds/beam_count",                   m_beamCountHandler);
    m_webServer->addHandler("/cmds/node_pos_at",                  m_nodePosAtHandler);
    m_webServer->addHandler("/cmds/beam_at",                      m_beamAtHandler);
    m_webServer->addHandler("/cmds/find_node_near",               m_findNodeNearHandler);
    m_webServer->addHandler("/cmds/is_node_fixed_at",             m_isNodeFixedAtHandler);
    m_webServer->addHandler("/cmds/is_node_pos_bc_at",            m_isNodePosBCAtHandler);
    m_webServer->addHandler("/cmds/is_node_selected_at",          m_isNodeSelectedAtHandler);
    m_webServer->addHandler("/cmds/has_node_load_at",             m_hasNodeLoadAtHandler);
    m_webServer->addHandler("/cmds/has_beam_load_at",             m_hasBeamLoadAtHandler);
    m_webServer->addHandler("/cmds/node_load_count",              m_nodeLoadCountHandler);
    m_webServer->addHandler("/cmds/beam_load_count",              m_beamLoadCountHandler);
    m_webServer->addHandler("/cmds/material_count",               m_materialCountHandler);
    m_webServer->addHandler("/cmds/model_bounds",                 m_modelBoundsHandler);
}

ofservice::Service::~Service()
{
    mg_exit_library();
}

std::shared_ptr<ofservice::Service> ofservice::Service::create(IAppController *controller)
{
    return std::make_shared<ofservice::Service>(controller);
}

// ── Model lifecycle handlers ──────────────────────────────────────────────────

bool ofservice::NewModelHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto *c = App::instance().controller();
    c->disable();
    c->newModel();
    c->enable();
    return true;
}

bool ofservice::OpenModelHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto *c = App::instance().controller();
    c->disable();
    std::string filename = this->read_response(conn);
    c->enable();
    c->open(filename);
    return true;
}

bool ofservice::SaveModelHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto *c = App::instance().controller();
    c->disable();
    std::string filename = this->read_response(conn);
    c->enable();
    c->save(filename);
    return true;
}

bool ofservice::ExportModelHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto *c = App::instance().controller();
    c->disable();
    std::string filename = this->read_response(conn);
    c->enable();
    c->exportAsCalfem(filename);
    return true;
}

bool ofservice::ImportModelHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto *c = App::instance().controller();
    c->disable();
    std::string filename = this->read_response(conn);
    c->enable();
    c->importAsCalfem(filename);
    return true;
}

bool ofservice::SnapShotHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->snapShot();
    return true;
}

// ── Node / beam creation handlers ─────────────────────────────────────────────

bool ofservice::AddNodesHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    float pos[3] = {0.0f, 0.0f, 0.0f};

    for (auto &element : j) {
        if (element.is_array() && element.size() == 3) {
            int i = 0;
            for (auto &v : element)
                if (v.is_number())
                    pos[i++] = v.get<float>();
            App::instance().controller()->addNode(pos[0], pos[1], pos[2]);
        }
    }
    return true;
}

bool ofservice::AddBeamsHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    int idx[2] = {0, 0};

    for (auto &element : j) {
        if (element.is_array() && element.size() == 2) {
            int i = 0;
            for (auto &v : element)
                if (v.is_number_integer())
                    idx[i++] = v.get<int>();
            App::instance().controller()->addBeam(idx[0], idx[1]);
        }
    }
    return true;
}

// ── Selection handlers ────────────────────────────────────────────────────────

bool ofservice::SelectAllHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->selectAll();
    return true;
}

bool ofservice::SelectAllNodesHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->selectAllNodes();
    return true;
}

bool ofservice::ClearSelectionHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->clearSelection();
    return true;
}

bool ofservice::AddLastNodeToSelectionHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->addLastNodeToSelection();
    return true;
}

bool ofservice::SelectNodeAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->selectNodeAt(j["index"].get<int>());
    return true;
}

bool ofservice::SelectBeamAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->selectBeamAt(j["index"].get<int>());
    return true;
}

// ── Node / beam mutation handlers ─────────────────────────────────────────────

bool ofservice::DeleteNodeAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->deleteNodeAt(j["index"].get<int>());
    return true;
}

bool ofservice::DeleteBeamAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->deleteBeamAt(j["index"].get<int>());
    return true;
}

bool ofservice::SubdivideBeamAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->subdivideBeamAt(j["index"].get<int>());
    return true;
}

bool ofservice::ConnectNearNodesHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->connectNearNodes(j["tolerance"].get<double>());
    return true;
}

bool ofservice::UpdateNodePosAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    int i = j["index"].get<int>();
    auto pos = j["pos"];
    App::instance().controller()->updateNodePosAt(i, pos[0], pos[1], pos[2]);
    return true;
}

bool ofservice::UpdateBeamAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->updateBeamAt(
        j["index"].get<int>(), j["i0"].get<int>(), j["i1"].get<int>());
    return true;
}

// ── Boundary condition handlers ───────────────────────────────────────────────

bool ofservice::AssignNodeFixedBCGroundHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->assignNodeFixedBCGround();
    return true;
}

bool ofservice::AssignNodePosBCGroundHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->assignNodePosBCGround();
    return true;
}

bool ofservice::AssignNodeFixedBCAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->assignNodeFixedBCAt(j["index"].get<int>());
    return true;
}

bool ofservice::AssignNodePosBCAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->assignNodePosBCAt(j["index"].get<int>());
    return true;
}

bool ofservice::RemoveNodeBCAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->removeNodeBCAt(j["index"].get<int>());
    return true;
}

// ── Load handlers ─────────────────────────────────────────────────────────────

bool ofservice::ClearAllLoadsHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->clearAllLoads();
    return true;
}

bool ofservice::ClearAllBCsHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->clearAllBCs();
    return true;
}

bool ofservice::AddNodeLoadAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    int i = j["index"].get<int>();
    auto f = j["force"];
    App::instance().controller()->addNodeLoadAt(i, f[0], f[1], f[2]);
    return true;
}

bool ofservice::ClearNodeLoadAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->clearNodeLoadAt(j["index"].get<int>());
    return true;
}

bool ofservice::AddBeamLoadAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    int i = j["index"].get<int>();
    auto f = j["force"];
    App::instance().controller()->addBeamLoadAt(i, f[0], f[1], f[2]);
    return true;
}

bool ofservice::ClearBeamLoadAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->clearBeamLoadAt(j["index"].get<int>());
    return true;
}

// ── Geometry modification handlers ────────────────────────────────────────────

// Only the count is required. Everything else falls back to the same defaults
// the command itself uses, so {"count": 4} is a complete request: repeat the
// selection along x by its own length, carrying loads and boundary conditions,
// leaving the copies unwelded.

bool ofservice::ArraySelectionHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    auto step = j.value("step", nljson::array({1.0, 0.0, 0.0}));
    App::instance().controller()->arraySelection(
        j["count"].get<int>(), step[0], step[1], step[2],
        j.value("span_step", true), j.value("copy_loads", true), j.value("tolerance", 0.0));
    return true;
}

bool ofservice::PolarArraySelectionHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    auto axis = j.value("axis", nljson::array({0.0, 1.0, 0.0}));
    App::instance().controller()->polarArraySelection(
        j["count"].get<int>(), axis[0], axis[1], axis[2],
        j.value("angle", 360.0), j.value("origin", 0), j.value("rotate_copies", true),
        j.value("full_circle", true), j.value("copy_loads", true), j.value("tolerance", 0.0));
    return true;
}

bool ofservice::PlaneArraySelectionHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    auto j = nljson::parse(this->read_response(conn));
    App::instance().controller()->planeArraySelection(
        j.value("plane", 1), j["count1"].get<int>(), j.value("step1", 1.0),
        j["count2"].get<int>(), j.value("step2", 1.0),
        j.value("span_step", false), j.value("copy_loads", true), j.value("tolerance", 0.0));
    return true;
}

// ── Mesh generation handlers ──────────────────────────────────────────────────

bool ofservice::MeshSelectedNodesHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->meshSelectedNodes();
    return true;
}

bool ofservice::SurfaceSelectedNodesHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_ok(conn);
    App::instance().controller()->surfaceSelectedNodes();
    return true;
}

// ── Query handlers ────────────────────────────────────────────────────────────

bool ofservice::NodeCountHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_json(conn, {{"value", App::instance().controller()->nodeCount()}});
    return true;
}

bool ofservice::BeamCountHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_json(conn, {{"value", App::instance().controller()->beamCount()}});
    return true;
}

bool ofservice::NodePosAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    auto j = nljson::parse(this->read_response(conn));
    double x, y, z;
    App::instance().controller()->nodePosAt(j["index"].get<int>(), x, y, z);
    write_json(conn, {{"pos", {x, y, z}}});
    return true;
}

bool ofservice::BeamAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    auto j = nljson::parse(this->read_response(conn));
    int i0, i1;
    App::instance().controller()->beamAt(j["index"].get<int>(), i0, i1);
    write_json(conn, {{"i0", i0}, {"i1", i1}});
    return true;
}

bool ofservice::FindNodeNearHandler::handlePost(CivetServer *, mg_connection *conn)
{
    auto j = nljson::parse(this->read_response(conn));
    auto pos = j["pos"];
    double tol = j["tolerance"].get<double>();
    int idx = App::instance().controller()->findNodeNear(pos[0], pos[1], pos[2], tol);
    write_json(conn, {{"index", idx}});
    return true;
}

bool ofservice::IsNodeFixedAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    auto j = nljson::parse(this->read_response(conn));
    write_json(conn, {{"value", App::instance().controller()->isNodeFixedAt(j["index"].get<int>())}});
    return true;
}

bool ofservice::IsNodePosBCAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    auto j = nljson::parse(this->read_response(conn));
    write_json(conn, {{"value", App::instance().controller()->isNodePosBCAt(j["index"].get<int>())}});
    return true;
}

bool ofservice::IsNodeSelectedAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    auto j = nljson::parse(this->read_response(conn));
    write_json(conn, {{"value", App::instance().controller()->isNodeSelectedAt(j["index"].get<int>())}});
    return true;
}

bool ofservice::HasNodeLoadAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    auto j = nljson::parse(this->read_response(conn));
    write_json(conn, {{"value", App::instance().controller()->hasNodeLoadAt(j["index"].get<int>())}});
    return true;
}

bool ofservice::HasBeamLoadAtHandler::handlePost(CivetServer *, mg_connection *conn)
{
    auto j = nljson::parse(this->read_response(conn));
    write_json(conn, {{"value", App::instance().controller()->hasBeamLoadAt(j["index"].get<int>())}});
    return true;
}

bool ofservice::NodeLoadCountHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_json(conn, {{"value", App::instance().controller()->nodeLoadCount()}});
    return true;
}

bool ofservice::BeamLoadCountHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_json(conn, {{"value", App::instance().controller()->beamLoadCount()}});
    return true;
}

bool ofservice::MaterialCountHandler::handlePost(CivetServer *, mg_connection *conn)
{
    write_json(conn, {{"value", App::instance().controller()->materialCount()}});
    return true;
}

bool ofservice::ModelBoundsHandler::handlePost(CivetServer *, mg_connection *conn)
{
    double xmin, ymin, zmin, xmax, ymax, zmax;
    App::instance().controller()->modelBounds(xmin, ymin, zmin, xmax, ymax, zmax);
    write_json(conn, {{"min", {xmin, ymin, zmin}}, {"max", {xmax, ymax, zmax}}});
    return true;
}
