# ChaiScript scripting

ObjectiveFrame exposes model creation and editing through ChaiScript. Scripts can create nodes and beams, query the current model, assign boundary conditions and loads, and automate repetitive modelling operations.

The AI / LLM integration also uses this scripting interface: the LLM generates ChaiScript, and ObjectiveFrame evaluates the result.

## Running scripts

Scripts use the `.chai` extension. You can work with them from the main menu:

- **File/New script...** opens a new script in the script editor.
- **File/Open script...** opens an existing script.
- **File/Run script...** runs a script file directly.
- **View/Script editor...** opens the script editor on whatever is currently loaded.

Scripts and plugins are evaluated inside ObjectiveFrame. A snapshot is taken before execution so normal undo workflows can be used after many script operations.

The same commands are also available over HTTP, for driving ObjectiveFrame from Python or another program. See [Automation API](rest-api.md).

## Minimal example

```chaiscript
newModel();

var n0 = addNodeWithIdx(0.0, 0.0, 0.0);
var n1 = addNodeWithIdx(4.0, 0.0, 0.0);
var n2 = addNodeWithIdx(2.0, 2.0, 0.0);

addBeamWithIdx(n0, n1);
addBeamWithIdx(n0, n2);
addBeamWithIdx(n1, n2);

assignNodeFixedBCAt(n0);
assignNodeFixedBCAt(n1);
addNodeLoadAt(n2, 0.0, -1000.0, 0.0);
```

## Model creation

| Function | Description |
| --- | --- |
| `newModel()` | Clear the current model and create an empty one. |
| `addNode(x, y, z)` | Add a node and return the visual node object. |
| `addNodeWithIdx(x, y, z)` | Add a node and return its index. |
| `addBeam(i0, i1)` | Add a beam between node indices `i0` and `i1`. |
| `addBeamWithIdx(i0, i1)` | Add a beam and return its index. |
| `meshSelectedNodes()` | Create beam elements from selected nodes using the mesh workflow. |
| `surfaceSelectedNodes()` | Create surface-style connections from selected nodes. |

Prefer the `WithIdx` functions for generated scripts because they make node and beam references explicit.

## Model query and editing

| Function | Description |
| --- | --- |
| `nodeCount()` | Return the number of nodes. |
| `beamCount()` | Return the number of beams. |
| `nodePosAt(i, x, y, z)` | Fill `x`, `y`, and `z` with the position of node `i`. |
| `updateNodePosAt(i, x, y, z)` | Move node `i`. |
| `beamAt(i, i0, i1)` | Fill `i0` and `i1` with the node indices of beam `i`. |
| `updateBeamAt(i, i0, i1)` | Reconnect beam `i`. |
| `findNodeNear(x, y, z, tolerance)` | Return the nearest node index within `tolerance`, or `-1`. |
| `modelBounds(xmin, ymin, zmin, xmax, ymax, zmax)` | Fill variables with the model bounding box. |
| `materialCount()` | Return the number of materials. |
| `deleteNodeAt(i)` | Delete a node and connected beams. |
| `deleteBeamAt(i)` | Delete a beam. |
| `subdivideBeamAt(i)` | Split beam `i` in two, adding a node at its midpoint. |
| `beamsAtNode(i)` | Return the indices of the beams meeting at node `i`. |
| `connectNearNodes(tolerance)` | Weld nodes closer together than `tolerance` into one. |
| `clearAllLoads()` | Remove every load from the model. |
| `clearAllBCs()` | Remove every boundary condition from the model. |
| `snapShot()` | Place an undo point. |

A few functions take the visual node object that `addNode()` returns rather than an index. They are convenient when a script already holds the node it just created:

| Function | Description |
| --- | --- |
| `nodeAt(i)` | Return the node object for index `i`. |
| `nodeCoord(node, x, y, z)` | Fill `x`, `y`, and `z` with the position of `node`. |
| `updateNodePos(node, x, y, z)` | Move `node`. |
| `isNodeSelected(node)` | Return whether `node` is selected. |
| `addLastNodeToSelection()` | Add the most recently created node to the selection. |

Out-parameters must be declared before calling functions such as `nodePosAt`, `beamAt`, and `modelBounds`:

```chaiscript
var x = 0.0;
var y = 0.0;
var z = 0.0;
nodePosAt(0, x, y, z);
```

## Selection

| Function | Description |
| --- | --- |
| `selectAll()` | Select all nodes and beams. |
| `selectAllNodes()` | Select all nodes. |
| `selectNodeAt(i)` | Add node `i` to the selection. |
| `selectBeamAt(i)` | Add beam `i` to the selection. |
| `clearSelection()` | Clear the current selection. |
| `isNodeSelectedAt(i)` | Return whether node `i` is selected. |

## Geometry commands

These commands act on the **current selection**, so select something first - with nothing selected they do nothing and say so on the console. The affected set is the selected nodes together with the end nodes of any selected beam.

