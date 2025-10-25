# --- Assistant-added dependency find/link helpers ---
include(cmake/FetchDependencies.cmake) # FetchContent (will be skipped if prebuilt present)

# Prefer prebuilt thirdparty in thirdparty/prebuilt/
set(THIRDPARTY_PREBUILT_DIR "${CMAKE_SOURCE_DIR}/thirdparty/prebuilt")
if(EXISTS "${THIRDPARTY_PREBUILT_DIR}")
  message(STATUS "Using thirdparty prebuilt artifacts in ${THIRDPARTY_PREBUILT_DIR}")
  include_directories("${THIRDPARTY_PREBUILT_DIR}/include" "${THIRDPARTY_PREBUILT_DIR}/include64")
  link_directories("${THIRDPARTY_PREBUILT_DIR}/lib" "${THIRDPARTY_PREBUILT_DIR}/lib64")
  # You may need to adjust target_link_libraries entries below to match your prebuilt layout.
endif()

# Try to find common packages (Assimp, SDL2, OpenAL, Bullet, OpenGL)
find_package(assimp QUIET)
find_package(SDL2 QUIET)
find_package(OpenAL QUIET)
find_package(Bullet QUIET)
find_package(OpenGL QUIET)

# Example: if found, expose variables for targets to use
if(assimp_FOUND)
  message(STATUS "Assimp found: ${assimp_INCLUDE_DIRS}")
  include_directories(${assimp_INCLUDE_DIRS})
endif()
if(SDL2_FOUND)
  message(STATUS "SDL2 found: ${SDL2_INCLUDE_DIRS}")
  include_directories(${SDL2_INCLUDE_DIRS})
  link_libraries(${SDL2_LIBRARIES})
endif()
# --- end assistant block ---

# --- Prebuilt thirdparty detection block (added by assistant) ---
set(THIRDPARTY_PREBUILT_DIR "${CMAKE_SOURCE_DIR}/thirdparty/prebuilt")
if(EXISTS "${THIRDPARTY_PREBUILT_DIR}")
  message(STATUS "Using thirdparty prebuilt artifacts in ${THIRDPARTY_PREBUILT_DIR}")
  # Example: add a path to link directories or include directories if libs are provided.
  # You may need to adapt these variables to match the layout of your prebuilt zips.
  link_directories("${THIRDPARTY_PREBUILT_DIR}/lib" "${THIRDPARTY_PREBUILT_DIR}/lib64")
  include_directories("${THIRDPARTY_PREBUILT_DIR}/include")
endif()
# --- end block ---

include(cmake/FetchDependencies.cmake)\n\nset(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.10)
project(CoreTechEnhanced)

