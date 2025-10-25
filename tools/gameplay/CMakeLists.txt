cmake_minimum_required(VERSION 3.10)
project(coretech_gameplay)

file(GLOB GAME_SRC "src/*.cpp")
file(GLOB GAME_INC "include/*.h")

add_library(coretech_gameplay STATIC ${GAME_SRC} ${GAME_INC})
target_include_directories(coretech_gameplay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link Lua if available (either fetched or prebuilt)
if(TARGET lua)
  target_link_libraries(coretech_gameplay PUBLIC lua)
elseif(TARGET prebuilt_lua)
  target_link_libraries(coretech_gameplay PUBLIC prebuilt_lua)
endif()

# Add sample executable demonstrating scene load and lua calls
add_executable(gameplay_demo src/gameplay_demo.cpp)
target_link_libraries(gameplay_demo PRIVATE coretech_gameplay)
if(TARGET lua)
  target_link_libraries(gameplay_demo PRIVATE lua)
elseif(TARGET prebuilt_lua)
  target_link_libraries(gameplay_demo PRIVATE prebuilt_lua)
endif()

# link platform/game app if available
if(TARGET coretech_game_app)
  target_link_libraries(coretech_gameplay PUBLIC coretech_game_app)
endif()

# physics and animation
file(GLOB GAME_EXTRA_SRC "src/PhysicsComponent.cpp" "src/AnimationComponent.cpp")
list(APPEND GAME_SRC ${GAME_EXTRA_SRC})

# prototype extras
file(GLOB GAME_EXTRA "src/Prefab.cpp" "src/AIComponent.cpp")
list(APPEND GAME_SRC ${GAME_EXTRA})
