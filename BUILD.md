# Building ObjectiveFrame

ObjectiveFrame uses CMake as its build system with automatic vcpkg integration for dependency management. **No build scripts are required** - you can build directly with CMake commands.

## Prerequisites

1. **CMake** (version 3.24 or higher)
2. **vcpkg** package manager (auto-detected)
3. **C++ Compiler** with C++20 support
   - Windows: Visual Studio 2022 or later
   - Linux: GCC 10+ or Clang 12+
   - macOS: Xcode 12+

## vcpkg Setup

The build system automatically detects vcpkg in the following locations:

1. `VCPKG_ROOT` environment variable
2. Common installation paths:
   - Windows: `c:/vcpkg` or `e:/vcpkg`
   - Linux/macOS: `$HOME/vcpkg` or `/usr/local/vcpkg`

If you don't have vcpkg installed:

```bash
# Windows
git clone https://github.com/Microsoft/vcpkg.git c:/vcpkg
c:/vcpkg/bootstrap-vcpkg.bat

# Linux/macOS
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
```

Optionally set the `VCPKG_ROOT` environment variable:

```bash
# Windows (PowerShell)
$env:VCPKG_ROOT = "c:/vcpkg"

# Linux/macOS
export VCPKG_ROOT=$HOME/vcpkg
```

## Dependencies

ObjectiveFrame requires the following packages (automatically installed via vcpkg):

- PNG
- JPEG
- ZLIB
- glfw3
- GLEW
- OpenGL
- CURL
- ChaiScript (header-only)

See `vcpkg.json` for the complete list.

## Building

### Option 1: Using CMake directly (Recommended)

The build system works with both single-config (Makefile, Ninja) and multi-config (Visual Studio, Xcode) generators.

#### Multi-config generators (Visual Studio, Xcode)

```bash
# Configure once
cmake -S . -B build -G"Visual Studio 17 2022"

# Build Release
cmake --build build --config Release

# Build Debug
cmake --build build --config Debug

# Or open build/objframe2.sln in Visual Studio
```

#### Single-config generators (Makefile, Ninja)

```bash
# Configure for Release
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build-release

# Configure for Debug
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

### Option 2: Using the build script (Optional convenience wrapper)

```bash
# Configure and build (Release by default)
python build-of.py --build

# Configure and build Debug
python build-of.py --build --build-type Debug

# Configure only
python build-of.py --configure --build-type Release

# Clean build directories
python build-of.py --clean

# Control parallel jobs
python build-of.py --build --jobs 8
```

## Build Directories

**Multi-config generators:**
- `build/` - Single build directory for all configurations
- `vcpkg_installed/` - Shared vcpkg packages (saves disk space)

**Single-config generators:**
- `build-release/` - Release build artifacts
- `build-debug/` - Debug build artifacts
- `vcpkg_installed/` - Shared vcpkg packages (saves disk space)

## Output

After building, the binaries will be in:
- `bin/` - Executables (Debug builds have 'd' suffix, e.g., `objframe_glfwd.exe`)
- `lib/` - Libraries (Debug libraries have 'd' suffix, e.g., `femd.lib`)

## Using ObjectiveFrame as a Dependency

If you want to link against ObjectiveFrame in another CMake project:

```cmake
# In your CMakeLists.txt
find_package(objframe REQUIRED)
target_link_libraries(your_target PRIVATE objframe::objframe)
```

The `objframe::objframe` interface library bundles all ObjectiveFrame libraries and their dependencies for convenient linking.

## Troubleshooting

### vcpkg not found
If the build system can't find vcpkg:
1. Set the `VCPKG_ROOT` environment variable
2. Or install vcpkg to a standard location (see vcpkg Setup above)

### Missing dependencies
If vcpkg dependencies fail to install, you may need to update vcpkg:
```bash
cd $VCPKG_ROOT
git pull
./bootstrap-vcpkg.sh  # or bootstrap-vcpkg.bat on Windows
```

### Build errors with IVF++
ObjectiveFrame depends on the IVF++ library which should be located at `../ivfplusplus` relative to the ObjectiveFrame directory. Make sure you have it cloned and built.

## Platform-Specific Notes

### Windows
- Visual Studio 2022 is recommended
- The build uses `/bigobj` flag for large object files
- Debug builds use `/Z7` instead of `/Zi` for better compatibility

### Linux
- Additional system libraries may be required: X11, Xft, Xcursor, Xinerama, Xrender, Xfixes, fontconfig, pthread

### macOS
- Xcode command line tools required
- Use Homebrew or vcpkg for dependencies
