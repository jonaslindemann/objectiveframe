if exist build-release (
    rmdir /Q /S build-release
)

mkdir build-release

pushd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
popd