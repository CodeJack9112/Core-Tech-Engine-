cmake_minimum_required(VERSION 3.10)
project(coretech_game_app)
file(GLOB SRC "src/*.cpp" "include/*.h")
add_library(coretech_game_app STATIC ${SRC})
target_include_directories(coretech_game_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Link platform interface
if(TARGET coretech_platform)
  target_link_libraries(coretech_game_app PUBLIC coretech_platform)
endif()
# Link GL (for OpenGL calls)
find_package(OpenGL REQUIRED)
target_link_libraries(coretech_game_app PUBLIC OpenGL::GL)

# Build and link ImGui implementation (use imgui source from FetchContent)
if(TARGET imgui)
  # compile ImGui sources directly into a static target for this app
  add_library(imgui_impl STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  )
  target_include_directories(imgui_impl PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
  target_link_libraries(coretech_game_app PRIVATE imgui_impl)
endif()

# link renderer
if(TARGET coretech_renderer)
  target_link_libraries(coretech_game_app PUBLIC coretech_renderer)
endif()

# Link Bullet if available
if(TARGET Bullet::Bullet)
  target_link_libraries(coretech_game_app PUBLIC Bullet::Bullet)
elseif(TARGET bullet)
  target_link_libraries(coretech_game_app PUBLIC bullet)
endif()

# If miniaudio header is available (via FetchContent), include its directory
if(EXISTS "${miniaudio_SOURCE_DIR}/miniaudio.h")
  target_include_directories(coretech_game_app PUBLIC ${miniaudio_SOURCE_DIR})
endif()

# link renderer and asset manager
if(TARGET coretech_renderer)
  target_link_libraries(coretech_game_app PUBLIC coretech_renderer)
endif()
