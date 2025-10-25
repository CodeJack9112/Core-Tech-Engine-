
OGG Support (stb_vorbis)
------------------------
I added OpenAL integration and WAV support. For OGG decoding, please add stb_vorbis source to the project:

1. Download stb_vorbis.c and stb_vorbis.h from https://github.com/nothings/stb (stb_vorbis)
2. Place them in thirdparty/stb_vorbis/
3. Add the files to src/audio_openal/CMakeLists.txt and define STB_VORBIS_IMPLEMENTATION in one C file compilation unit.
4. Then modify OpenALAudio::loadWAV to detect .ogg and use stb_vorbis to decode into PCM buffer and then call alBufferData.

I can integrate that automatically if you provide the stb_vorbis files or allow internet access.
