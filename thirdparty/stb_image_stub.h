#pragma once
/* Minimal stub for stb_image to allow compilation when real stb is not fetched.
   This does NOT provide image loading functionality. Replace with real stb_image.h
   from https://github.com/nothings/stb when you want full image loading. */
static inline unsigned char* stbi_load_stub(const char* filename, int* x, int* y, int* channels_in_file, int desired_channels) { return 0; }
static inline void stbi_image_free_stub(void* retval_from_stbi_load) {}
