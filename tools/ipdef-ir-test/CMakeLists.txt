add_executable(ipdef-ir-test main.cpp)
target_include_directories(ipdef-ir-test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ipdef-ir-test
    ip_dsl ip_ir
    ${LLVM_LIBS}
)