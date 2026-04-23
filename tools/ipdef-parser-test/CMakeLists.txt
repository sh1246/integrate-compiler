add_executable(ipdef-parser-test main.cpp)
target_include_directories(ipdef-parser-test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ipdef-parser-test
    ip_dsl
    ${LLVM_LIBS}
)