#pragma once

#include <ofem/beam_model.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>
#include <limits>

#include <Eigen/Dense>

namespace ofsolver
{

class TetgenNode 
{
private:
    int m_idx;
    double m_pos[3];

public:
    TetgenNode(int idx, double x, double y, double z);
    int index();
    double x();
    double y();
    double z();

    bool xIsNear(double value, double eps = 1e-6);
    bool yIsNear(double value, double eps = 1e-6);
    bool zIsNear(double value, double eps = 1e-6);
};

class TetgenEdge
{
private:
    int m_idx;
    int m_i0;
    int m_i1;

public:
    TetgenEdge(int idx, int i0, int i1);
    int index();
    int i0();
    int i1();

    void setIndex(int idx);
    void setIndices(int i0, int i1);
};

class TetgenFace
{
private:
    int m_idx;
    int m_i0;
    int m_i1;
    int m_i2;

public:
    TetgenFace(int idx, int i0, int i1, int i2);
    int index();
    int i0();
    int i1();
    int i2();

    void setIndex(int idx);
    void setIndices(int i0, int i1, int i2);
};


class TetgenNodes
{
private:
    std::vector<TetgenNode> m_nodes;
    std::vector<int> m_nodeIndices;

public:
    TetgenNodes(size_t preAllocate=0);

    void addNode(int idx, double x, double y, double z);
    void clear();
    void preAllocate(size_t nNodes);
    const std::vector<int>& nodeIndices();

    size_t count();

    TetgenNode at(int idx);

    void save(const std::string filename);
};

class TetgenEdges
{
private:
    std::vector<TetgenEdge> m_edges;

public:
    TetgenEdges();

    void clear();
    size_t count();

    void load(const std::string filename);

    void updateIndices(const std::vector<int>& indices);

    TetgenEdge at(int idx);
};

class TetgenFaces
{
private:
    std::vector<TetgenFace> m_faces;

public:
    TetgenFaces();

    void clear();
    size_t count();

    void load(const std::string filename);

    void updateIndices(const std::vector<int>& indices);

    TetgenFace at(int idx);
};


class TetgenBeamMesher
{
private:
    TetgenNodes m_nodes;
    TetgenEdges m_edges;
    TetgenFaces m_faces;

    std::string m_workDir;
    std::string m_progPath;

public:
    TetgenBeamMesher(const std::string workDir = "", size_t preAllocate = 0);

    virtual ~TetgenBeamMesher();

    static std::shared_ptr<TetgenBeamMesher> create(const std::string workDir = "", size_t preAllocate = 0)
    {
        return std::make_shared<TetgenBeamMesher>(workDir, preAllocate);
    }

    void setProgPath(const std::string progPath);
    void clear();
    void addNode(int idx, double x, double y, double z);
    size_t nodeCount();
    void generate();

    TetgenNodes& nodes();
    TetgenEdges& edges();
    TetgenFaces& faces();
};

typedef std::shared_ptr<TetgenBeamMesher> TetgenBeamMesherPtr;

}
