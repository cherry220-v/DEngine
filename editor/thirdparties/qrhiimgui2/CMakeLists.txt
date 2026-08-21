cmake_minimum_required(VERSION 3.20)
project(qrhiimgui2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(QT_NO_PRIVATE_MODULE_WARNING ON)
find_package(Qt6 COMPONENTS Core Gui Quick ShaderTools OPTIONAL_COMPONENTS GuiPrivate QuickPrivate)

add_library(qrhiimgui2
    imgui/qrhiimgui.h
    imgui/qrhiimgui.cpp
    imgui/qrhiimguiitem.h
    imgui/qrhiimguiitem.cpp
)

target_link_libraries(qrhiimgui2 PRIVATE
    Qt::Core
    Qt::GuiPrivate
    Qt::QuickPrivate

    imgui
)

target_include_directories(qrhiimgui2
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

qt_add_shaders(qrhiimgui2 "shaders"
    PREFIX
        "/shaders"
    FILES
        imgui/imgui.vert
        imgui/imgui.frag
)
