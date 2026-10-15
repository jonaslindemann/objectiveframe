# macOS installer package for ObjectiveFrame.
#
# Adds a "pkg" target that stages ObjectiveFrame.app from the Release build,
# signs it, wraps it in an installer package for /Applications and, when a
# notary profile is given, notarizes and staples it. It is never part of the
# default build - packaging is always asked for explicitly:
#
#     cmake --build build-release --target pkg

if(NOT APPLE)
    return()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "macOS packaging: Debug build, the 'pkg' target is unavailable")
    return()
endif()

# ------------------------------------------------------------------ tools ---

find_program(OF_PKGBUILD_EXECUTABLE pkgbuild)
find_program(OF_PRODUCTBUILD_EXECUTABLE productbuild)
find_program(OF_CODESIGN_EXECUTABLE codesign)
find_program(OF_SIPS_EXECUTABLE sips)
find_program(OF_ICONUTIL_EXECUTABLE iconutil)

foreach(_tool PKGBUILD PRODUCTBUILD CODESIGN SIPS ICONUTIL)
    if(NOT OF_${_tool}_EXECUTABLE)
        message(STATUS "macOS packaging: ${_tool} not found, the 'pkg' target is unavailable")
        return()
    endif()
endforeach()

# ------------------------------------------------------- package identity ---

# The bundle identifier is what macOS uses to recognise the app across updates,
# and Installer uses it to find the copy to upgrade. Once a package has been
# shipped, do not change it.
set(OF_PKG_BUNDLE_ID "se.lu.byggmek.objectiveframe"
    CACHE STRING "macOS bundle and package identifier")
# Empty means the project() version in the top-level CMakeLists.txt. Leaving
# the cache entry empty matters: a cached default is never refreshed, so a
# version copied in at first configure would outlive every later bump. The
# normal variable set below shadows the cache entry for the rest of this file.
set(OF_PKG_VERSION "" CACHE STRING
    "Version shown in Finder and checked by Installer (empty: the project() version)")
if(NOT OF_PKG_VERSION)
    set(OF_PKG_VERSION "${PROJECT_VERSION}")
endif()
set(OF_PKG_ARCH "${CMAKE_SYSTEM_PROCESSOR}"
    CACHE STRING "Architectures the installer accepts (arm64, x86_64 or arm64,x86_64)")

# ------------------------------------------------------------ signing ------

# Distribution outside the Mac App Store needs two Developer ID certificates
# from the Apple Developer Program - "Apple Development" certificates are for
# running on your own devices and Gatekeeper rejects them. List what the
# keychain holds with:
#
#     security find-identity -v -p basic
#
# Leave both empty for an unsigned package to test locally. The app is then
# ad-hoc signed, which Apple Silicon requires to run it at all.
set(OF_PKG_SIGN_APP "" CACHE STRING
    "Code signing identity for the app, e.g. 'Developer ID Application: Name (TEAMID)'")
set(OF_PKG_SIGN_INSTALLER "" CACHE STRING
    "Signing identity for the package, e.g. 'Developer ID Installer: Name (TEAMID)'")

# Notarization uses credentials stored once in the keychain:
#
#     xcrun notarytool store-credentials objectiveframe-notary \
#         --apple-id <apple id> --team-id <TEAMID> --password <app-specific password>
#
# and then OF_PKG_NOTARY_PROFILE=objectiveframe-notary. Empty skips it.
set(OF_PKG_NOTARY_PROFILE "" CACHE STRING
    "notarytool keychain profile; empty skips notarization")

# Meshing runs tetgen as a separate program, shipped inside the bundle. By
# default that is the one src/tetgen builds; set this to bundle another binary.
# With neither, meshing falls back to a tetgen on PATH, which an app started
# from Finder will not find.
set(OF_PKG_TETGEN "" CACHE FILEPATH "macOS tetgen executable to bundle (default: the built tetgen target)")

set(_of_tetgen "${OF_PKG_TETGEN}")
if(NOT _of_tetgen AND TARGET tetgen)
    set(_of_tetgen "$<TARGET_FILE:tetgen>")
endif()

if(OF_PKG_NOTARY_PROFILE AND NOT (OF_PKG_SIGN_APP AND OF_PKG_SIGN_INSTALLER))
    message(FATAL_ERROR
        "OF_PKG_NOTARY_PROFILE is set but the package is not fully signed. "
        "Notarization needs both OF_PKG_SIGN_APP and OF_PKG_SIGN_INSTALLER.")
endif()

# ---------------------------------------------------------------- metadata ---

