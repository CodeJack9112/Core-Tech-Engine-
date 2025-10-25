#include "EngineModules/InputModule.h"
#include <iostream>

namespace coretech {

struct InputModule::Impl {
    std::vector<InputModule::KeyCallback> keyCbs;
};

static InputModule* s_input = nullptr;

bool InputModule::init() {
    std::cout << "[InputModule] init (headless)\n";
    impl_ = new Impl();
    s_input = this;
    return true;
}

void InputModule::shutdown() {
    std::cout << "[InputModule] shutdown\n";
    delete impl_;
    impl_ = nullptr;
    s_input = nullptr;
}

void InputModule::addKeyCallback(KeyCallback cb) {
    if(impl_) impl_->keyCbs.push_back(cb);
}

void InputModule::pollEvents() {
    // no-op in headless mode
}

} // namespace coretech
