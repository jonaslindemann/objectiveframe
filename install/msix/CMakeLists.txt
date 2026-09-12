# MSIX packaging for ObjectiveFrame.
#
# Adds a "msix" target that stages the Release payload and packs it with the
# Windows SDK's makeappx. It is never part of the default build - packaging is
# always asked for explicitly:
#
#     cmake --build build --target msix --config Release

if(NOT WIN32)
    return()
endif()

# ------------------------------------------------------------- SDK tooling ---

# makeappx and signtool live in a versioned Windows SDK bin directory. Prefer
# the newest one installed; both tools ship side by side, so one search path
# list serves for both.
set(_of_sdk_hints "")
foreach(_program_files "$ENV{ProgramFiles\(x86\)}" "$ENV{ProgramFiles}")
    if(_program_files AND EXISTS "${_program_files}/Windows Kits/10/bin")
        file(GLOB _of_sdk_versions "${_program_files}/Windows Kits/10/bin/10.*")
        list(SORT _of_sdk_versions COMPARE NATURAL ORDER DESCENDING)
        foreach(_version_dir ${_of_sdk_versions})
            list(APPEND _of_sdk_hints "${_version_dir}/x64")
        endforeach()
    endif()
endforeach()

find_program(OF_MAKEAPPX_EXECUTABLE
    NAMES makeappx
    HINTS ${_of_sdk_hints}
    DOC "Windows SDK makeappx.exe, used to build the MSIX package")

find_program(OF_SIGNTOOL_EXECUTABLE
    NAMES signtool
    HINTS ${_of_sdk_hints}
    DOC "Windows SDK signtool.exe, used to sign the MSIX package")

if(NOT OF_MAKEAPPX_EXECUTABLE)
    message(STATUS "MSIX packaging: makeappx.exe not found, the 'msix' target is unavailable")
    return()
endif()

# ------------------------------------------------------- package identity ---

# These three values are what ties a package to a publisher, and they are set
# differently depending on where the package is going.
#
# Microsoft Store: reserve the app name in Partner Center, then copy the three
# values it shows under Product identity - they are assigned, not chosen, and a
# submission whose manifest disagrees with them is rejected at upload:
#
#     OF_MSIX_IDENTITY_NAME          Package/Identity/Name
#     OF_MSIX_PUBLISHER              Package/Identity/Publisher, a CN=<guid>
#     OF_MSIX_PUBLISHER_DISPLAY_NAME Package/Properties/PublisherDisplayName
#
# The Store re-signs the package with a Microsoft certificate, so leave
# OF_MSIX_SIGN off for that route.
#
# Sideloading: any identity name will do, but Identity/@Publisher must then be
# the exact subject of the certificate the package is signed with. The defaults
# below are the sideload values.
set(OF_MSIX_IDENTITY_NAME "LundUniversity.ObjectiveFrame"
    CACHE STRING "MSIX package identity name")
# This is the subject of the Lunds Universitet EV code signing certificate that
# install/objectiveframe_v2.iss signs with, reproduced character for character.
# If the certificate is ever reissued with a different subject, this has to
# follow it or the package will not install.
set(OF_MSIX_PUBLISHER "CN=Lunds Universitet, O=Lunds Universitet, S=Skane lan, C=SE, OID.2.5.4.15=Government Entity, OID.1.3.6.1.4.1.311.60.2.1.3=SE, SERIALNUMBER=Government Entity"
    CACHE STRING "MSIX publisher - must match the signing certificate subject exactly")
set(OF_MSIX_PUBLISHER_DISPLAY_NAME "Division of Structural Mechanics"
    CACHE STRING "MSIX publisher display name")
set(OF_MSIX_VERSION "${PROJECT_VERSION}.0"
    CACHE STRING "MSIX package version, four parts, last part must be 0")
set(OF_MSIX_DISPLAY_NAME "ObjectiveFrame"
    CACHE STRING "MSIX display name")
