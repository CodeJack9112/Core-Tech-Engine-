
# Downloads stb_vorbis.c on configure and adds it as a static library
cmake_minimum_required(VERSION 3.0)
project(stb_vorbis_fetch NONE)

set(STB_VORBIS_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_vorbis.c")
set(STB_VORBIS_FILE "${CMAKE_CURRENT_BINARY_DIR}/stb_vorbis.c")

if(NOT EXISTS "${STB_VORBIS_FILE}")
    message(STATUS "Downloading stb_vorbis.c from ${STB_VORBIS_URL}")
    file(DOWNLOAD "${STB_VORBIS_URL}" "${STB_VORBIS_FILE}" SHOW_PROGRESS STATUS download_status)
    list(GET download_status 0 download_result)
    if(NOT download_result EQUAL 0)
        message(FATAL_ERROR "Failed to download stb_vorbis.c (status ${download_status})")
    endif()
endif()

add_library(coretech_stb_vorbis STATIC "${STB_VORBIS_FILE}")
target_include_directories(coretech_stb_vorbis PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