option(ENABLE_PROFILER "Enable profiler overlay" OFF)
option(ENABLE_VS_ENHANCED "Enable enhanced visual scripting" ON)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp" "src/*.c")
if(ENABLE_VS_ENHANCED)
  list(APPEND SRC_FILES "src/visualscripting/EnhancedVisualScripting.cpp" "src/visualscripting/Variant.cpp")
endif()

add_executable(CoreTechDemo ${SRC_FILES})
target_include_directories(CoreTechDemo PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/third_party)

# link platform libraries as needed (user should adjust)
if (WIN32)
  target_compile_definitions(CoreTechDemo PRIVATE PLATFORM_WINDOWS)
endif()

# Linking hints (auto-detect where possible)
find_package(OpenGL)
if (OPENGL_FOUND)
  target_link_libraries(CoreTechDemo PRIVATE OpenGL::GL)
endif()
find_package(Threads)
if (Threads_FOUND)
  target_link_libraries(CoreTechDemo PRIVATE Threads::Threads)
endif()
# ImGui single-file is compiled from third_party/imgui_single.cpp; ensure include path is set.


# Detect Bullet headers; if missing, disable physics compilation to allow building without Bullet.
find_path(BULLET_INCLUDE btBulletDynamicsCommon.h HINTS ${CMAKE_SOURCE_DIR}/third_party/bullet /usr/include /usr/local/include)
if (NOT BULLET_INCLUDE)
  message(STATUS "Bullet headers not found; disabling physics sources (DISABLE_PHYSICS=ON).")
  set(DISABLE_PHYSICS ON)
else()
  message(STATUS "Bullet headers found: ${BULLET_INCLUDE}")
endif()

if (DISABLE_PHYSICS)
  # remove physics-related sources to avoid compile errors without Bullet
  list(REMOVE_ITEM SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/PhysicsManager.cpp
    ${CMAKE_SOURCE_DIR}/src/PhysicsComponent.cpp
    ${CMAKE_SOURCE_DIR}/src/NavMesh.cpp
    )
  add_definitions(-DDISABLE_PHYSICS)
endif()

# --- Add Assimp demo target ---
option(BUILD_ASSIMP_DEMO "Build Assimp skinned mesh importer demo" ON)
if(BUILD_ASSIMP_DEMO)
  add_subdirectory(demo_assimp)
endif()
# --- end Assimp demo block ---


# --- FileFormatSupport integration (added) ---
add_library(FileFormatSupport STATIC src/FileFormatSupport.cpp)
target_include_directories(FileFormatSupport PUBLIC ${CMAKE_SOURCE_DIR}/include)
# Try to link with Assimp if available
find_package(assimp QUIET)
if(assimp_FOUND)
  target_link_libraries(FileFormatSupport PUBLIC assimp::assimp)
endif()
# Add header-only or small libs via FetchContent
include(FetchContent)
FetchContent_Declare(tinyobjloader GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git GIT_TAG v2.0.0)
FetchContent_Declare(tinygltf GIT_REPOSITORY https://github.com/syoyo/tinygltf.git GIT_TAG master)
FetchContent_Declare(miniaudio GIT_REPOSITORY https://github.com/mackron/miniaudio.git GIT_TAG v0.12.13)
FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG master)
FetchContent_MakeAvailable(tinyobjloader tinygltf miniaudio stb)
# tinyobj provides tinyobj::tinyobj_loader_cc depending on CMake config
if(TARGET tinyobjloader)
  target_link_libraries(FileFormatSupport PUBLIC tinyobjloader)
endif()
if(TARGET tinygltf)
  target_link_libraries(FileFormatSupport PUBLIC tinygltf)
endif()
if(TARGET miniaudio)
  target_link_libraries(FileFormatSupport PUBLIC miniaudio)
endif()
# Expose stb include directory for stb_image.h
if(TARGET stb)
  target_include_directories(FileFormatSupport PUBLIC ${stb_SOURCE_DIR})
endif()
# --- end FileFormatSupport integration ---

# --- ToonIntegration optional module (added) ---
if(EXISTS "${CMAKE_SOURCE_DIR}/include/Renderer.h")
  message(STATUS "Renderer.h detected — adding ToonIntegration module")
  add_library(ToonIntegration STATIC src/ToonIntegration.cpp)
  target_include_directories(ToonIntegration PUBLIC ${CMAKE_SOURCE_DIR}/include)
  # Link to ImGui if available
  find_package(imgui QUIET)
  if(imgui_FOUND)
    target_link_libraries(ToonIntegration PUBLIC imgui::imgui)
  endif()
  # Link to renderer target if exists
  if(TARGET Renderer)
    target_link_libraries(ToonIntegration PUBLIC Renderer)
  endif()
else()
  message(STATUS "Renderer.h not found — ToonIntegration will be built as stub (no-op).")
endif()
# --- end ToonIntegration block ---


# --- Optimization systems (added) ---
add_library(CoreTechOptimizations STATIC
  src/FrustumCuller.cpp
  src/Octree.cpp
  src/BVH.cpp
  src/HZBOcclusion.cpp
  src/LODManager.cpp
  src/GPUInstancing.cpp
  src/RenderGraph.cpp
  src/JobSystem.cpp
  src/ShaderCache.cpp
  src/TextureStreaming.cpp
  src/MeshOptimizer.cpp
  src/OptimizationPanels.cpp
)
target_include_directories(CoreTechOptimizations PUBLIC ${CMAKE_SOURCE_DIR}/include)
# Link to imgui if present (for panels)
find_package(imgui QUIET)
if(imgui_FOUND)
  target_link_libraries(CoreTechOptimizations PUBLIC imgui::imgui)
endif()
# Link to renderer if available
if(TARGET Renderer)
  target_link_libraries(CoreTechOptimizations PUBLIC Renderer)
endif()
# --- end optimizations block ---

add_subdirectory(tools)

# --- QEM LOD & Editor ---
target_sources(CoreTechOptimizations PRIVATE src/QEMSimplifier.cpp src/LODEditor.cpp src/LODGenerator.cpp)
target_include_directories(CoreTechOptimizations PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_subdirectory(tools)

# QEM optimized and LOD runtime
target_sources(CoreTechOptimizations PRIVATE src/QEMOptimized.cpp src/LODRuntime.cpp src/LODRuntime.cpp src/QEMSimplifier.cpp src/LODEditor.cpp)


# --- Assistant-added OpenGL/GLFW link block ---
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

if(TARGET CoreTechDemo)
    target_link_libraries(CoreTechDemo PRIVATE glfw OpenGL::GL)
endif()
if(TARGET CoreTechOptimizations)
    target_link_libraries(CoreTechOptimizations PRIVATE glfw OpenGL::GL)
endif()
# --- end OpenGL/GLFW block ---


# Add GLAD download and library
add_subdirectory(thirdparty/glad)

if(TARGET CoreTechDemo)
  target_link_libraries(CoreTechDemo PRIVATE coretech_glad)
endif()

# --- Assistant-added Assimp & stb_image support ---
add_subdirectory(thirdparty/stb_image)
if(TARGET CoreTechDemo)
    target_link_libraries(CoreTechDemo PRIVATE assimp::assimp coretech_stb_image)
endif()
if(TARGET CoreTechOptimizations)
    target_link_libraries(CoreTechOptimizations PRIVATE assimp::assimp coretech_stb_image)
endif()
# --- end Assimp/stb_image block ---
