#pragma once
#ifdef _WIN32
  #define PLUGIN_API __declspec(dllexport)
#else
  #define PLUGIN_API
#endif
extern "C" {
typedef void (*PluginInitFunc)();
typedef void (*PluginShutdownFunc)();
typedef const char* (*PluginNameFunc)();
struct PluginInterface {
    PluginInitFunc init;
    PluginShutdownFunc shutdown;
    PluginNameFunc name;
};
PLUGIN_API PluginInterface* createPlugin(); // plugin should implement and export this
}
