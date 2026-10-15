# Overlay of the upstream vcpkg port: adds a patch so ChaiScript 6.1.0 builds
# with newer libc++ (Apple clang 17+), which rejects the std::vector of the
# still-incomplete AST_Node_Trace held by exception::eval_error.
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ChaiScript/ChaiScript
    REF v6.1.0
    SHA512 7728abd8d3e1e8007f4d95c8065ba5a9154583bcd1d493bfd96d608ff91f4c9bb23e538378b77c1cfd6c637d6b4885af94ac0a2a9a34b6bae19581d9f1a831cc
    HEAD_REF master
    PATCHES
        fix-incomplete-ast-node-trace.patch
)

file(INSTALL ${SOURCE_PATH}/include/ DESTINATION ${CURRENT_PACKAGES_DIR}/include/)

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
