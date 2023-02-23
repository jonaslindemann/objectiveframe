#include <ofsolve/tetgen_beam_mesher.h>

#include <logger.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdio>

#include <ofutil/util_functions.h>

using namespace ofem;
using namespace ofsolver;

TetgenNode::TetgenNode(int idx, double x, double y, double z)
    : m_idx { idx }
    , m_pos { x, y, z }
{
}

int TetgenNode::index()
{
    return m_idx;
}

double TetgenNode::x()
{
    return m_pos[0];
}

double TetgenNode::y()
{
    return m_pos[1];
}

double TetgenNode::z()
{
    return m_pos[2];
}

bool ofsolver::TetgenNode::xIsNear(double value, double eps)
{
    return (fabs(m_pos[0] - value) <= eps * std::max(fabs(m_pos[0]), fabs(value)));
}

bool ofsolver::TetgenNode::yIsNear(double value, double eps)
{
    return (fabs(m_pos[1] - value) <= eps * std::max(fabs(m_pos[1]), fabs(value)));
}

bool ofsolver::TetgenNode::zIsNear(double value, double eps)
{
    return (fabs(m_pos[2] - value) <= eps * std::max(fabs(m_pos[2]), fabs(value)));
}

TetgenEdge::TetgenEdge(int idx, int i0, int i1)
    : m_idx { idx }
    , m_i0 { i0 }
    , m_i1 { i1 }
{
}

int TetgenEdge::index()
{
    return m_idx;
}

int TetgenEdge::i0()
{
    return m_i0;
}

int TetgenEdge::i1()
{
    return m_i1;
}

void ofsolver::TetgenEdge::setIndex(int idx)
{
    m_idx = idx;
}

void ofsolver::TetgenEdge::setIndices(int i0, int i1)
{
    m_i0 = i0;
    m_i1 = i1;
}

ofsolver::TetgenFace::TetgenFace(int idx, int i0, int i1, int i2)
    : m_idx { idx }
    , m_i0 { i0 }
    , m_i1 { i1 }
    , m_i2 { i2 }
{
}

int ofsolver::TetgenFace::index()
{
    return m_idx;
}

int ofsolver::TetgenFace::i0()
{
    return m_i0;
}

int ofsolver::TetgenFace::i1()
{
    return m_i1;
}

int ofsolver::TetgenFace::i2()
{
    return m_i2;
}

void ofsolver::TetgenFace::setIndex(int idx)
{
    m_idx = idx;
}

void ofsolver::TetgenFace::setIndices(int i0, int i1, int i2)
{
    m_i0 = i0;
    m_i1 = i1;
    m_i2 = i2;
}

TetgenNodes::TetgenNodes(size_t preAllocate)
{
    m_nodes.reserve(preAllocate);
}

void TetgenNodes::addNode(int idx, double x, double y, double z)
{
    m_nodes.emplace_back(TetgenNode(idx, x, y, z));
    m_nodeIndices.emplace_back(idx);
}

void TetgenNodes::clear()
{
    m_nodes.clear();
    m_nodeIndices.clear();
}

void TetgenNodes::preAllocate(size_t nNodes)
{
    m_nodes.reserve(nNodes);
}

const std::vector<int>& ofsolver::TetgenNodes::nodeIndices()
{
    return m_nodeIndices;
}

size_t ofsolver::TetgenNodes::count()
{
    return m_nodes.size();
}

TetgenNode ofsolver::TetgenNodes::at(int idx)
{
    if ((idx >= 0) && (idx < m_nodes.size()))
    {
        return m_nodes[idx];
    }
    else
        return TetgenNode(-1, 0.0, 0.0, 0.0);
}

void TetgenNodes::save(const std::string filename)
{
    std::ofstream nodeFile;
    nodeFile.open(filename, std::ios::out|std::ios::trunc);

    if (!nodeFile.is_open())
        return;

    nodeFile << m_nodes.size() << " 3 0 0\n";

    auto i = 0;

    for (auto& node : m_nodes)
        nodeFile << i++ << " " << node.x() << " " << node.y() << " " << node.z() << "\n";

    nodeFile.close();
}

TetgenEdges::TetgenEdges()
{

}

void TetgenEdges::clear()
{
    m_edges.clear();
}

size_t TetgenEdges::count()
{
    return m_edges.size();
}

void TetgenEdges::load(const std::string filename)
{
    std::ifstream edgeFile;

    edgeFile.open(filename);

    if (!edgeFile.is_open())
        return;

    int n_edges;
    int n_markers;

    edgeFile >> n_edges >> n_markers;

    int idx, i0, i1, marker;

    m_edges.clear();

    for (auto i=0; i<n_edges; i++)
    {
        edgeFile >> idx >> i0 >> i1 >> marker;
        m_edges.emplace_back(TetgenEdge(idx, i0, i1));
    }

    edgeFile.close();
}

