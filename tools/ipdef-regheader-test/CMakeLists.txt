add_executable(ipdef-regheader-test main.cpp)
target_include_directories(ipdef-regheader-test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ipdef-regheader-test
    ip_dsl ip_ir ip_backends
    ${LLVM_LIBS}
)