#pragma once
namespace CoreTech {
bool initAudio();
void shutdownAudio();
bool playSoundFile(const char* path);
}
void audioUpdate();