void ofsolver::TetgenEdges::updateIndices(const std::vector<int>& indices)
{
    for (auto& edge : m_edges)
    {
        auto i0 = edge.i0();
        auto i1 = edge.i1();
        edge.setIndices(indices[i0], indices[i1]);
    }
}

TetgenEdge ofsolver::TetgenEdges::at(int idx)
{
    if ((idx >= 0) && (idx < m_edges.size()))
    {
        return m_edges[idx];
    }
    else
        return TetgenEdge(-1, -1, -1);
}

ofsolver::TetgenFaces::TetgenFaces()
{
}

void ofsolver::TetgenFaces::clear()
{
    m_faces.clear();
}

size_t ofsolver::TetgenFaces::count()
{
    return m_faces.size();
}

void ofsolver::TetgenFaces::load(const std::string filename)
{
    std::ifstream faceFile;

    faceFile.open(filename);

    if (!faceFile.is_open())
        return;

    int n_faces;
    int n_markers;

    faceFile >> n_faces >> n_markers;

    int idx, i0, i1, i2, marker;

    m_faces.clear();

    for (auto i = 0; i < n_faces; i++)
    {
        faceFile >> idx >> i0 >> i1 >> i2;
        m_faces.emplace_back(TetgenFace(idx, i0, i1, i2));
    }

    faceFile.close();
}

void ofsolver::TetgenFaces::updateIndices(const std::vector<int>& indices)
{
    for (auto& face : m_faces)
    {
        auto i0 = face.i0();
        auto i1 = face.i1();
        auto i2 = face.i2();
        face.setIndices(indices[i0], indices[i1], indices[i2]);
    }
}

TetgenFace ofsolver::TetgenFaces::at(int idx)
{
    if ((idx >= 0) && (idx < m_faces.size()))
    {
        return m_faces[idx];
    }
    else
        return TetgenFace(-1, -1, -1, -1);
}


TetgenBeamMesher::TetgenBeamMesher(const std::string workDir, size_t preAllocate)
    : m_nodes(preAllocate)
    , m_workDir(workDir)
{

}

ofsolver::TetgenBeamMesher::~TetgenBeamMesher()
{
}

void ofsolver::TetgenBeamMesher::setProgPath(const std::string progPath)
{
    m_progPath = progPath;
}

void ofsolver::TetgenBeamMesher::clear()
{
    m_nodes.clear();
    m_edges.clear();
    m_faces.clear();
}

void ofsolver::TetgenBeamMesher::addNode(int idx, double x, double y, double z)
{
    m_nodes.addNode(idx, x, y, z);
}

size_t ofsolver::TetgenBeamMesher::nodeCount()
{
    return m_nodes.count();
}

void ofsolver::TetgenBeamMesher::generate()
{
    std::string workDir;

    if (!m_workDir.empty())
    {
        std::filesystem::current_path(m_workDir);
        workDir = m_workDir;
    }
    else
    {
        workDir = std::filesystem::temp_directory_path().generic_string();
        std::filesystem::current_path(workDir);
    }

    //std::string nodeFilename = std::tmpnam(nullptr);
    std::string nodeFilename = workDir + "tetmesh.node";

    m_nodes.save(nodeFilename);

    std::string tetgenExec = "\"" + m_progPath + "tetgen.exe\" -e " + nodeFilename;

    Logger::instance()->log(LogLevel::Info, tetgenExec);

#ifdef WIN32
    auto result = ofutil::run_process(tetgenExec);
#else
    auto result = std::system(tetgenExec.c_str());
#endif

    if (result == 0)
    {
        std::string edgeFilename = workDir + "tetmesh.1.edge";

        if (std::filesystem::exists(edgeFilename))
        {
            m_edges.load(edgeFilename);
            m_edges.updateIndices(m_nodes.nodeIndices());
        }
        else
            m_edges.clear();
  
        std::string faceFilename = workDir + "tetmesh.1.face";

        if (std::filesystem::exists(faceFilename))
        {
            m_faces.load(faceFilename);
            m_faces.updateIndices(m_nodes.nodeIndices());
        }
        else
            m_faces.clear();
    }
    else
        m_edges.clear();

}

TetgenNodes& ofsolver::TetgenBeamMesher::nodes()
{
    return m_nodes;
}

TetgenEdges& ofsolver::TetgenBeamMesher::edges()
{
    return m_edges;
}

TetgenFaces& ofsolver::TetgenBeamMesher::faces()
{
    return m_faces;
}
