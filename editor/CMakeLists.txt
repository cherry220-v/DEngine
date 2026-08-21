cmake_minimum_required(VERSION 3.16)
set(Qt6_DIR "C:/Qt/6.11.0/msvc2022_64/lib/cmake/Qt6")

# https://github.com/paceholder/nodeeditor

project(ui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOQRC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui OpenGL OpenGLWidgets GuiPrivate Quick Qml)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    assets/resources.qrc
    imguidemo.qrc
)

qt_add_executable(ui
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    mainwindow.ui
    assets/resources.qrc
    glwidget.h glwidget.cpp

    engine/EditorApp.hpp
    engine/EditorApp.cpp

    widgets/HierarchyObject.h
    widgets/HierarchyObject.cpp

    widgets/ObjHierarchyView.h
    widgets/ObjHierarchyView.cpp

    widgets/CustomDock.h
    widgets/CustomDock.cpp

    widgets/hierarchy/model/HierarchyNode.hpp
    widgets/hierarchy/model/SceneHierarchyModel.hpp
    widgets/hierarchy/model/SceneHierarchyModel.cpp

    widgets/hierarchy/tools/HierarchyBuilder.hpp
    widgets/hierarchy/tools/HierarchyBuilder.cpp

    widgets/hierarchy/view/HierarchyTreeWidget.hpp
    widgets/hierarchy/view/HierarchyTreeWidget.cpp

    widgets/hierarchy/commands/SceneCommands.hpp
    widgets/hierarchy/commands/SceneCommands.cpp

    widgets/sceneview/imguiii.cpp
    widgets/sceneview/imguiii.h
)

add_subdirectory(thirdparties)

target_link_libraries(ui PRIVATE Qt6::Widgets Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets Qt6::GuiPrivate Qt6::Quick Qt6::Qml)

target_link_libraries(ui PUBLIC engine)

target_link_libraries(ui PRIVATE qrhiimgui2)

target_include_directories(ui PRIVATE "engine")


set_target_properties(ui PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_include_directories(ui
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

qt_finalize_executable(ui)
