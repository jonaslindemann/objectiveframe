# Stages ObjectiveFrame.app, signs it and packs it into an installer package.
#
# Run in script mode (cmake -P) by the "pkg" target so that the bundle is
# rebuilt from scratch on every run - a stale file left in it would otherwise
# be shipped forever, and any file changed after signing breaks the signature.
# Every path comes in as a -D argument; nothing here is resolved from a build
# tree.
#
#   OF_SOURCE_DIR       repository root
#   OF_EXE              full path to the built objframe executable
#   OF_WORK_DIR         scratch directory (wiped first)
#   OF_INFO_PLIST       configured Info.plist
#   OF_ICNS             rendered app icon
#   OF_COMPONENT_PLIST  pkgbuild component property list
#   OF_DISTRIBUTION     configured productbuild distribution file
#   OF_OUTPUT           .pkg file to write
#   OF_BUNDLE_ID        bundle and package identifier
#   OF_VERSION          package version
#   OF_TETGEN           tetgen executable to bundle (may be empty)
#   OF_SIGN_APP         'Developer ID Application' identity, empty for ad-hoc
#   OF_SIGN_INSTALLER   'Developer ID Installer' identity, empty for unsigned
#   OF_NOTARY_PROFILE   notarytool keychain profile, empty to skip
#   OF_CODESIGN         codesign
#   OF_PKGBUILD         pkgbuild
#   OF_PRODUCTBUILD     productbuild

cmake_minimum_required(VERSION 3.24)

get_filename_component(_exe_name "${OF_EXE}" NAME)
if(NOT _exe_name STREQUAL "objframe")
    message(FATAL_ERROR
        "Info.plist names objframe as the bundle executable, but this "
        "configuration builds ${_exe_name}. Package a Release build.")
endif()

if(NOT EXISTS "${OF_EXE}")
    message(FATAL_ERROR "objframe not found at ${OF_EXE} - build the objframe target first.")
endif()

function(of_run)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE _result)
    if(NOT _result EQUAL 0)
        list(GET ARGN 0 _tool)
        get_filename_component(_tool "${_tool}" NAME)
        message(FATAL_ERROR "${_tool} failed (${_result})")
    endif()
endfunction()

# ---------------------------------------------------------------- staging ---

set(_root "${OF_WORK_DIR}/root")
set(_app "${_root}/ObjectiveFrame.app")
set(_contents "${_app}/Contents")

message(STATUS "Staging ObjectiveFrame.app in ${_root}")

file(REMOVE_RECURSE "${OF_WORK_DIR}")
file(MAKE_DIRECTORY "${_contents}/MacOS" "${_contents}/Resources")

file(COPY "${OF_EXE}" DESTINATION "${_contents}/MacOS")
file(COPY_FILE "${OF_INFO_PLIST}" "${_contents}/Info.plist")
file(WRITE "${_contents}/PkgInfo" "APPL????")
file(COPY_FILE "${OF_ICNS}" "${_contents}/Resources/ObjectiveFrame.icns")

# Data the application loads at run time. FemViewWindow::setProgramPath()
# looks for it in Contents/Resources when the executable sits in a bundle.
foreach(_dir fonts images maps plugins python examples ai)
    if(NOT EXISTS "${OF_SOURCE_DIR}/bin/${_dir}")
        message(FATAL_ERROR "bin/${_dir} is missing - it is part of the application payload.")
    endif()
    file(COPY "${OF_SOURCE_DIR}/bin/${_dir}" DESTINATION "${_contents}/Resources"
         PATTERN ".DS_Store" EXCLUDE)
endforeach()

# The mesher, invoked as a child process by ofsolve. It goes next to the
# executable, which is where the mesher looks first.
if(OF_TETGEN)
    if(NOT EXISTS "${OF_TETGEN}")
        message(FATAL_ERROR "OF_PKG_TETGEN points at ${OF_TETGEN}, which does not exist.")
    endif()
    file(COPY_FILE "${OF_TETGEN}" "${_contents}/MacOS/tetgen")
    file(CHMOD "${_contents}/MacOS/tetgen"
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
                     WORLD_READ WORLD_EXECUTE)
else()
    message(WARNING "No tetgen bundled (OF_BUILD_TETGEN off, OF_PKG_TETGEN empty) - "
                    "meshing will not work in this package.")
endif()

# --------------------------------------------------------------- code sign ---

# Notarization requires the hardened runtime and a secure timestamp on every
# executable. Nested code is signed before the bundle that contains it; the
# bundle signature then seals it. No --deep: it is deprecated for signing and
# would apply the app's options to everything inside.
if(OF_SIGN_APP)
    set(_sign_args --force --options runtime --timestamp --sign "${OF_SIGN_APP}")
    message(STATUS "Signing with '${OF_SIGN_APP}'")
else()
    set(_sign_args --force --sign -)
    message(STATUS "Ad-hoc signing (no OF_PKG_SIGN_APP) - for local testing only")
endif()

if(OF_TETGEN)
    of_run("${OF_CODESIGN}" ${_sign_args} --identifier "${OF_BUNDLE_ID}.tetgen"
           "${_contents}/MacOS/tetgen")
endif()
of_run("${OF_CODESIGN}" ${_sign_args} "${_app}")
of_run("${OF_CODESIGN}" --verify --strict --deep --verbose=2 "${_app}")

# ----------------------------------------------------------------- package ---

set(_packages "${OF_WORK_DIR}/packages")
file(MAKE_DIRECTORY "${_packages}")

of_run("${OF_PKGBUILD}"
       --root "${_root}"
       --component-plist "${OF_COMPONENT_PLIST}"
       --identifier "${OF_BUNDLE_ID}"
       --version "${OF_VERSION}"
       --install-location /Applications
       "${_packages}/ObjectiveFrame-component.pkg")

get_filename_component(_output_dir "${OF_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_output_dir}")
file(REMOVE "${OF_OUTPUT}")

set(_product_args --distribution "${OF_DISTRIBUTION}" --package-path "${_packages}")
if(OF_SIGN_INSTALLER)
    list(APPEND _product_args --sign "${OF_SIGN_INSTALLER}" --timestamp)
endif()
of_run("${OF_PRODUCTBUILD}" ${_product_args} "${OF_OUTPUT}")

# ------------------------------------------------------------- notarize ------

if(OF_NOTARY_PROFILE)
    message(STATUS "Submitting for notarization - this usually takes a few minutes")

    # notarytool exits 0 for a submission Apple processed but rejected, so the
    # verdict is read from the output rather than the exit code.
    execute_process(
        COMMAND xcrun notarytool submit "${OF_OUTPUT}"
                --keychain-profile "${OF_NOTARY_PROFILE}" --wait
        OUTPUT_VARIABLE _notary_out
        ERROR_VARIABLE _notary_err
        RESULT_VARIABLE _notary_result)
    message(STATUS "${_notary_out}${_notary_err}")

    if(NOT _notary_result EQUAL 0 OR NOT _notary_out MATCHES "status: Accepted")
        string(REGEX MATCH "id: [0-9a-f-]+" _notary_id "${_notary_out}")
        message(FATAL_ERROR
            "Notarization did not succeed. For Apple's reasons run:\n"
            "    xcrun notarytool log <${_notary_id}> --keychain-profile ${OF_NOTARY_PROFILE}")
    endif()

    of_run(xcrun stapler staple "${OF_OUTPUT}")
endif()

message(STATUS "Package written to ${OF_OUTPUT}")
