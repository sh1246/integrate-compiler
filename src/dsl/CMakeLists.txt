add_library(ip_dsl
    Lexer.cpp
)
target_include_directories(ip_dsl PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)