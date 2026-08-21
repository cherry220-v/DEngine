add_library(ImGuizmo STATIC
    ImGuizmo.cpp
    GraphEditor.cpp
    ImCurveEdit.cpp
    ImGradient.cpp
    ImSequencer.cpp

)

target_link_libraries(ImGuizmo PRIVATE imgui)

target_include_directories(ImGuizmo
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
