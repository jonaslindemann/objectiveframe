# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ObjectiveFrame is a C++20 finite element beam analysis application with real-time 3D visualization, an embedded scripting engine, a REST API, and an LLM-based structure generator. It is developed at the Division of Structural Mechanics, Lund University.

## Build Commands

ObjectiveFrame uses CMake + vcpkg. vcpkg is auto-detected; set `VCPKG_ROOT` if not found automatically.

**Critical external dependency:** IVF++ must be cloned and built at `../ivfplusplus` (sibling of this repo). Set `IVF_ROOT` env var to override.

```powershell
# Configure (Visual Studio multi-config, recommended on Windows)
cmake -S . -B build -G"Visual Studio 17 2022"

# Build
cmake --build build --config Release
cmake --build build --config Debug

# Single-config (Ninja/Make)
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release && cmake --build build-release
cmake -S . -B build-debug  -DCMAKE_BUILD_TYPE=Debug   && cmake --build build-debug

# Optional Python build wrapper
python build-of.py --build
python build-of.py --build --build-type Debug
```

**Output binaries (Windows):**
- `bin/Release/objframe.exe`
- `bin/Debug/objframed.exe`

There are no automated tests. Build and manually run the application to verify changes.

**Code formatting:** `.clang-format` is present (Microsoft-based style, 4-space indent). Run `clang-format` on changed files before committing.

## Architecture

### Layer stack (bottom to top)

```
GLFWWindow          — raw GLFW window, OpenGL context, input dispatch
  └─ IvfViewWindow  — ivf++ scene graph, camera, editing widget modes (Select/Move/Create/…)
       └─ FemViewWindow — main application: FEM logic, all ImGui UI, scripting, AI
```

`FemViewWindow` is the central class ([src/objframe/FemView.h](src/objframe/FemView.h)). It owns the beam model, all UI windows, the solver, the script engine, and the AI integration.

#### FemViewWindow modularization conventions

`FemViewWindow` is deliberately split across several files to keep the class manageable:

| File | Role |
| ---- | ---- |
| `FemView.cpp` | Core lifecycle, input handling, scene management |
| `FemViewImGui.cpp` | `onDrawImGui` + helpers (`drainScriptQueue`, `drawMainMenuBar`, `drawPopups`, `drawFileDialogs`) |
| `FemViewSolverHandler.cpp` | `executeCalc`, `recompute` |
| `FemViewEigenmodeHandler.cpp` | All eigenmode compute/visualize/animate operations |
| `FemViewAiHandler.cpp` | `makeRequest`, `onGenerationComplete` |
| `FemViewSelectionHandler.cpp` | Structure-aware selection (grow, shrink, connected, member, same plane, same material) |
| `FemViewGeometryHandler.cpp` | Geometry modification (translate, scale, rotate, taper, smooth, mirror) |
| `FemViewModelGraph.cpp` | Shared `ofview_detail::ModelGraph` — model topology, model↔shape maps, selection readback |
| `FemViewScriptRunner.cpp` | `runPlugin`, `runScript`, `runScriptFromText` |

**Static handler class pattern** — operation clusters are extracted into classes with only `static` methods that take `FemViewWindow &view`. Each handler is declared `friend class` in `FemView.h` so it can access private members directly. Follow this pattern when adding new operation groups.

**`ModelGraph`** — handlers that need topology rather than screen geometry build an `ofview_detail::ModelGraph` (`FemViewModelGraph.h`) with `buildGraph(view, graph)`. It gives the node/beam lists in model order, the `beamsAt` adjacency, the model↔scene-shape maps, and the current selection expressed in model terms. It is built fresh per command — the models are small enough that this beats keeping an index in sync.

**Nested state struct pattern** — related private member variables are grouped into nested structs with default member initializers. Current structs:

- `AppPaths m_paths` — all filesystem paths (`prog`, `font`, `image`, `plugin`, `map`, `python`, `example`, `ai`) and `fileName`
- `ViewSettings m_view` — `representation`, `relNodeSize/Load/LineRadius`, `uiScale`, `useSphereCursor`, `useBlending`, `useImGuiFileDialogs`, `saveScreenShot`
- `EditState m_edit` — `currentMaterial`, `currentElementLoad`, `currentNodeLoad`, `currentNodeBC`, `clipBoard`
- `DialogFlags m_dlg` — all 14 boolean show/open/save dialog flags
- `ScriptingState m_scripting` — `pluginRunning`, `calledNewModel`, `running`, `plugins`, `examples`
- `EigenmodeState m_eigenmode` — `inSecondaryView`, `showing`, `savedShowNodeNumbers`
- `SolverState m_solver` — `beam`, `current`, `needRecalc`, `saneModel`, `haveScaleFactor`, `lockScaleFactor`
- `AiState m_ai` — `apiKey`, `structureGenerator`, `promptDatabase`, `isProcessing`, `autoRunScript`, `systemPromptFilename`, `scriptQueueMutex`, `pendingScripts`

