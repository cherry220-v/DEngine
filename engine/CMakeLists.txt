cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(OpenGL_GL_PREFERENCE GLVND)

project(engine)
# -------------------------
# Options
# -------------------------
option(USE_GLFW "Use GLFW platform" ON)
option(USE_QT6 "Use QT6 platform" ON)
option(USE_OPENGL "Use OpenGL render" ON)

# -------------------------
# Qt (optional)
# -------------------------
if (USE_QT6)
    set(Qt6_DIR "C:/Qt/6.11.0/msvc2022_64/lib/cmake/Qt6")
    find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui OpenGL OpenGLWidgets)
endif()

# -------------------------
# VCPKG packages (CLEAN WAY)
# -------------------------
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)

find_path(STB_INCLUDE_DIR "stb_image.h")
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR})

add_library(engine STATIC
    tinygizmo/tiny-gizmo.hpp
    tinygizmo/tiny-gizmo.cpp

    "core/platform/window/IWindow.hpp"
    "core/platform/input/IInput.hpp"
    "core/platform/render/IRenderPlatform.hpp"

    core/Application.cpp
    core/Application.h

    core/Engine.cpp
    core/Engine.hpp

    "render/renderer/IRenderer.hpp"
    
    render/shader/Shader.cpp
    "render/shader/Shader.hpp"
    render/texture/Material.cpp
    "render/texture/Material.hpp"
    "render/mesh/Mesh.hpp"
    render/mesh/Mesh.cpp
    
    
    "ecs/components/CameraComponent.hpp"

    "render/RenderContext.hpp"

    
    "ecs/components/Transform.hpp"

    "ecs/Scene.cpp"
    "ecs/Scene.hpp"

    "ecs/components/IComponent.hpp"
    
    "models/Model.hpp"
    models/Model.cpp
    
    "scene/Skybox.hpp"
    scene/Skybox.cpp
    "geometry/Cube.hpp"
    "render/texture/Texture.hpp"
    render/texture/Texture.cpp
    
    
    math/Vertex.h
    "assets/AssetManager.hpp"
    assets/AssetManager.cpp

  "core/GUUID.h"
 "ecs/entity/Entity.hpp" "math/CameraMath.h"
 "ecs/components/PlayerComponent.hpp" 
 "ecs/systems/PlayerSystem.hpp"
 "ecs/systems/PlayerSystem.cpp"
 "ecs/systems/CameraSystem.hpp"
 "ecs/systems/CameraSystem.cpp"
 "ecs/systems/ISystem.hpp" "ecs/ComponentPool.hpp" "ecs/IComponentPool.hpp" "render/renderer/passes/IRenderPass.hpp" "render/renderer/passes/ShadowPass.cpp" "render/renderer/passes/ScenePass.cpp" "render/renderer/passes/SelectPass.cpp" "render/renderer/passes/WirePass.cpp" "render/renderer/IRenderDevice.hpp" "render/renderer/Renderer.cpp" "render/renderer/gl/OpenGLRenderDevice.cpp" "scene/particles/EffectManager.cpp" "math/Random.h" "render/renderer/passes/ParticlePass.cpp" "ecs/registry/ComponentRegistry.cpp" "ecs/registry/PropertyMeta.cpp" "ecs/systems/ParticleSystem.cpp" "ecs/systems/LightSystem.cpp" "render/renderer/passes/LightPass.cpp" "render/renderer/gpu/GPULight.cpp" "math/ShadowMath.h" "ecs/systems/AnimationSystem.cpp" "math/VertexKey.h" "models/loaders/OBJLoader.cpp" "models/loaders/GLBLoader.cpp" "assets/AssetMeta.cpp" "assets/AssetRegistry.cpp" "render/shader/loaders/ShaderLoader.cpp" "render/texture/loaders/MaterialLoader.cpp" "render/texture/loaders/TextureLoader.cpp"    "ecs/registry/GizmoRegistry.cpp" "editor/gizmos/TransformGizmo.cpp" "render/renderer/passes/EditorPass.cpp" "editor/EditorDrawList.cpp"
 #editor/gizmo/ImGuizmo.h
 #editor/gizmo/ImGuizmo.cpp
 "editor/DrawList.cpp" "editor/DrawTypes.h" "core/platform/input/InputSystem.cpp" "core/platform/input/win32/Win32Input.cpp" "core/platform/window/win32/Win32Window.cpp" "core/platform/render/win32/Win32RenderPlatform.cpp" "ecs/systems/SceneGraphSystem.cpp" "ecs/entity/EntityManager.cpp" "models/loaders/tiny_gltf.cpp" "render/texture/stb_impl.cpp" "ecs/systems/PhysicsSystem.cpp" "overlay/Gizmo.cpp" "render/renderer/passes/OverlayPass.cpp" "overlay/OverlayMath.hpp" "core/prints.h" "math/PickObject.hpp" "math/PickObject.cpp")
if (USE_GLFW)
target_sources(engine PRIVATE
    core/platform/window/glfw/GLFWWindow.hpp
    core/platform/window/glfw/GLFWWindow.cpp

    core/platform/input/glfw/GLFWInput.hpp
    core/platform/input/glfw/GLFWInput.cpp

    core/platform/render/glfw/GLFWRenderPlatform.hpp
)
endif()

if (USE_QT6)
target_sources(engine PRIVATE
    core/platform/window/qt6/QT6Window.hpp
    core/platform/window/qt6/QT6Window.cpp

    core/platform/input/qt6/QT6Input.hpp
    core/platform/input/qt6/QT6Input.cpp

    core/platform/render/qt6/QtRenderPlatform.hpp
)
endif()

 target_link_libraries(engine PRIVATE
    unofficial::omniverse-physx-sdk::sdk
)

target_link_libraries(engine PUBLIC
    nlohmann_json::nlohmann_json
    spdlog::spdlog
	glad::glad
    glm::glm
    glfw
    stb
    tinyobjloader::tinyobjloader
    OpenGL::GL
)

if (USE_QT6)
    target_link_libraries(engine PUBLIC
        Qt6::Widgets Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets
    )
endif()

target_include_directories(engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows/bin
)