Each command is a single undoable step and takes its own snapshot, so do not call `snapShot()` before one. Prefer them over hand-written coordinate loops: node and beam indices survive, materials and cross section rotation are kept, and degenerate arguments are refused instead of quietly destroying geometry.

| Function | Description |
| --- | --- |
| `translateSelection(dx, dy, dz)` | Move the selection. |
| `scaleSelection(sx, sy, sz, origin)` | Scale about `origin`. |
| `rotateSelection(ax, ay, az, angleDeg, origin)` | Rotate about the axis `(ax, ay, az)` through `origin`. |
| `taperSelection(axis, s0, s1, origin)` | Scale perpendicular to `axis` by a factor running from `s0` at the low end to `s1` at the high end. |
| `smoothSelection(iterations, lambda, mu, lengthWeighted, pinBC, pinLoaded)` | Taubin smoothing of the selected nodes. |
| `mirrorSelection(axis, origin, weldTolerance)` | Reflect in a principal plane and keep both halves. |
| `arraySelection(count, dx, dy, dz, spanStep, copyLoadsAndBCs, weldTolerance)` | Repeat along a direction. |
| `planeArraySelection(plane, count1, step1, count2, step2, spanStep, copyLoadsAndBCs, weldTolerance)` | Repeat across a principal plane. |
| `polarArraySelection(count, ax, ay, az, totalAngleDeg, origin, rotateCopies, fullCircle, copyLoadsAndBCs, weldTolerance)` | Repeat around an axis. |
| `setSelectionCoord(setX, x, setY, y, setZ, z)` | Assign world coordinates to the selection, one axis at a time. |
| `setSelectionCoordAxis(axis, value)` | The same for a single axis - `0` for x, `1` for y, `2` for z. |

`setSelectionCoord` writes only the axes whose flag is true and leaves the others as they are, so `setSelectionCoord(false, 0.0, true, 0.0, false, 0.0)` flattens the selection onto `y = 0` while its x and z stay spread.

`axis` is `0` for x, `1` for y and `2` for z. `origin` selects what the transform is measured from:

| Value | Origin |
| --- | --- |
| `0` | World origin |
| `1` | Centroid of the affected nodes |
| `2` | Centre of their bounding box |
| `3` | Last picked position |
| `4` | Low face of their bounding box |
| `5` | High face |

Mirror needs `0`, `4` or `5`: a plane through the middle of the selection reflects it onto itself, and the weld then removes the copy. A polar array wants `0` or `3` for the same reason - an axis through the selection's own centroid spins the copies on top of it.

### Arrays

Counts include the original, so `arraySelection(2, ...)` gives one copy and a 4 x 3 grid is 12 instances. Steps are per copy rather than the total span. With `spanStep` set, a step is measured in bounding box lengths of the selection, so `1.0` puts each copy exactly one selection length further on. `weldTolerance` fuses coincident nodes at the end of the command; pass `0` to leave the copies detached.

```chaiscript
// Extend one bay into a four bay truss, joined at the seams
selectAll();
arraySelection(4, 1.0, 0.0, 0.0, true, true, 0.001);

// A 4 x 3 grid of frames in the XZ plane, 5 m apart along x and 6 m along z
selectAll();
planeArraySelection(1, 4, 5.0, 3, 6.0, false, true, 0.001);

// Six ribs evenly around the y axis through the world origin
selectAll();
polarArraySelection(6, 0.0, 1.0, 0.0, 360.0, 0, true, true, true, 0.001);
```

For `planeArraySelection`, `plane` is `0` for xy, `1` for xz and `2` for yz. `count1` and `step1` run along the first named axis of the plane and `count2` and `step2` along the second, so an xz grid is x repeat, x step, z repeat, z step. Prefer it over two chained `arraySelection` calls: it produces the same geometry in one undo step and welds once.

For `polarArraySelection`, `fullCircle` divides the sweep by the count so the last copy stops one step short of the original; setting it to `false` divides by `count - 1` instead, placing the first and last instance on the ends of the arc. `rotateCopies` set to `false` slides the copies along the arc without turning them.

Linear and grid arrays carry loads and boundary conditions onto the copies, because a translation preserves every direction. A rotating polar array carries distributed beam loads, which are defined in the member's own directions, but skips nodal loads and partial supports, which are defined globally, and reports how many were left off.

## Boundary conditions and loads

