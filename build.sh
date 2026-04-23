#shell-tool build
mkdir -p build && cd build
cmake .. \
    -G Ninja \
    -DLLVM_DIR=../../llvm-project/install/lib/cmake/llvm

ninja

# Link check
#ldd ./ip-tool | grep LLVM    # For-Linux
#otool -L ./shell-tools | grep LLVM  # macOS