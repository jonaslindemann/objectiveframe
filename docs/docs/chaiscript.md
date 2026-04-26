# ChaiScript scripting

ObjectiveFrame exposes model creation and editing through ChaiScript. Scripts can create nodes and beams, query the current model, assign boundary conditions and loads, and automate repetitive modelling operations.

The AI / LLM integration also uses this scripting interface: the LLM generates ChaiScript, and ObjectiveFrame evaluates the result.

## Running scripts

Scripts use the `.chai` extension. You can work with them from the main menu:

- **File/New script...** opens a new script in the script editor.
- **File/Open script...** opens an existing script.
- **File/Run script...** runs a script file directly.

Scripts and plugins are evaluated inside ObjectiveFrame. A snapshot is taken before execution so normal undo workflows can be used after many script operations.

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
var rows = %%rows,4,int%%;
var cols = %%cols,4,int%%;
var dx = %%dx,1.0,float%%;
```

Supported parameter types are `int`, `float`, and `string`.

## Practical advice

Use `newModel()` only when the script should clear the current structure. For scripts that modify an existing model, query the current state with `nodeCount()`, `beamCount()`, and `modelBounds()` first.

When generating regular geometry, store node indices in arrays or maps so beams connect to known nodes. When modifying existing geometry, use `findNodeNear()` with a suitable tolerance instead of assuming node indices are unchanged.
