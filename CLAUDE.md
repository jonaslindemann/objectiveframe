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

**Static handler class pattern** — operation clusters are extracted into classes with only `static` methods that take `FemViewWindow &view`. Each handler is declared `friend class` in `FemView.h` so it can access private members directly. Follow this pattern when adding new operation groups.

**Nested state struct pattern** — related private member variables are grouped into nested structs with default member initializers. Current structs:

- `EigenmodeState m_eigenmode` — `inSecondaryView`, `showing`, `savedShowNodeNumbers`
- `SolverState m_solver` — `beam`, `current`, `needRecalc`, `saneModel`, `haveScaleFactor`, `lockScaleFactor`
- `AiState m_ai` — `apiKey`, `structureGenerator`, `promptDatabase`, `isProcessing`, `autoRunScript`, `systemPromptFilename`, `scriptQueueMutex`, `pendingScripts`

**IntelliSense false positives** — VS Code may report `cannot open source file "ivf/Base.h"` on handler `.cpp` files. This is an IDE include-path issue; CMake configures the paths correctly and the build succeeds. Do not treat these as real errors.

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

`ofservice::Service` registers CivetWeb handlers on port 8081. Each handler calls methods on `FemViewWindow` through the `ofservice::App` singleton (which holds a raw pointer to the view). The Python REST client is in `python/rest_client/ofapi.py`.

### External dependency: IVF++

ivf++ provides the scene graph (`ivf::Composite`, `ivf::Shape`, `ivf::Camera`, `ivf::Workspace`, etc.) and math utilities. Its headers are under `../ivfplusplus/include/`. Do not modify files under `vcpkg_installed/` or the ivf++ sibling repo.
