# Stages the ObjectiveFrame payload and packs it into an MSIX.
#
# Run in script mode (cmake -P) by the "msix" target so that the whole layout
# is rebuilt from scratch on every run - a stale file left in the layout would
# otherwise be shipped forever. Every path comes in as a -D argument; nothing
# here is resolved from a build tree.
#
#   OF_SOURCE_DIR     repository root
#   OF_EXE            full path to the built objframe.exe
#   OF_LAYOUT_DIR     directory to stage into (wiped first)
#   OF_MANIFEST       configured AppxManifest.xml
#   OF_ASSETS_DIR     directory holding the generated tile PNGs
#   OF_OUTPUT         .msix file to write
#   OF_MAKEAPPX       makeappx.exe
#   OF_RUNTIME_LIBS   '|'-separated MSVC runtime DLLs to bundle (may be empty)
#   OF_SIGNTOOL       signtool.exe, or empty to leave the package unsigned
#   OF_SIGN_CERT      .pfx file    ) one of these two selects the certificate
#   OF_SIGN_SHA1      cert thumbprint in the local store
#   OF_SIGN_PASSWORD  password for the .pfx (optional)
#   OF_SIGN_TIMESTAMP timestamp server URL (optional)

cmake_minimum_required(VERSION 3.24)

get_filename_component(_exe_name "${OF_EXE}" NAME)
if(NOT _exe_name STREQUAL "objframe.exe")
    message(FATAL_ERROR
        "The MSIX manifest declares objframe.exe as its entry point, but this "
        "configuration builds ${_exe_name}. Package a Release build:\n"
        "    cmake --build <build dir> --target msix --config Release")
endif()

if(NOT EXISTS "${OF_EXE}")
    message(FATAL_ERROR "objframe.exe not found at ${OF_EXE} - build the objframe target first.")
endif()

# ---------------------------------------------------------------- staging ---

message(STATUS "Staging MSIX payload in ${OF_LAYOUT_DIR}")

file(REMOVE_RECURSE "${OF_LAYOUT_DIR}")
file(MAKE_DIRECTORY "${OF_LAYOUT_DIR}")

# The application and whatever vcpkg copied next to it.
get_filename_component(_bin_dir "${OF_EXE}" DIRECTORY)
file(GLOB _dlls "${_bin_dir}/*.dll")
file(COPY "${OF_EXE}" ${_dlls} DESTINATION "${OF_LAYOUT_DIR}")

# The MSVC redistributable cannot be run as an installer from inside a package,
# so the runtime is deployed app-local instead.
if(OF_RUNTIME_LIBS)
    string(REPLACE "|" ";" _runtime_libs "${OF_RUNTIME_LIBS}")
    file(COPY ${_runtime_libs} DESTINATION "${OF_LAYOUT_DIR}")
endif()

# The mesher, invoked as a child process by ofsolve.
if(EXISTS "${OF_SOURCE_DIR}/external/tetgen.exe")
    file(COPY "${OF_SOURCE_DIR}/external/tetgen.exe" DESTINATION "${OF_LAYOUT_DIR}")
else()
    message(WARNING "external/tetgen.exe is missing - meshing will not work in this package.")
endif()

# Runtime data, laid out next to the executable the way FemViewWindow::setProgramPath expects.
foreach(_dir fonts images maps plugins python examples ai)
    if(EXISTS "${OF_SOURCE_DIR}/bin/${_dir}")
        file(COPY "${OF_SOURCE_DIR}/bin/${_dir}" DESTINATION "${OF_LAYOUT_DIR}")
    else()
        message(WARNING "bin/${_dir} is missing - it will not be in the package.")
    endif()
endforeach()

file(COPY "${OF_ASSETS_DIR}/" DESTINATION "${OF_LAYOUT_DIR}/Assets")
file(COPY_FILE "${OF_MANIFEST}" "${OF_LAYOUT_DIR}/AppxManifest.xml")

# ---------------------------------------------------------------- packing ---

get_filename_component(_out_dir "${OF_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_out_dir}")

message(STATUS "Packing ${OF_OUTPUT}")

execute_process(
    COMMAND "${OF_MAKEAPPX}" pack /o /d "${OF_LAYOUT_DIR}" /p "${OF_OUTPUT}"
    COMMAND_ERROR_IS_FATAL ANY)

# ---------------------------------------------------------------- signing ---

if(NOT OF_SIGNTOOL)
    message(STATUS
        "Package is unsigned. Windows will not install it until it is signed "
        "with a certificate trusted on the target machine; see install/msix/README.md.")
    return()
endif()

set(_sign_args sign /fd SHA256)

if(OF_SIGN_CERT)
    list(APPEND _sign_args /f "${OF_SIGN_CERT}")
    if(OF_SIGN_PASSWORD)
        list(APPEND _sign_args /p "${OF_SIGN_PASSWORD}")
    endif()
elseif(OF_SIGN_SHA1)
    list(APPEND _sign_args /sha1 "${OF_SIGN_SHA1}")
else()
    message(FATAL_ERROR
        "OF_MSIX_SIGN is on but neither OF_MSIX_SIGN_CERT nor OF_MSIX_SIGN_SHA1 is set.")
endif()

if(OF_SIGN_TIMESTAMP)
    list(APPEND _sign_args /tr "${OF_SIGN_TIMESTAMP}" /td SHA256)
endif()

message(STATUS "Signing ${OF_OUTPUT}")

execute_process(
    COMMAND "${OF_SIGNTOOL}" ${_sign_args} "${OF_OUTPUT}"
    COMMAND_ERROR_IS_FATAL ANY)
