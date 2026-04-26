cmake_minimum_required(VERSION 3.20)
project(shell-tools C CXX)
set(CMAKE_CXX_STANDARD 17)

#Specify local build LLVM path
find_package(LLVM 17 REQUIRED CONFIG
  PATHS "${CMAKE_SOURCE_DIR}/../llvm-project/install/lib/cmake/llvm"
  NO_DEFAULT_PATH
)

message(STATUS "LLVM version : ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM install : ${LLVM_INSTALL_PREFIX}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

if(NOT LLVM_ENABLE_RTTI)
    add_compile_options(-fno-rtti)
endif()

llvm_map_components_to_libnames(LLVM_LIBS
    Core        # Module, Function, Metadata, IRBuilder
    Support     # StringRef, raw_ostream, Error
    IRReader    # IR read
    BitWriter   # .bc dump
)
add_subdirectory(src/dsl)
add_subdirectory(src/ir)
add_subdirectory(tools/lexer-test)
add_subdirectory(tools/ipdef-parser-test)
add_subdirectory(tools/ipdef-ir-test)