**IntelliSense false positives** — VS Code may report `cannot open source file "ivf/Base.h"` on handler `.cpp` files. This is an IDE include-path issue; CMake configures the paths correctly and the build succeeds. Do not treat these as real errors.

### Selection system

Selection lives in `IvfViewWindow` (gestures, screen-space maths) with FEM-specific decisions delegated to `FemViewWindow` through virtuals.

**Three gestures, three modes** — `WidgetMode::Select` (click), `WidgetMode::BoxSelection` (screen-space rubber band drag), `WidgetMode::PaintSelect` (drag over objects). All share one modifier convention via `IvfViewWindow::currentSelectOp()`: plain = `SelectOp::Replace`, `[Shift]` = `Add`, `[Ctrl]` = `Remove`. Apply this convention to any new selection gesture.

**Rubber band** — `BoxSelection` projects each shape through `Camera::glmProjectionMatrix() * glmViewMatrix()` into window pixels (`projectToScreen`). Dragging right-to-left sets `m_rubberBandCrossing`, switching from "fully enclosed" to "touched". Point-like shapes are tested by `isInsideRect()`; everything else goes to the virtual `onInsideRect()`, which `FemViewWindow` overrides to test beams as segments. This mirrors the older `isInsideVolume()` / `onInsideVolume()` pair.

**Rubber band drawing must happen before `onDrawImGui()`** in `IvfViewWindow::doDrawImGui()`. `FemViewWindow::onDrawImGui()` ends with `ImGui::Render()`, which finalizes all draw lists — appending to `GetForegroundDrawList()` after that point crashes on an already-popped clip-rect stack.

**Never call `draw()` from a GLFW callback.** `GLFWApplication::run()` renders continuously, so `redraw()` is sufficient; a nested `draw()` opens a second ImGui frame inside the one already in flight.

**Selection filter** — `m_selectFilter` is what is in force; `m_userSelectFilter` is the user's toolbar choice. Modes that need their own filter (e.g. `CreateLine` needs nodes) override `m_selectFilter`; the three selection modes restore `m_userSelectFilter`. One-off commands that need a temporary filter must save and restore it.

**Dormant code** — `WidgetMode::SelectVolume`, the `m_volumeSelection` wire brick and `selectAllBox()` are the pre-rubber-band world-space volume selection. Nothing reaches them from the UI.

### Geometry modification

`FemViewGeometryHandler` holds the commands that change model geometry. Pure maths lives in `ofmath` (`include/ofmath/geom_ops.h`) and knows nothing about FEM or scene types. Four invariants apply to every geometry command — follow them when adding more:

**Move nodes, never rebuild them.** Node and beam indices must survive, or scripts holding indices and result arrays indexed by node silently misalign. Only `mirror()` adds anything, and it appends.

**Write to the model, refresh once.** Commands call `ofem::Node::setCoord` and then `refreshBeamModelVisuals()` exactly once. Do not use `updateNodePosAt` in a loop — it goes through `vfem::Node::setPosition`, which updates the node's own shape but leaves the attached beams stale.

**Reset the representation, but only from a displacement mode.** `begin()` does `snapShot()` → drop to `RepresentationMode::Geometry` *if* the current mode is `Displacements` or `Results` → `m_solver.needRecalc = true`. `vfem::Node::refresh()` draws a node at its model coordinate *plus* its scaled displacement, so moving nodes while displacements are on screen would blend new geometry with old deformation. Do **not** reset unconditionally: `Fem` and `Geometry` are both undeformed but render differently (`IVF_BEAM_SOLID` tubes vs `IVF_BEAM_EXTRUSION` cross-sections), so forcing the switch visibly changes beam thickness for no reason — and because selected beams bypass the display list while unselected ones replay it, the two halves of the model end up drawn in different modes until the next deselect.

**The affected set is selected nodes ∪ endpoints of selected beams**, so selecting a beam and scaling behaves as it looks. `PinPolicy` then excludes nodes with BCs or loads: transforms default to pinning neither (an explicit transform should do exactly what was asked), while smoothing and projection default to pinning both, because silently relaxing a support or load point changes what the model means.

`mirror()` copies geometry and materials only — BCs and loads have direction, and reflecting them would change the load case without saying so. It also welds via `connectNearNodes()`, which takes its own snapshot, so a welded mirror currently costs two undo steps.

The public forwarders on `FemViewWindow` (`translateSelection`, `scaleSelection`, `rotateSelection`, `taperSelection`, `smoothSelection`, `mirrorSelection`) take the origin as an `int` (0 world, 1 centroid, 2 bounding box centre, 3 cursor, 4 bounding box low face, 5 high face) so they bind directly into ChaiScript. Mirror must use 0, 4 or 5 — a plane through the middle of the selection reflects it onto itself and the weld then removes the copy.

