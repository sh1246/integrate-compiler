add_library(ip_backends
    RegHeaderBackend.cpp
    # QEMUBackend.cpp 
    # SVBackend.cpp
)
target_include_directories(ip_backends PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ip_backends
    ip_ir
    ${LLVM_LIBS}
)