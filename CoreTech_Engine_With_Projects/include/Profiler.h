#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <iostream>

namespace coretech {
  using Clock = std::chrono::high_resolution_clock;
  struct ScopeTimer {
    const char* name;
    Clock::time_point start;
    ScopeTimer(const char* n): name(n), start(Clock::now()){}
    ~ScopeTimer(){ auto d = Clock::now() - start; std::cout<<"[PROFILE] "<<name<<" "<<std::chrono::duration<double, std::milli>(d).count()<<" ms\n"; }
  };
}
#define CT_PROFILE_SCOPE(name) coretech::ScopeTimer timer##__LINE__(name)
