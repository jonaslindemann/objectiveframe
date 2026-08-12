#include "FemViewScriptBindings.h"

#include "FemView.h"

#include <chaiscript/extras/math.hpp>

void FemViewScriptBindings::bind(chaiscript::ChaiScript &script, FemViewWindow &view)
{
    auto mathlib = chaiscript::extras::math::bootstrap();
    script.add(mathlib);

    script.add(chaiscript::fun(&FemViewWindow::addNode, &view), "addNode");
    script.add(chaiscript::fun(&FemViewWindow::newModel, &view), "newModel");
    script.add(chaiscript::fun(&FemViewWindow::addBeam, &view), "addBeam");
    script.add(chaiscript::fun(&FemViewWindow::nodeCount, &view), "nodeCount");
    script.add(chaiscript::fun(&FemViewWindow::meshSelectedNodes, &view), "meshSelectedNodes");
    script.add(chaiscript::fun(&FemViewWindow::surfaceSelectedNodes, &view), "surfaceSelectedNodes");
    script.add(chaiscript::fun(&FemViewWindow::selectAll, &view), "selectAll");
    script.add(chaiscript::fun(&FemViewWindow::selectAllNodes, &view), "selectAllNodes");
    script.add(chaiscript::fun(&FemViewWindow::clearSelection, &view), "clearSelection");
    script.add(chaiscript::fun(&FemViewWindow::assignNodeFixedBCGround, &view), "assignNodeFixedBCGround");
    script.add(chaiscript::fun(&FemViewWindow::assignNodePosBCGround, &view), "assignNodePosBCGround");
    script.add(chaiscript::fun(&FemViewWindow::addLastNodeToSelection, &view), "addLastNodeToSelection");
    script.add(chaiscript::fun(&FemViewWindow::nodePos, &view), "nodeCoord");
    script.add(chaiscript::fun(&FemViewWindow::updateNodePos, &view), "updateNodePos");
    script.add(chaiscript::fun(&FemViewWindow::nodePosAt, &view), "nodePosAt");
    script.add(chaiscript::fun(&FemViewWindow::updateNodePosAt, &view), "updateNodePosAt");
    script.add(chaiscript::fun(&FemViewWindow::nodeAt, &view), "nodeAt");
    script.add(chaiscript::fun(&FemViewWindow::isNodeSelected, &view), "isNodeSelected");
    script.add(chaiscript::fun(&FemViewWindow::isNodeSelectedAt, &view), "isNodeSelectedAt");
    script.add(chaiscript::fun(&FemViewWindow::randFloat, &view), "randFloat");
    script.add(chaiscript::fun(&FemViewWindow::randInt, &view), "randInt");
    script.add(chaiscript::fun(&FemViewWindow::randSeed, &view), "randSeed");
    script.add(chaiscript::fun(&FemViewWindow::addNodeWithIdx, &view), "addNodeWithIdx");
    script.add(chaiscript::fun(&FemViewWindow::addBeamWithIdx, &view), "addBeamWithIdx");
    script.add(chaiscript::fun(&FemViewWindow::beamCount, &view), "beamCount");
    script.add(chaiscript::fun(&FemViewWindow::beamAt, &view), "beamAt");
    script.add(chaiscript::fun(&FemViewWindow::updateBeamAt, &view), "updateBeamAt");

    script.add(chaiscript::fun(&FemViewWindow::selectNodeAt, &view), "selectNodeAt");
    script.add(chaiscript::fun(&FemViewWindow::selectBeamAt, &view), "selectBeamAt");
    script.add(chaiscript::fun(&FemViewWindow::findNodeNear, &view), "findNodeNear");

    script.add(chaiscript::fun(&FemViewWindow::deleteNodeAt, &view), "deleteNodeAt");
    script.add(chaiscript::fun(&FemViewWindow::deleteBeamAt, &view), "deleteBeamAt");
    script.add(chaiscript::fun(&FemViewWindow::subdivideBeamAt, &view), "subdivideBeamAt");
    script.add(chaiscript::fun(&FemViewWindow::beamsAtNode, &view), "beamsAtNode");
    script.add(chaiscript::fun(&FemViewWindow::connectNearNodes, &view), "connectNearNodes");
    script.add(chaiscript::fun(&FemViewWindow::clearAllLoads, &view), "clearAllLoads");
    script.add(chaiscript::fun(&FemViewWindow::clearAllBCs, &view), "clearAllBCs");
    script.add(chaiscript::fun(&FemViewWindow::snapShot, &view), "snapShot");

    script.add(chaiscript::fun(&FemViewWindow::assignNodeFixedBCAt, &view), "assignNodeFixedBCAt");
    script.add(chaiscript::fun(&FemViewWindow::assignNodePosBCAt, &view), "assignNodePosBCAt");
    script.add(chaiscript::fun(&FemViewWindow::removeNodeBCAt, &view), "removeNodeBCAt");
    script.add(chaiscript::fun(&FemViewWindow::isNodeFixedAt, &view), "isNodeFixedAt");
    script.add(chaiscript::fun(&FemViewWindow::isNodePosBCAt, &view), "isNodePosBCAt");

    script.add(chaiscript::fun(&FemViewWindow::modelBounds, &view), "modelBounds");
    script.add(chaiscript::fun(&FemViewWindow::materialCount, &view), "materialCount");

    script.add(chaiscript::fun(&FemViewWindow::addNodeLoadAt, &view), "addNodeLoadAt");
    script.add(chaiscript::fun(&FemViewWindow::clearNodeLoadAt, &view), "clearNodeLoadAt");
    script.add(chaiscript::fun(&FemViewWindow::hasNodeLoadAt, &view), "hasNodeLoadAt");
    script.add(chaiscript::fun(&FemViewWindow::nodeLoadCount, &view), "nodeLoadCount");

    // Geometry modification. The origin argument is 0 world, 1 centroid,
    // 2 bounding box centre, 3 cursor.

    script.add(chaiscript::fun(&FemViewWindow::translateSelection, &view), "translateSelection");
    script.add(chaiscript::fun(&FemViewWindow::scaleSelection, &view), "scaleSelection");
    script.add(chaiscript::fun(&FemViewWindow::rotateSelection, &view), "rotateSelection");
    script.add(chaiscript::fun(&FemViewWindow::mirrorSelection, &view), "mirrorSelection");
    script.add(chaiscript::fun(&FemViewWindow::taperSelection, &view), "taperSelection");
    script.add(chaiscript::fun(&FemViewWindow::smoothSelection, &view), "smoothSelection");

    script.add(chaiscript::fun(&FemViewWindow::addBeamLoadAt, &view), "addBeamLoadAt");
    script.add(chaiscript::fun(&FemViewWindow::clearBeamLoadAt, &view), "clearBeamLoadAt");
    script.add(chaiscript::fun(&FemViewWindow::hasBeamLoadAt, &view), "hasBeamLoadAt");
    script.add(chaiscript::fun(&FemViewWindow::beamLoadCount, &view), "beamLoadCount");
}
