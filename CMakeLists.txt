cmake_minimum_required(VERSION 3.27)
project(opengl_scene)

set(CMAKE_CXX_STANDARD 20)

set(glfw_dir ${PROJECT_SOURCE_DIR}/lib/glfw)
set(glm_dir ${PROJECT_SOURCE_DIR}/lib/glm)
set(assimp_dir ${PROJECT_SOURCE_DIR}/lib/assimp)
set(bullet3_dir ${PROJECT_SOURCE_DIR}/lib/bullet3)
set(glad_dir ${PROJECT_SOURCE_DIR}/lib/glad)
set(imgui_dir ${PROJECT_SOURCE_DIR}/lib/imgui)

set(glad_sources
        ${glad_dir}/src/gl.c
)

set(imgui_sources
        ${imgui_dir}/imgui.cpp
        ${imgui_dir}/imgui_draw.cpp
        ${imgui_dir}/imgui_widgets.cpp
        ${imgui_dir}/imgui_tables.cpp
        ${imgui_dir}/backends/imgui_impl_glfw.cpp
)

set(sources
        ${imgui_sources}
        ${glad_sources}
        src/main.cpp
        src/core/entity.cpp
        src/core/component.cpp
        src/core/scene.cpp
        src/core/window.cpp
        src/rendering/renderer.cpp
        src/core/input.cpp
        src/data/importer.cpp
        src/rendering/mesh.cpp
        src/rendering/meshRenderer.cpp
        src/core/transform.cpp
        src/rendering/camera.cpp
        src/rendering/light.cpp
        src/rendering/shader.cpp
        src/rendering/material.cpp
        src/rendering/texture.cpp
)

add_executable(opengl_scene ${sources})

include_directories(
        ${imgui_dir}
        ${glad_dir}/include
        ${glm_dir}
        ${glfw_dir}/include
        ${assimp_dir}/include
)

add_subdirectory(${glfw_dir})
add_subdirectory(${assimp_dir})
add_subdirectory(${bullet3_dir})

target_link_libraries(opengl_scene glfw)
target_link_libraries(opengl_scene assimp)
target_link_libraries(opengl_scene BulletDynamics BulletCollision LinearMath Bullet3Common)