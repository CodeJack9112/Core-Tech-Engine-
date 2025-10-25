#include "JobSystem.h"
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <random>
static std::vector<std::thread> workers;
static std::vector<std::deque<std::function<void()>>> queues;
static std::vector<std::mutex> queueMutexes;
static std::condition_variable_any cv;
static std::atomic<bool> running(false);
static std::atomic<int> jobCount(0);
static int workerCountGlobal = 0;
void workerLoop(int id){ std::mt19937 rng(id); while(running){ std::function<void()> job;
    { std::unique_lock<std::mutex> lk(queueMutexes[id]); if(!queues[id].empty()){ job = std::move(queues[id].front()); queues[id].pop_front(); } }
    if(job){ try{ job(); } catch(...){} jobCount--; continue; }
    // try steal
    int n = workerCountGlobal;
    int start = rng() % (n>0?n:1);
    bool found=false;
    for(int i=0;i<n;i++){ int idx=(start+i)%n; if(idx==id) continue; std::unique_lock<std::mutex> lk(queueMutexes[idx]); if(!queues[idx].empty()){ job = std::move(queues[idx].back()); queues[idx].pop_back(); found=true; } if(found) break; }
    if(found){ try{ job(); } catch(...){} jobCount--; continue; }
    // wait
    std::this_thread::yield();
}
}
void JS_Init(int workerCount){ if(workerCount<=0) workerCount = std::max(1u, std::thread::hardware_concurrency()); workerCountGlobal = workerCount; running=true; queues.resize(workerCount); queueMutexes.resize(workerCount);
for(int i=0;i<workerCount;i++) workers.emplace_back([i](){ workerLoop(i); }); }
void JS_Shutdown(){ running=false; for(auto &t: workers) if(t.joinable()) t.join(); workers.clear(); queues.clear(); queueMutexes.clear(); }
void JS_Submit(std::function<void()> job){ // push to a random queue
  static thread_local std::mt19937 rng((unsigned)std::hash<std::thread::id>{}(std::this_thread::get_id())); if(queues.empty()){ JS_Init(); }
  int idx = rng() % queues.size(); { std::unique_lock<std::mutex> lk(queueMutexes[idx]); queues[idx].push_front(std::move(job)); }
  jobCount++; }
