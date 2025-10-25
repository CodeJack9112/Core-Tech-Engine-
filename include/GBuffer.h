#pragma once
namespace CoreTech {

class GBuffer {
public:
    GBuffer();
    bool Initialize(int w, int h);
    bool Initialize(); // default
    void BindForWriting();
    void BindForReading();
    void Shutdown();
private:
    int width, height;
    bool initialized;
};

}