**One transform path** — `TransformParams` describes any node-moving operation (translate, scale, rotate, taper, smooth), and `applyTo()` is the only place that implements one. Both the one-shot commands (via `runOnce()`) and the live preview go through it, so they cannot drift apart. `applyTo()` returns false for degenerate parameters, and `runOnce()` validates *before* `begin()`, so a refused command leaves no undo entry.

**`TransformContext` captures the whole model, not the selection.** Smoothing has to see the neighbours just outside the selection so a boundary node is pulled toward the structure it is attached to rather than only toward its selected neighbours. The context therefore holds every node in model order, plus `adjacency`, and three parallel flag arrays: `affected` (in selection), `hasBC`, `hasLoad`. A `movable` mask is derived from those *per call* via `movableFor()`, so toggling a pin checkbox takes effect without recapturing the model. Matrix transforms touch only movable points; smoothing runs over all points and lets the mask decide who moves. Taper is the exception that needs `movablePoints()` — it interpolates over the extent of what it is given, so handing it the whole model would measure the wrong span.

**Smoothing is Taubin, not Laplacian.** `ofmath::taubinSmooth` alternates a shrink pass (`lambda`) with an inflate pass (`mu`, negative). Plain Laplacian (`mu == 0`) pulls a free-standing frame toward its centroid, which changes the span rather than tidying it. `laplacianPass` computes every new position from the old ones, so results don't depend on node storage order.

**Live preview** (`ofui::TransformWindow`) — `beginPreview()` captures the affected nodes and their coordinates into `GeometryState m_geometry`. Every parameter change re-applies the transform **from that baseline** rather than compounding, so the result depends only on the current values, never on how the slider was dragged; the origin is resolved from the baseline too, so it doesn't drift. `applyPreview()` restores the baseline, *then* snapshots, then applies — which puts the undo point at the start of the gesture and yields exactly one undo entry per gesture.

A preview holds raw `ofem::Node *`, so it is cancelled from `onSelect()`/`onDeSelect()` and whenever the panel is found hidden in `onDrawImGui()`. Any new path that deletes nodes while a preview could be live must cancel it too.

`ofui` headers must not include `objframe` headers — `transform_window.h` keeps its widget values in a plain `Fields` struct and translates them to `TransformParams` in the `.cpp`, which is the only place that includes `FemView.h`.

### Keyboard shortcuts

`IvfViewWindow::onGlfwKey()` dispatches modifier + key combinations to the virtual `doShortcut()` / `onShortcut()` pair; `FemViewWindow::onShortcut()` holds all application shortcuts. Only one modifier is reported per event — shift, then ctrl, then alt win in that order — and keys at or above `GLFW_KEY_LEFT_SHIFT` are never treated as shortcuts.

**Punctuation keys are layout-dependent.** GLFW key tokens name *physical* keys by their US-layout position, so `GLFW_KEY_MINUS` is not the key that prints `-` on a non-US keyboard (on a Swedish layout it prints `+`, and `-` sits at `GLFW_KEY_SLASH`). `onGlfwKey()` therefore runs every shortcut through `normalizeShortcutKey()`, which asks `glfwGetKeyName()` what the key actually prints in the active layout and funnels anything printing `+` or `-` into `GLFW_KEY_KP_ADD` / `GLFW_KEY_KP_SUBTRACT`. Match against the keypad tokens when binding a punctuation shortcut; add the US-layout token (e.g. `GLFW_KEY_EQUAL` for `+`, which needs shift there) only as an extra case.

### Library modules (each compiled as a separate static lib)

| Library | Source | Headers | Namespace | Role |
|---------|--------|---------|-----------|------|
| `fem` | `src/fem/` | `include/ofem/` | `ofem` | Pure FEM data model — nodes, beams, materials, loads, BCs, sections, file I/O |
| `visfem` | `src/visfem/` | *(same dir)* | `vfem` | ivf++-based visual representations of FEM objects |
| `ofui` | `src/ofui/` | `include/ofui/` | `ofui` | All ImGui panel/popup/window classes |
| `ofsolve` | `src/ofsolve/` | `include/ofsolve/` | `ofsolver` | Solver interface + TetGen beam mesher (calls `tetgen.exe`) |
| `ofmath` | `src/ofmath/` | `include/ofmath/` | — | Grid plane, ray-cylinder intersection |
| `ofservice` | `src/ofservice/` | `include/ofservice/` | `ofservice` | CivetWeb HTTP REST server (port 8081) |
| `ofai` | *(headers only)* | `include/ofai/` | `ofai` | Claude API integration — `StructureGenerator`, `PromptDatabase` |
| `util` | `src/util/` | `include/ofutil/` | `ofutil` | ColorMap, ResultInfo, logger, app_settings |