| Function | Description |
| --- | --- |
| `assignNodeFixedBCGround()` | Fully fix all ground nodes. |
| `assignNodePosBCGround()` | Position-fix all ground nodes. |
| `assignNodeFixedBCAt(i)` | Apply the default fully fixed boundary condition to node `i`. |
| `assignNodePosBCAt(i)` | Apply the default position-only boundary condition to node `i`. |
| `removeNodeBCAt(i)` | Remove editable boundary conditions from node `i`. |
| `isNodeFixedAt(i)` | Return whether node `i` has the default fixed condition. |
| `isNodePosBCAt(i)` | Return whether node `i` has the default position-only condition. |
| `addNodeLoadAt(i, fx, fy, fz)` | Add a nodal load vector to node `i`. |
| `clearNodeLoadAt(i)` | Remove node loads from node `i`. |
| `hasNodeLoadAt(i)` | Return whether node `i` has a nodal load. |
| `nodeLoadCount()` | Return the number of node load objects. |
| `addBeamLoadAt(i, fx, fy, fz)` | Add a distributed beam load vector to beam `i`. |
| `clearBeamLoadAt(i)` | Remove beam loads from beam `i`. |
| `hasBeamLoadAt(i)` | Return whether beam `i` has a beam load. |
| `beamLoadCount()` | Return the number of beam load objects. |

Load vectors encode both direction and magnitude. For example, `addNodeLoadAt(i, 0.0, -1000.0, 0.0)` applies a 1000 N downward load.

## Quick forces and supports

These are the scripting side of the quick tools. They act on the **current selection**, and share the grouping the tools use: supports are shared objects looked up by name, and forces join an existing load with the same direction and magnitude instead of creating one per node.

| Function | Description |
| --- | --- |
| `quickForceSelection(fx, fy, fz)` | Apply a force to the selected nodes. |
| `quickConstraintSelection(kind)` | Apply a standard support to the selected nodes. |
| `clearQuickForceSelection()` | Remove quick forces from the selected nodes. |
| `clearQuickConstraintSelection()` | Remove supports from the selected nodes. |
| `setQuickForce(fx, fy, fz)` | Set what the quick force tool will apply next. |
| `setQuickConstraint(kind)` | Set which support the quick support tool will apply next. |

`kind` names one of the five standard supports:

| Value | Support |
| --- | --- |
| `0` | Fixed - all translations and rotations |
| `1` | Pinned - all translations, free to rotate |
| `2` | Roller X - free along x |
| `3` | Roller Y - free along y |
| `4` | Roller Z - free along z |

A node carries one support at a time, so applying one replaces whatever the node had.

## Self-weight

Self-weight is a property of the model, saved with it, and applied when the model is solved.

| Function | Description |
| --- | --- |
| `setSelfWeightEnabled(flag)` / `selfWeightEnabled()` | Turn self-weight on or off. |
| `setSelfWeightMode(mode)` / `selfWeightMode()` | `0` material density, `1` total load, `2` mass per length. |
| `setGravity(g)` / `gravity()` | Gravitational acceleration, default 9.81. |
| `setGravityScale(s)` / `gravityScale()` | Load factor applied to the result. |
| `setTotalWeight(w)` / `totalWeight()` | Total load, spread over the structure by element length. |
| `setMassPerLength(m)` / `massPerLength()` | Mass per unit length on every element. |

```chaiscript
setSelfWeightEnabled(true);
setSelfWeightMode(0);   // from each material's density
setGravityScale(1.35);  // a factored self-weight case
```

## Work plane

A script that places nodes at known coordinates has no use for the cursor, but a script that sets up a modelling session for someone else does.

| Function | Description |
| --- | --- |
| `lockWorkPlaneAt(plane, x, y, z)` | Pin cursor placement to `plane` through the given point. |
| `releaseWorkPlaneLock()` | Return to `[Shift]` driven plane selection. |
| `isWorkPlaneLocked()` | Return whether a lock is in force. |

`plane` is `0` for xz (the horizontal ground plane), `1` for xy and `2` for yz.

## Utilities

The ChaiScript math extension is available, including functions such as `sin`, `cos`, `sqrt`, and `pow`.

ObjectiveFrame also exposes:

| Function | Description |
| --- | --- |
| `randFloat(min, max)` | Return a random floating point value. |
| `randInt(min, max)` | Return a random integer value. |
| `randSeed()` | Seed the random generator. |

## Plugin parameters

Scripts in the plugin folder can declare editable parameters using `%%name,default,type%%` tags. ObjectiveFrame replaces each tag with the current parameter value before running the plugin.

```chaiscript
var pluginName = "%%pluginName,Node grid,string%%";
var pluginCategory = "%%pluginCategory,Grids,string%%";
var rows = %%rows,4,int%%;
var cols = %%cols,4,int%%;
var dx = %%dx,1.0,float%%;
```

Supported parameter types are `int`, `float`, and `string`.

`pluginName` and `pluginCategory` describe the plugin itself and are not shown as editable fields in the parameter panel. `pluginCategory` is optional: plugins that declare it are grouped into a submenu of that name under **Create**, and plugins without it are listed directly in the **Create** menu above the submenus.

## Practical advice

Use `newModel()` only when the script should clear the current structure. For scripts that modify an existing model, query the current state with `nodeCount()`, `beamCount()`, and `modelBounds()` first.

When generating regular geometry, store node indices in arrays or maps so beams connect to known nodes. When modifying existing geometry, use `findNodeNear()` with a suitable tolerance instead of assuming node indices are unchanged.
