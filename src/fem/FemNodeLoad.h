#ifndef _CFemNodeLoad_H_
#define _CFemNodeLoad_H_

#include "FemLoad.h"
#include "FemNode.h"

#include <vector>

namespace std {};
using namespace std;

FemSmartPointer(CFemNodeLoad);

class CFemNodeLoad : public CFemLoad {
private:
    std::vector<CFemNode*> m_nodes;
    std::vector<long> m_nodeIndex;
    double m_direction[3];
public:
    CFemNodeLoad ();
    virtual ~CFemNodeLoad ();

    // Class info

    FemClassInfo("CFemNodeLoad",CFemLoad);

    // Methods

    bool removeNode(CFemNode* node);
    void clearNodes();
    void addNode(CFemNode* node);

    // Get/set methods

    void getDirection(double *v);
    void setDirection(double* v);
    void getDirection(double &ex, double &ey, double &ez);
    void setDirection(double ex, double ey, double ez);
    long getNodeIndex(unsigned int idx);
    CFemNode* getNode(unsigned int idx);
    unsigned int getNodeSize();
    unsigned int getNodeIndexSize();

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
