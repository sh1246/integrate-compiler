add_executable(lexer-test main.cpp)
target_include_directories(lexer-test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(lexer-test
    ip_dsl
    ${LLVM_LIBS}
)