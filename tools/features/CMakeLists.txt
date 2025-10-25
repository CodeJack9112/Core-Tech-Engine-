cmake_minimum_required(VERSION 3.10)
project(coretech_features)

# Build a small static library with the feature prototypes
file(GLOB FEATURE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(coretech_features STATIC ${FEATURE_SRC})
target_include_directories(coretech_features PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Position-independent for linking
set_target_properties(coretech_features PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(demo)
