add_library(ip_ir
    EncoderBase.cpp
    IPDefEncoder.cpp
    # FabricEncoder.cpp 
    # SoCEncoder.cpp 
)
target_include_directories(ip_ir PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ip_ir ip_dsl ${LLVM_LIBS})