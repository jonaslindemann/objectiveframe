if exist build-debug (
    rmdir /Q /S build-debug
)

mkdir build-debug

pushd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
popd