The convenience target `objframe::libs` (alias `objframe_libs`) links all of the above plus all third-party dependencies.

### Key data flow

1. **FEM data** lives in `ofem::BeamModel` (owned by `FemViewWindow`).
2. **Visual data** lives in the ivf++ scene graph (`vfem::BeamModel` wraps the ofem model with renderable shapes).
3. **Solver** (`ofsolver::BeamSolverPtr`) reads from `ofem::BeamModel` and writes results back; the view then switches `RepresentationMode` to `Displacements` or `Results`.
4. **Scripts** (ChaiScript) call `FemViewWindow` methods directly via bindings in `FemViewScriptBindings` ([src/objframe/FemViewScriptBindings.h](src/objframe/FemViewScriptBindings.h)). Scripts are always executed on the main render thread via a mutex-protected queue (`m_ai.pendingScripts`).
5. **AI generation** (`ofai::StructureGenerator`) calls the Claude API asynchronously via CURL, extracts ChaiScript from the response, and posts it to the script queue. The API key is stored in application settings.

### UI system

All panels are ImGui-based and rendered inside `FemViewWindow::onDrawImGui()`. Panel classes in `ofui` follow the pattern: inherit `ofui::UiWindow`, override `draw()`, expose a `create()` factory returning `std::shared_ptr<XxxWindow>`. `FemViewWindow` holds shared pointers to every panel and toggling boolean flags (`m_show*`) controls visibility.

### Scripting / Plugin system

- **Inline scripts**: ChaiScript run from the console or via the UI.
- **Plugins** (`ScriptPlugin`): `.chai` files with a metadata header (name, parameters). Plugins are discovered from the plugin directory at startup and listed in the UI. Parameter UI is generated automatically from the metadata.
- **AI scripts**: Generated by `ofai::StructureGenerator`, treated as inline scripts.

### REST API (`ofservice`)

`ofservice::Service` registers CivetWeb handlers on port 8081. The library has **no dependency on `objframe` headers** — the circular dependency is broken by the `ofservice::IAppController` interface.

**Decoupling via `IAppController`:**

- `include/ofservice/iapp_controller.h` — pure abstract interface with 48 virtual methods covering model lifecycle, node/beam CRUD, selection, BCs, loads, and queries
- `src/objframe/AppControllerAdapter` — `FemViewWindow`-side implementation that delegates each interface method to the corresponding `FemViewWindow` method
- `ofservice::App` singleton holds an `IAppController*`; `ofservice` never sees `FemViewWindow` or any `objframe` header

**Endpoint surface (48 endpoints, all POST to `/cmds/<name>`):**

| Category | Endpoints |
| -------- | --------- |
| Model lifecycle | `new_model`, `open_model`, `save_model`, `export_model`, `import_model`, `snap_shot` |
| Node/beam creation | `add_nodes`, `add_beams` |
| Selection | `select_all`, `select_all_nodes`, `clear_selection`, `add_last_node_to_selection`, `select_node_at`, `select_beam_at` |
| Node/beam mutation | `delete_node_at`, `delete_beam_at`, `subdivide_beam_at`, `connect_near_nodes`, `update_node_pos_at`, `update_beam_at` |
| Boundary conditions | `assign_node_fixed_bc_ground`, `assign_node_pos_bc_ground`, `assign_node_fixed_bc_at`, `assign_node_pos_bc_at`, `remove_node_bc_at` |
| Loads | `clear_all_loads`, `clear_all_bcs`, `add_node_load_at`, `clear_node_load_at`, `add_beam_load_at`, `clear_beam_load_at` |
| Mesh generation | `mesh_selected_nodes`, `surface_selected_nodes` |
| Queries | `node_count`, `beam_count`, `node_pos_at`, `beam_at`, `find_node_near`, `is_node_fixed_at`, `is_node_pos_bc_at`, `is_node_selected_at`, `has_node_load_at`, `has_beam_load_at`, `node_load_count`, `beam_load_count`, `material_count`, `model_bounds` |

**Response format:** void handlers return `text/html 200 OK` with no body. Query handlers return `application/json` with a body: `{"value": n}` for scalars, `{"pos": [x,y,z]}` for positions, `{"i0": i0, "i1": i1}` for beam indices, `{"min": [...], "max": [...]}` for bounds.

The Python REST client is in `python/rest_client/ofapi.py`.

### External dependency: IVF++

ivf++ provides the scene graph (`ivf::Composite`, `ivf::Shape`, `ivf::Camera`, `ivf::Workspace`, etc.) and math utilities. Its headers are under `../ivfplusplus/include/`. Do not modify files under `vcpkg_installed/` or the ivf++ sibling repo.
