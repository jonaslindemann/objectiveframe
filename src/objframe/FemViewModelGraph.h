#pragma once

#include <map>
#include <set>
#include <vector>

#include <ofem/beam.h>
#include <ofem/node.h>

namespace ivf {
class Shape;
}

class FemViewWindow;

/**
 * The beam model seen as a plain graph, shared by the command handlers that
 * need to reason about topology rather than about screen geometry.
 *
 * Built fresh for each command - the models are small enough that this is
 * cheaper than keeping an index in sync.
 */
namespace ofview_detail {

struct ModelGraph {
    std::vector<ofem::Node *> nodes;
    std::vector<ofem::Beam *> beams;

    // Beams meeting at a node
    std::map<ofem::Node *, std::vector<ofem::Beam *>> beamsAt;

    // The scene shape representing each model object
    std::map<ofem::Node *, ivf::Shape *> nodeShape;
    std::map<ofem::Beam *, ivf::Shape *> beamShape;

    // The current selection, expressed in model terms
    std::set<ofem::Node *> selectedNodes;
    std::set<ofem::Beam *> selectedBeams;

    bool hasSelection() const
    {
        return !selectedNodes.empty() || !selectedBeams.empty();
    }
};

/** Returns the node at the far end of a beam, or nullptr if node is not on it. */
ofem::Beam *otherEnd(ofem::Beam *beam, ofem::Node *node, ofem::Node *&other);

void nodeCoord(ofem::Node *node, double v[3]);

/** Unit vector from one node to the other. Returns false for a zero length beam. */
bool beamDirection(ofem::Beam *beam, double dir[3]);

/** Builds the graph and reads the current selection out of the scene. */
bool buildGraph(FemViewWindow &view, ModelGraph &graph);

/** Pushes a model level selection back into the view. */
void applySelection(FemViewWindow &view, ModelGraph &graph, const std::set<ofem::Node *> &nodes,
                    const std::set<ofem::Beam *> &beams);

} // namespace ofview_detail
