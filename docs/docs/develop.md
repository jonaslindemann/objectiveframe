# Develop and extend ObjectiveFrame

ObjectiveFrame is a C++ application built upon several libraries. To build the application, required libraries have to be built. The main library used is Ivf++, an object-oriented 3D scene graph library encapsulating the OpenGL library. The second library is GLFW for creating OpenGL windows and providing a source of window events.

## Project structure

ObjectiveFrame is built as a set of static libraries with the application on top:

| Library | Namespace | Role |
| --- | --- | --- |
| `fem` | `ofem` | The FEM data model - nodes, beams, materials, loads, boundary conditions, sections, file I/O |
| `visfem` | `vfem` | Ivf++ based visual representations of the FEM objects |
| `ofui` | `ofui` | All Dear ImGui panels, popups and toolbars |
| `ofsolve` | `ofsolver` | Solver interface, and the TetGen based mesher |
| `ofmath` | — | Geometry maths: grid planes, ray intersection, transforms, smoothing |
| `ofservice` | `ofservice` | The CivetWeb HTTP service behind the [Automation API](rest-api.md) |
| `ofai` | `ofai` | Claude API integration for the [AI workflow](llm-integration.md) |
| `util` | `ofutil` | Colour maps, result info, logging, application settings |

The application layers three windows: `GLFWWindow` owns the window and the OpenGL context, `IvfViewWindow` adds the Ivf++ scene graph, the camera and the editing modes, and `FemViewWindow` holds the FEM model, the user interface, the scripting engine and the AI integration.

`ofui` never includes application headers, and `ofservice` reaches the application only through the `ofservice::IAppController` interface, so neither library depends on `FemViewWindow`.

## Prerequisites

Before building ObjectiveFrame, ensure you have the following prerequisites installed:

