add_executable(coretech_features_demo src/demo/demo.cpp)
target_include_directories(coretech_features_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(coretech_features_demo PRIVATE coretech_features)