set(_of_info_plist "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
configure_file(Info.plist.in "${_of_info_plist}" @ONLY)

set(_of_distribution "${CMAKE_CURRENT_BINARY_DIR}/distribution.xml")
configure_file(distribution.xml.in "${_of_distribution}" @ONLY)

# -------------------------------------------------------------------- icon ---

set(_of_icon_source "${PROJECT_SOURCE_DIR}/images/logo.png")
set(_of_iconset "${CMAKE_CURRENT_BINARY_DIR}/ObjectiveFrame.iconset")
set(_of_icns "${CMAKE_CURRENT_BINARY_DIR}/ObjectiveFrame.icns")

set(_of_icon_commands
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${_of_iconset}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${_of_iconset}")
foreach(_size 16 32 128 256 512)
    math(EXPR _size2x "${_size} * 2")
    list(APPEND _of_icon_commands
        COMMAND "${OF_SIPS_EXECUTABLE}" -z ${_size} ${_size} "${_of_icon_source}"
                --out "${_of_iconset}/icon_${_size}x${_size}.png"
        COMMAND "${OF_SIPS_EXECUTABLE}" -z ${_size2x} ${_size2x} "${_of_icon_source}"
                --out "${_of_iconset}/icon_${_size}x${_size}@2x.png")
endforeach()

add_custom_command(
    OUTPUT "${_of_icns}"
    ${_of_icon_commands}
    COMMAND "${OF_ICONUTIL_EXECUTABLE}" -c icns "${_of_iconset}" -o "${_of_icns}"
    DEPENDS "${_of_icon_source}"
    COMMENT "Rendering macOS app icon"
    VERBATIM)

add_custom_target(pkg_icon DEPENDS "${_of_icns}")
set_target_properties(pkg_icon PROPERTIES FOLDER "Packaging")

# ------------------------------------------------------------------ target ---

set(OF_PKG_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/packages/macos"
    CACHE PATH "Directory the .pkg is written to")
string(REPLACE "," "-" _of_arch_tag "${OF_PKG_ARCH}")
set(_of_pkg "${OF_PKG_OUTPUT_DIR}/ObjectiveFrame-${OF_PKG_VERSION}-${_of_arch_tag}.pkg")

add_custom_target(pkg
    COMMAND "${CMAKE_COMMAND}"
            "-DOF_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DOF_EXE=$<TARGET_FILE:objframe>"
            "-DOF_WORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/work"
            "-DOF_INFO_PLIST=${_of_info_plist}"
            "-DOF_ICNS=${_of_icns}"
            "-DOF_COMPONENT_PLIST=${CMAKE_CURRENT_SOURCE_DIR}/component.plist"
            "-DOF_DISTRIBUTION=${_of_distribution}"
            "-DOF_OUTPUT=${_of_pkg}"
            "-DOF_BUNDLE_ID=${OF_PKG_BUNDLE_ID}"
            "-DOF_VERSION=${OF_PKG_VERSION}"
            "-DOF_TETGEN=${_of_tetgen}"
            "-DOF_SIGN_APP=${OF_PKG_SIGN_APP}"
            "-DOF_SIGN_INSTALLER=${OF_PKG_SIGN_INSTALLER}"
            "-DOF_NOTARY_PROFILE=${OF_PKG_NOTARY_PROFILE}"
            "-DOF_CODESIGN=${OF_CODESIGN_EXECUTABLE}"
            "-DOF_PKGBUILD=${OF_PKGBUILD_EXECUTABLE}"
            "-DOF_PRODUCTBUILD=${OF_PRODUCTBUILD_EXECUTABLE}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/pack_pkg.cmake"
    COMMENT "Building macOS package ${_of_pkg}"
    VERBATIM
    USES_TERMINAL)

add_dependencies(pkg objframe pkg_icon)
if(TARGET tetgen)
    add_dependencies(pkg tetgen)
endif()
set_target_properties(pkg PROPERTIES FOLDER "Packaging")

message(STATUS "macOS packaging: target 'pkg' available, output ${_of_pkg}")
message(STATUS "  bundle id      : ${OF_PKG_BUNDLE_ID}")
message(STATUS "  version        : ${OF_PKG_VERSION}")
message(STATUS "  minimum macOS  : ${CMAKE_OSX_DEPLOYMENT_TARGET}")
if(OF_PKG_SIGN_APP)
    message(STATUS "  app signed by  : ${OF_PKG_SIGN_APP}")
else()
    message(STATUS "  app signed by  : ad-hoc (local testing only)")
endif()
if(OF_PKG_SIGN_INSTALLER)
    message(STATUS "  pkg signed by  : ${OF_PKG_SIGN_INSTALLER}")
else()
    message(STATUS "  pkg signed by  : unsigned")
endif()
if(OF_PKG_NOTARY_PROFILE)
    message(STATUS "  notarization   : profile '${OF_PKG_NOTARY_PROFILE}'")
else()
    message(STATUS "  notarization   : skipped")
endif()
if(OF_PKG_TETGEN)
    message(STATUS "  tetgen         : ${OF_PKG_TETGEN}")
elseif(TARGET tetgen)
    message(STATUS "  tetgen         : built from source (src/tetgen)")
else()
    message(STATUS "  tetgen         : not bundled - meshing needs tetgen on PATH")
endif()
