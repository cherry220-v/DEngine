add_library(ImGuiColorTextEdit STATIC TextEditor.cpp)

target_link_libraries(ImGuiColorTextEdit PRIVATE imgui)

target_include_directories(ImGuiColorTextEdit
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
