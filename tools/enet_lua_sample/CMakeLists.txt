cmake_minimum_required(VERSION 3.10)
project(enet_lua_sample)

file(GLOB SAMPLE_SRC "*.cpp" "*.h")

add_executable(enet_lua_sample ${SAMPLE_SRC})

# Prefer FetchContent targets if they exist; otherwise try prebuilt import targets
set(ENET_TARGET "enet")
set(LUA_TARGET "lua")

if(TARGET enet)
  set(ENET_TARGET "enet")
elseif(TARGET prebuilt_enet)
  set(ENET_TARGET "prebuilt_enet")
endif()

if(TARGET lua)
  set(LUA_TARGET "lua")
elseif(TARGET prebuilt_lua)
  set(LUA_TARGET "prebuilt_lua")
endif()

if(ENET_TARGET)
  message(STATUS "Linking ENET target: ${ENET_TARGET}")
  target_link_libraries(enet_lua_sample PRIVATE ${ENET_TARGET})
endif()

if(LUA_TARGET)
  message(STATUS "Linking LUA target: ${LUA_TARGET}")
  target_link_libraries(enet_lua_sample PRIVATE ${LUA_TARGET})
endif()

# Add include dirs for thirdparty if present
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../thirdparty")
  target_include_directories(enet_lua_sample PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../thirdparty")
endif()