set(OF_MSIX_DESCRIPTION "Finite element beam analysis with real-time 3D visualisation"
    CACHE STRING "MSIX package description")

if(NOT OF_MSIX_VERSION MATCHES "^[0-9]+\.[0-9]+\.[0-9]+\.0$")
    message(FATAL_ERROR
        "OF_MSIX_VERSION is '${OF_MSIX_VERSION}'. An MSIX version needs four numeric "
        "parts and the fourth is reserved by the Store, so it must be 0.")
endif()

# -------------------------------------------------------------- signing ------

# Off for Store submissions - Partner Center signs. On for sideloaded builds,
# which Windows refuses to install unsigned.
option(OF_MSIX_SIGN "Sign the MSIX package after packing" OFF)
set(OF_MSIX_SIGN_CERT "" CACHE FILEPATH "Signing certificate (.pfx)")
set(OF_MSIX_SIGN_PASSWORD "" CACHE STRING "Password for OF_MSIX_SIGN_CERT")
set(OF_MSIX_SIGN_SHA1 "" CACHE STRING "SHA1 thumbprint of a certificate in the local store")
set(OF_MSIX_SIGN_TIMESTAMP "http://timestamp.digicert.com"
    CACHE STRING "RFC 3161 timestamp server, empty to skip timestamping")

if(OF_MSIX_SIGN AND NOT OF_SIGNTOOL_EXECUTABLE)
    message(FATAL_ERROR "OF_MSIX_SIGN is ON but signtool.exe was not found.")
endif()

if(OF_MSIX_SIGN)
    set(_of_signtool "${OF_SIGNTOOL_EXECUTABLE}")
else()
    set(_of_signtool "")
endif()

# ------------------------------------------------------------- MSVC runtime ---

# A package cannot run VC_redist.exe the way the Inno installer does, so the
# runtime is carried inside the package. The UCRT is part of Windows 10 and
# later and is deliberately left out.
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
set(CMAKE_INSTALL_UCRT_LIBRARIES FALSE)
include(InstallRequiredSystemLibraries)

set(_of_runtime_libs "")
if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    # The packaging script takes this as one argument; '|' survives -D where a
    # list separator would not.
    string(REPLACE ";" "|" _of_runtime_libs "${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}")
endif()

# ---------------------------------------------------------------- manifest ---

set(_of_manifest "${CMAKE_CURRENT_BINARY_DIR}/AppxManifest.xml")
configure_file(AppxManifest.xml.in "${_of_manifest}" @ONLY)

# ------------------------------------------------------------------ assets ---

set(_of_assets_dir "${CMAKE_CURRENT_BINARY_DIR}/Assets")
set(_of_asset_source "${PROJECT_SOURCE_DIR}/images/logo.png")
set(_of_assets
    "${_of_assets_dir}/StoreLogo.png"
    "${_of_assets_dir}/Square44x44Logo.png"
    "${_of_assets_dir}/Square71x71Logo.png"
    "${_of_assets_dir}/Square150x150Logo.png"
    "${_of_assets_dir}/Square310x310Logo.png"
    "${_of_assets_dir}/Wide310x150Logo.png")

find_program(OF_POWERSHELL_EXECUTABLE NAMES pwsh powershell
    DOC "PowerShell used to render the MSIX tile assets")

if(NOT OF_POWERSHELL_EXECUTABLE)
    message(STATUS "MSIX packaging: PowerShell not found, the 'msix' target is unavailable")
    return()
endif()

add_custom_command(
    OUTPUT ${_of_assets}
    COMMAND "${OF_POWERSHELL_EXECUTABLE}" -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/generate_assets.ps1"
            -Source "${_of_asset_source}"
            -OutDir "${_of_assets_dir}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/generate_assets.ps1" "${_of_asset_source}"
    COMMENT "Rendering MSIX tile assets"
    VERBATIM)

