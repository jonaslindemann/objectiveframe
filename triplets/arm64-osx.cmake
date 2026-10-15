# Overlay of vcpkg's arm64-osx triplet that pins the minimum macOS version, so
# the static dependencies linked into objframe run as far back as the app does.
# Keep VCPKG_OSX_DEPLOYMENT_TARGET equal to CMAKE_OSX_DEPLOYMENT_TARGET in the
# top-level CMakeLists.txt.
set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Darwin)
set(VCPKG_OSX_ARCHITECTURES arm64)
set(VCPKG_OSX_DEPLOYMENT_TARGET 13.3)
