
cmake_minimum_required(VERSION 3.10)
project(coretech_renderer)
file(GLOB R_SRC "src/*.cpp")
file(GLOB R_INC "include/*.h")
add_library(coretech_renderer STATIC ${R_SRC} ${R_INC})
target_include_directories(coretech_renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# link platform and GL
if(TARGET coretech_platform)
  target_link_libraries(coretech_renderer PUBLIC coretech_platform)
endif()
find_package(OpenGL REQUIRED)
target_link_libraries(coretech_renderer PUBLIC OpenGL::GL)
# link assimp if found
if(assimp_FOUND)
  target_link_libraries(coretech_renderer PUBLIC ${assimp_LIBRARIES})
  target_include_directories(coretech_renderer PUBLIC ${assimp_INCLUDE_DIRS})
endif()

# renderer extras
file(GLOB R_EXTRA "src/Shader.cpp" "src/AssetManager.cpp" "src/Frustum.cpp")
list(APPEND R_SRC ${R_EXTRA})