add_custom_target(msix_assets DEPENDS ${_of_assets})
set_target_properties(msix_assets PROPERTIES FOLDER "Packaging")

# ------------------------------------------------------------------ target ---

set(OF_MSIX_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/packages/windows"
    CACHE PATH "Directory the .msix package is written to")
set(_of_msix "${OF_MSIX_OUTPUT_DIR}/ObjectiveFrame-${OF_MSIX_VERSION}-x64.msix")

add_custom_target(msix
    COMMAND "${CMAKE_COMMAND}"
            "-DOF_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DOF_EXE=$<TARGET_FILE:objframe>"
            "-DOF_LAYOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/layout"
            "-DOF_MANIFEST=${_of_manifest}"
            "-DOF_ASSETS_DIR=${_of_assets_dir}"
            "-DOF_OUTPUT=${_of_msix}"
            "-DOF_MAKEAPPX=${OF_MAKEAPPX_EXECUTABLE}"
            "-DOF_RUNTIME_LIBS=${_of_runtime_libs}"
            "-DOF_SIGNTOOL=${_of_signtool}"
            "-DOF_SIGN_CERT=${OF_MSIX_SIGN_CERT}"
            "-DOF_SIGN_PASSWORD=${OF_MSIX_SIGN_PASSWORD}"
            "-DOF_SIGN_SHA1=${OF_MSIX_SIGN_SHA1}"
            "-DOF_SIGN_TIMESTAMP=${OF_MSIX_SIGN_TIMESTAMP}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/pack_msix.cmake"
    COMMENT "Building MSIX package ${_of_msix}"
    VERBATIM
    USES_TERMINAL)

add_dependencies(msix objframe msix_assets)

# Registers the staged layout as a package without installing anything, which is
# how a Store-identity package gets tested: its publisher is a Partner Center
# GUID, so no local certificate can sign it and it cannot be installed the
# ordinary way. Registering runs the app from the layout directory in the build
# tree, so the files stay writable and a rebuild is picked up by re-running this.
#
# Needs Developer Mode. Undo with
#     Get-AppxPackage -Name <identity name> | Remove-AppxPackage
add_custom_target(msix_register
    COMMAND "${OF_POWERSHELL_EXECUTABLE}" -NoProfile -ExecutionPolicy Bypass -Command
            "Add-AppxPackage -Register '${CMAKE_CURRENT_BINARY_DIR}/layout/AppxManifest.xml'"
    COMMENT "Registering the staged MSIX layout for local testing"
    VERBATIM
    USES_TERMINAL)

add_dependencies(msix_register msix)
set_target_properties(msix_register PROPERTIES FOLDER "Packaging")

set_target_properties(msix PROPERTIES FOLDER "Packaging")

# The identity triple is baked into the manifest at configure time, so report it
# here - an upload rejected for a mismatched identity is a slow way to find out.
message(STATUS "MSIX packaging: target 'msix' available, output ${_of_msix}")
message(STATUS "  identity name  : ${OF_MSIX_IDENTITY_NAME}")
message(STATUS "  publisher      : ${OF_MSIX_PUBLISHER}")
message(STATUS "  publisher shown: ${OF_MSIX_PUBLISHER_DISPLAY_NAME}")
message(STATUS "  version        : ${OF_MSIX_VERSION}")

if(OF_MSIX_PUBLISHER MATCHES "^CN=[0-9A-Fa-f]+-[0-9A-Fa-f-]+$")
    message(STATUS "  route          : Microsoft Store (publisher is a Partner Center GUID)")
    if(OF_MSIX_SIGN)
        message(WARNING
            "OF_MSIX_SIGN is ON with a Partner Center publisher identity. The Store "
            "signs submissions itself, and signtool will refuse this package because "
            "no local certificate has that subject. Turn OF_MSIX_SIGN off.")
    endif()
else()
    message(STATUS "  route          : sideload (publisher must match the signing certificate)")
endif()