1. **CMake** (version 3.24 or higher)
   - Download from [cmake.org](https://cmake.org/download/)
   - Ensure `cmake` is in your PATH

2. **C++ Compiler** with C++20 support
   - **Windows**: Visual Studio 2022 or later (with C++ workload)
   - **Linux**: GCC 10+ or Clang 12+
     ```bash
     # Ubuntu/Debian
     sudo apt update
     sudo apt install build-essential cmake

     # Fedora/CentOS
     sudo dnf install gcc-c++ cmake
     ```

3. **Git** (for cloning repositories)
   - Download from [git-scm.com](https://git-scm.com/)

## vcpkg Setup

ObjectiveFrame uses [vcpkg](https://github.com/Microsoft/vcpkg) for dependency management. The build system automatically detects vcpkg in the following locations:

1. `VCPKG_ROOT` environment variable
2. Common installation paths:
   - Windows: `c:/vcpkg` or `e:/vcpkg`
   - Linux: `$HOME/vcpkg` or `/usr/local/vcpkg`

### Installing vcpkg

If you don't have vcpkg installed:

=== "Windows"

    ```powershell
    # Clone vcpkg
    git clone https://github.com/Microsoft/vcpkg.git c:/vcpkg

    # Bootstrap vcpkg
    c:/vcpkg/bootstrap-vcpkg.bat

    # Optionally set VCPKG_ROOT
    $env:VCPKG_ROOT = "c:/vcpkg"
    ```

=== "Linux"

    ```bash
    # Clone vcpkg
    git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg

    # Bootstrap vcpkg
    ~/vcpkg/bootstrap-vcpkg.sh

    # Optionally set VCPKG_ROOT
    export VCPKG_ROOT=$HOME/vcpkg
    ```

### Dependencies

ObjectiveFrame requires the following packages (automatically installed via vcpkg):

- PNG
- JPEG
- ZLIB
- glfw3
- GLEW
- OpenGL
- CURL
- ChaiScript (header-only)

See `vcpkg.json` in the root directory for the complete list of dependencies.

## Building ObjectiveFrame

### Using CMake directly (Recommended)

The build system works with both single-config and multi-config generators.

#### Windows (Visual Studio)

```powershell
# Configure the project
cmake -S . -B build -G "Visual Studio 17 2022"

# Build Release configuration
cmake --build build --config Release

# Build Debug configuration
cmake --build build --config Debug
```

Alternatively, open `build/objframe2.sln` in Visual Studio and build from there.

#### Linux (Makefile/Ninja)

```bash
# Configure for Release
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release

# Build Release
cmake --build build-release

# Configure for Debug
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug

# Build Debug
cmake --build build-debug
```

### Using Ninja (Faster builds)

On both Windows and Linux, you can use Ninja for faster builds:

```bash
# Install Ninja
# Windows: choco install ninja
# Linux: sudo apt install ninja-build

# Configure with Ninja
cmake -S . -B build-ninja -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build-ninja
```

### Build Options

You can customize the build with these CMake options:

- `CMAKE_BUILD_TYPE`: Set to `Release`, `Debug`, or `RelWithDebInfo`
- `CMAKE_INSTALL_PREFIX`: Installation directory
- `IVF_ROOT`: Location of the Ivf++ tree (default: `../ivfplusplus`, next to this repository)
- `OF_SWEPT_EXTRUSION`: Build beam geometry on the gle-free swept extrusion classes (default: ON)
- `USE_LEAP`: Build with LeapMotion support (default: OFF)

Example:
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./install
```

## Running ObjectiveFrame

After building, you can run ObjectiveFrame from the installation directory:

=== "Windows"
    ```powershell
    # From Release build
    .\bin\Release\objframe.exe

    # From Debug build
    .\bin\Debug\objframe.exe
    ```

=== "Linux"
    ```bash
    # From Release build
    ./build-release/objframe

    # From Debug build
    ./build-debug/objframe
    ```

### Command Line Options

The application itself takes a few options, which are useful while developing:

- `--ui-mode=simple-bar` / `--ui-mode=simple-beam` / `--ui-mode=advanced`: start in that interface profile for this run only, without changing the stored setting.
- `--gldebug`: create a debug OpenGL context and route driver messages into the log window.
- `--core`, `--legacy`, `--mixed`: choose the render profile. Core is the default.

A model filename can also be passed, and is opened at startup.

## Troubleshooting

### Common Issues

1. **vcpkg not found**: Ensure `VCPKG_ROOT` is set or vcpkg is in a standard location
2. **CMake errors**: Make sure you have CMake 3.24+
3. **Compiler errors**: Ensure your compiler supports C++20
4. **Missing dependencies**: Run `vcpkg install` manually if needed

### Manual vcpkg Installation

If automatic dependency installation fails:

```bash
# Install dependencies manually
vcpkg install --triplet x64-windows  # Windows
vcpkg install --triplet x64-linux    # Linux
```

## Contributing

When contributing to ObjectiveFrame:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test your build on both Windows and Linux
5. Submit a pull request

## Building Ivf++

Ivf++ is not automatically built via vcpkg. You need to build it manually. The recommended approach is to clone and build Ivf++ in a directory next to the objectiveframe project source.

### Prerequisites for Ivf++

- CMake 3.24+
- vcpkg (same setup as ObjectiveFrame)
- C++20 compatible compiler

### Building Ivf++

```bash
# Clone Ivf++ repository next to objectiveframe
cd ..
git clone https://github.com/jonaslindemann/ivfplusplus.git

# Create build directory
cd ivfplusplus
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Install (optional, or set IVFPLUSPLUS_DIR)
cmake --install . --prefix ../install
```

### Integration with ObjectiveFrame

ObjectiveFrame looks for Ivf++ in `../ivfplusplus`, next to this repository, which is why the clone above is made there. If it lives somewhere else, point `IVF_ROOT` at it:

=== "Windows"
    ```powershell
    $env:IVF_ROOT = "c:\src\ivfplusplus"
    ```

=== "Linux"
    ```bash
    export IVF_ROOT=$HOME/src/ivfplusplus
    ```

Or pass it to CMake:
```bash
cmake -S . -B build -DIVF_ROOT=../ivfplusplus
```

The build reads the headers from `$IVF_ROOT/include` and the libraries from `$IVF_ROOT/lib`, so Ivf++ has to be built but does not have to be installed.

## Building GLFW

GLFW is automatically installed via vcpkg. No manual building required.
