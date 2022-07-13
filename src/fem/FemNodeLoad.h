#ifndef _CFemNodeLoad_H_
#define _CFemNodeLoad_H_

#include "FemLoad.h"
#include "FemNode.h"

#include <vector>

namespace std {};
using namespace std;

FemSmartPointer(FemNodeLoad);

class FemNodeLoad : public FemLoad {
private:
    std::vector<FemNode*> m_nodes;
    std::vector<long> m_nodeIndex;
    double m_direction[3];
public:
    FemNodeLoad ();
    virtual ~FemNodeLoad ();

    // Class info

    FemClassInfo("FemNodeLoad",FemLoad);

    // Methods

    bool removeNode(FemNode* node);
    void clearNodes();
    void addNode(FemNode* node);

    // Get/set methods

    void getDirection(double *v);
    void setDirection(double* v);
    void getDirection(double &ex, double &ey, double &ez);
    void setDirection(double ex, double ey, double ez);
    long getNodeIndex(unsigned int idx);
    FemNode* getNode(unsigned int idx);
    size_t getNodeSize();
    size_t getNodeIndexSize();

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
