add_library(ip_dsl
    Lexer.cpp
    AST.cpp
    ParserBase.cpp
    IPDefParser.cpp
    #FabricParser.cpp
    #SoCParser.cpp
)
target_include_directories(ip_dsl PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)