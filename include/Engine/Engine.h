#pragma once
#include <vector>
#include <functional>
#include <string>
#include <atomic>
#include <chrono>

namespace coretech {

class EngineModule {
public:
    virtual ~EngineModule() = default;
    virtual bool init() = 0;
    virtual void shutdown() = 0;
    virtual const char* name() const = 0;
    // Optional lifecycle hooks
    virtual void onUpdate(float dt) {}
    virtual void onRender() {}
    virtual void onEvent(const std::string& evt, const std::string& payload) {}
};

class Engine {
public:
    static Engine& instance();
    void registerModule(EngineModule* mod);
    bool startup();   // initialize registered modules in order
    void shutdown();  // shutdown in reverse order

    // Run loop control
    void run(int maxFrames = -1, float targetHz = 60.0f);
    void stop();
    bool isRunning() const { return running_.load(); }

    // Simple event bus
    void broadcastEvent(const std::string& evt, const std::string& payload);

private:
    Engine() = default;
    std::vector<EngineModule*> modules_;
    std::atomic<bool> running_{false};
};

} // namespace coretech
