
# Download stb_image.h at configure time and create a compilation unit with STB_IMAGE_IMPLEMENTATION
set(STB_IMAGE_H_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")
set(STB_IMAGE_H_FILE "${CMAKE_CURRENT_BINARY_DIR}/stb_image.h")
if(NOT EXISTS "${STB_IMAGE_H_FILE}")
    message(STATUS "Downloading stb_image.h from ${STB_IMAGE_H_URL}")
    file(DOWNLOAD "${STB_IMAGE_H_URL}" "${STB_IMAGE_H_FILE}" SHOW_PROGRESS STATUS dl_status)
    list(GET dl_status 0 dl_result)
    if(NOT dl_result EQUAL 0)
        message(FATAL_ERROR "Failed to download stb_image.h")
    endif()
endif()

# create a tiny compilation unit that implements stb_image
set(STB_IMPL_FILE "${CMAKE_CURRENT_BINARY_DIR}/stb_image_impl.c")
file(WRITE "${STB_IMPL_FILE}" "#define STB_IMAGE_IMPLEMENTATION\n#include \\\"${STB_IMAGE_H_FILE}\\\"\n")

add_library(coretech_stb_image STATIC "${STB_IMPL_FILE}")
target_include_directories(coretech_stb_image PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
