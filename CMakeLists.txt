cmake_minimum_required(VERSION 3.12)
project(CoreTechEnhanced)

# --- Assistant-added: enforce modern C++ and include dirs for IDE friendliness ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()
include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/tools/gameplay/include")

option(FETCH_DEPS "Enable fetching external dependencies" OFF)
if(NOT FETCH_DEPS)
  function(FetchContent_Declare)
  endfunction()
  function(FetchContent_MakeAvailable)
  endfunction()
endif()

include(cmake/FetchDependencies.cmake) # existing helper (no-op if FETCH_DEPS=OFF)

# Collect all source files under src/
file(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c")

# Partition sources by subsystem based on path
set(CORE_SOURCES "")
set(RENDERER_SOURCES "")
set(ECS_SOURCES "")
set(PLATFORM_SOURCES "")
set(GAMEPLAY_SOURCES "")
set(TOOLS_SOURCES "")

foreach(srcfile ${ALL_SRC})
  # Normalize Windows backslashes to Unix-style slashes for MATCHES
  string(REPLACE "\\" "/" srcfile_unix "${srcfile}")
  if(srcfile_unix MATCHES "/src/ecs/")
    list(APPEND ECS_SOURCES "${srcfile}")
  elseif(srcfile_unix MATCHES "/src/Renderer/" OR srcfile_unix MATCHES "/src/GBuffer.cpp" OR srcfile_unix MATCHES "/src/Renderer.cpp" )
    list(APPEND RENDERER_SOURCES "${srcfile}")
  elseif(srcfile_unix MATCHES "/src/Engine/" OR srcfile_unix MATCHES "/src/core/" OR srcfile_unix MATCHES "/src/Core/")
    list(APPEND CORE_SOURCES "${srcfile}")
  elseif(srcfile_unix MATCHES "/src/platform/" OR srcfile_unix MATCHES "/src/Launcher")
    list(APPEND PLATFORM_SOURCES "${srcfile}")
  elseif(srcfile_unix MATCHES "/src/gameplay/" )
    list(APPEND GAMEPLAY_SOURCES "${srcfile}")
  else()
    # conservative: treat as core
    list(APPEND CORE_SOURCES "${srcfile}")
  endif()
endforeach()

# Create modular static libraries
if(CORE_SOURCES)
  add_library(core_lib STATIC ${CORE_SOURCES})
  target_include_directories(core_lib PUBLIC "${CMAKE_SOURCE_DIR}/include")
endif()
if(RENDERER_SOURCES)
  add_library(renderer_lib STATIC ${RENDERER_SOURCES})
  target_include_directories(renderer_lib PUBLIC "${CMAKE_SOURCE_DIR}/include")
  target_link_libraries(renderer_lib PUBLIC core_lib)
endif()
if(ECS_SOURCES)
  add_library(ecs_lib STATIC ${ECS_SOURCES})
  target_include_directories(ecs_lib PUBLIC "${CMAKE_SOURCE_DIR}/include")
  target_link_libraries(ecs_lib PUBLIC core_lib)
endif()
if(PLATFORM_SOURCES)
  add_library(platform_lib STATIC ${PLATFORM_SOURCES})
  target_include_directories(platform_lib PUBLIC "${CMAKE_SOURCE_DIR}/include")
  target_link_libraries(platform_lib PUBLIC core_lib renderer_lib ecs_lib)
endif()

# Gameplay/tools remain unbuilt as libraries, but can be added as needed
# Create executable from src/main.cpp if present
if(EXISTS "${CMAKE_SOURCE_DIR}/src/main.cpp")
  add_executable(CoreTechApp src/main.cpp)
  # link libs if present
  if(TARGET core_lib)
    target_link_libraries(CoreTechApp PRIVATE core_lib)
  endif()
  if(TARGET ecs_lib)
    target_link_libraries(CoreTechApp PRIVATE ecs_lib)
  endif()
  if(TARGET renderer_lib)
    target_link_libraries(CoreTechApp PRIVATE renderer_lib)
  endif()
  if(TARGET platform_lib)
    target_link_libraries(CoreTechApp PRIVATE platform_lib)
  endif()
  target_include_directories(CoreTechApp PRIVATE "${CMAKE_SOURCE_DIR}/include")
endif()

# Allow users to opt into warnings-as-errors
option(ENABLE_WERROR "Treat warnings as errors" OFF)
if(ENABLE_WERROR)
  if(MSVC)
    add_compile_options(/WX)
  else()
    add_compile_options(-Werror)
  endif()
endif()

# Provide a simple Export target for downstream projects
export(TARGETS core_lib renderer_lib ecs_lib FILE "${CMAKE_BINARY_DIR}/CoreTechTargets.cmake")


# --- Assistant-added options for optional subsystems ---
option(ENABLE_OPENGL "Enable OpenGL renderer" OFF)
option(ENABLE_GLFW "Enable GLFW windowing" OFF)
option(ENABLE_BULLET "Enable Bullet physics integration" OFF)
option(ENABLE_OPENAL "Enable OpenAL audio" OFF)
option(ENABLE_IMGUI "Enable ImGui editor/debug UI" OFF)

if(ENABLE_GLFW)
  find_package(GLFW3 QUIET)
  if(GLFW3_FOUND)
    message(STATUS "GLFW found")
    add_compile_definitions(ENABLE_GLFW=1)
  else()
    message(STATUS "GLFW not found; set ENABLE_GLFW=OFF or provide GLFW library")
  endif()
endif()

if(ENABLE_OPENGL)
  find_package(OpenGL REQUIRED)
  if(OPENGL_FOUND)
    message(STATUS "OpenGL found")
    add_compile_definitions(ENABLE_OPENGL=1)
  endif()
endif()

if(ENABLE_BULLET)
  find_package(Bullet QUIET)
  if(BULLET_FOUND)
    message(STATUS "Bullet found")
    add_compile_definitions(ENABLE_BULLET=1)
  else()
    message(STATUS "Bullet not found; physics module will be disabled")
  endif()
endif()

if(ENABLE_OPENAL)
  find_package(OpenAL QUIET)
  if(OpenAL_FOUND)
    message(STATUS "OpenAL found")
    add_compile_definitions(ENABLE_OPENAL=1)
  else()
    message(STATUS "OpenAL not found; audio module will be disabled")
  endif()
endif()

if(ENABLE_IMGUI)
  # ImGui will be fetched if FETCH_DEPS=ON, otherwise user must supply it
  if(FETCH_DEPS)
    include(cmake/FetchImGui.cmake)
  endif()
  add_compile_definitions(ENABLE_IMGUI=1)
endif()

# Link optional libraries to renderer/platform targets if they exist
if(TARGET renderer_lib)
  if(ENABLE_GLFW AND GLFW3_FOUND)
    target_link_libraries(renderer_lib PUBLIC GLFW::GLFW)
  endif()
  if(ENABLE_OPENGL AND OPENGL_FOUND)
    target_link_libraries(renderer_lib PUBLIC OpenGL::GL)
  endif()
endif()

if(TARGET platform_lib)
  if(ENABLE_GLFW AND GLFW3_FOUND)
    target_link_libraries(platform_lib PUBLIC GLFW::GLFW)
  endif()
endif()


# If tools/renderer exists, include it into renderer_lib
if(EXISTS "${CMAKE_SOURCE_DIR}/tools/renderer")
  file(GLOB_RECURSE TOOLS_RENDERER_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tools/renderer/src/*.cpp" "${CMAKE_SOURCE_DIR}/tools/renderer/include/*.h")
  if(TARGET renderer_lib)
    target_sources(renderer_lib PRIVATE ${TOOLS_RENDERER_SRC})
    target_include_directories(renderer_lib PUBLIC "${CMAKE_SOURCE_DIR}/tools/renderer/include")
  endif()
endif()


# --- FetchContent for common deps (only when FETCH_DEPS=ON) ---
include(FetchContent)
if(FETCH_DEPS)
  message(STATUS "FETCH_DEPS=ON: attempting to download third-party dependencies")

  FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(glad)

  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
  )
  FetchContent_MakeAvailable(glfw)

  FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
  )
  FetchContent_MakeAvailable(glm)

  FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(stb)

  FetchContent_Declare(
    bullet
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(bullet)

  FetchContent_Declare(
    openal
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(openal)

  # Link targets where appropriate if found
  if(TARGET glad)
    target_include_directories(renderer_lib PUBLIC ${glad_SOURCE_DIR}/include)
  endif()
  if(TARGET glfw)
    target_link_libraries(renderer_lib PUBLIC glfw)
  endif()
  if(TARGET glm)
    target_include_directories(renderer_lib PUBLIC ${glm_SOURCE_DIR})
  endif()
endif()


# Fetch Catch2 for tests when FETCH_DEPS=ON
if(FETCH_DEPS)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.7
  )
  FetchContent_MakeAvailable(catch2)
endif()

# Add tests target
enable_testing()
file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/*.cpp")
add_executable(engine_tests ${TEST_SRC})
target_include_directories(engine_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
if(TARGET core_lib)
  target_link_libraries(engine_tests PRIVATE core_lib)
endif()
if(TARGET renderer_lib)
  target_link_libraries(engine_tests PRIVATE renderer_lib)
endif()
if(TARGET ecs_lib)
  target_link_libraries(engine_tests PRIVATE ecs_lib)
endif()
if(TARGET catch2)
  target_link_libraries(engine_tests PRIVATE Catch2::Catch2)
endif()
add_test(NAME engine_tests COMMAND engine_tests)

option(ENABLE_VULKAN "Enable Vulkan renderer" OFF)
if(ENABLE_VULKAN)
  add_compile_definitions(ENABLE_VULKAN=1)
endif()


# Quick tests target (no external deps)
file(GLOB QUICK_TEST_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/quick_tests.cpp")
add_executable(quick_tests ${QUICK_TEST_SRC})
target_include_directories(quick_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
if(TARGET core_lib)
  target_link_libraries(quick_tests PRIVATE core_lib)
endif()
if(TARGET renderer_lib)
  target_link_libraries(quick_tests PRIVATE renderer_lib)
endif()
if(TARGET ecs_lib)
  target_link_libraries(quick_tests PRIVATE ecs_lib)
endif()