cmake_minimum_required(VERSION 3.14)
project(CoreTechTools)
add_executable(lod_generator lod_generator.cpp)
target_include_directories(lod_generator PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(generate_lods generate_lods.cpp)
target_include_directories(generate_lods PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(export_gltf export_gltf.cpp)
target_include_directories(export_gltf PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/thirdparty)
add_executable(demo_runtime_lod ../demo_assimp/demo_runtime_lod.cpp)
target_include_directories(demo_runtime_lod PRIVATE ${CMAKE_SOURCE_DIR}/include)
