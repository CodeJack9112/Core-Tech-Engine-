#pragma once
namespace CoreTech {
class ShadowMapper {
public:
    static void init(int size=1024);
    static void beginShadowPass();
    static void endShadowPass();
};
}