# TetGen, built from source as the stand-alone "tetgen" program.
#
# ofsolve's TetgenBeamMesher runs tetgen as a child process and looks for it
# next to the objframe executable, which is where the shared runtime output
# directory puts this target. The macOS package bundles it from here.
#
# The source is downloaded at configure time from the pinned release archive.
# TetGen's own CMakeLists.txt is deliberately not used: it would add a library
# target and install rules to this project. The one setting from it that
# matters is reproduced below.
#
# TetGen is licensed under the AGPLv3 (see LICENSE in the downloaded source).

option(OF_BUILD_TETGEN "Download and build the tetgen mesher" ON)
if(NOT OF_BUILD_TETGEN)
    return()
endif()

include(FetchContent)

# SOURCE_SUBDIR names a directory without a CMakeLists.txt, which makes
# FetchContent_MakeAvailable() download and extract only - it never enters
# TetGen's own build.
FetchContent_Declare(tetgen_src
    URL https://github.com/TetGen/Tetgen/archive/refs/tags/v1.6.1.tar.gz
    URL_HASH SHA256=88398de3e0c16463f27686b73c7fc863df97360784466d22a24e0590886b16ec
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_SUBDIR no-cmake-build)
FetchContent_MakeAvailable(tetgen_src)

add_executable(tetgen
    "${tetgen_src_SOURCE_DIR}/tetgen.cxx"
    "${tetgen_src_SOURCE_DIR}/predicates.cxx")

# Shewchuk's exact arithmetic predicates depend on strict floating-point
# evaluation order, which optimisation can reorder or fuse. TetGen's own build
# always compiles this file unoptimised; so must this one.
if(MSVC)
    set_source_files_properties("${tetgen_src_SOURCE_DIR}/predicates.cxx"
        PROPERTIES COMPILE_OPTIONS "/Od")
else()
    set_source_files_properties("${tetgen_src_SOURCE_DIR}/predicates.cxx"
        PROPERTIES COMPILE_OPTIONS "-O0")
endif()

# Third-party code: its warnings are not ours to fix.
if(MSVC)
    target_compile_options(tetgen PRIVATE /w)
else()
    target_compile_options(tetgen PRIVATE -w)
endif()

set_target_properties(tetgen PROPERTIES FOLDER "External")

# Building the application builds the mesher it runs.
add_dependencies(objframe tetgen)
