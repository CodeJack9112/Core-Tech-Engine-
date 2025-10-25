# Optional OpenAL audio backend
option(USE_OPENAL "Enable OpenAL audio backend" ON)

if(USE_OPENAL)
    find_package(OpenAL REQUIRED)
endif()

add_library(coretech_audio_openal STATIC OpenALAudio.cpp)
target_include_directories(coretech_audio_openal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(USE_OPENAL AND OpenAL_FOUND)
    target_compile_definitions(coretech_audio_openal PRIVATE USE_OPENAL=1)
    target_link_libraries(coretech_audio_openal PRIVATE OpenAL::OpenAL)
endif()

# Added by assistant: link with stb_vorbis if available
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/stb_vorbis)

target_link_libraries(coretech_audio_openal PRIVATE coretech_stb_vorbis)
