
#include "../include/AudioManager.h"
#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char** argv){
    AudioManager am;
    if(!am.initialize()){
        std::cerr << "Failed to init audio manager\\n";
        return 1;
    }
    int h = am.loadSound("assets/audio/test_sound.wav");
    if(h==0){
        std::cerr << "Failed to load sound. Make sure assets/audio/test_sound.wav exists\\n";
    } else {
        am.playSound(h, false);
        std::cout << "Playing sound...\\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
        am.unloadSound(h);
    }
    am.shutdown();
    return 0;
}
