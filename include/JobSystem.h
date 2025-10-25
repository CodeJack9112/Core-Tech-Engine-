#pragma once
#include <functional>
void JS_Init(int workerCount=0);
void JS_Shutdown();
void JS_Submit(std::function<void()> job);
