add_library(imnodes STATIC
    imnodes.cpp
)

target_link_libraries(imnodes PRIVATE imgui)

target_include_directories(imnodes
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)