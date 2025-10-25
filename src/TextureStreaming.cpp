#include "TextureStreaming.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#ifdef __has_include
#  if __has_include("stb_image.h")
#    include "stb_image.h"
#    define HAVE_STB_IMAGE 1
#  endif
#endif
static std::thread worker; static std::mutex qmtx; static std::condition_variable qcv; static bool running=false;
struct Req { std::string path; std::function<void(const StreamingTexture&)> cb; };
static std::queue<Req> q;
void workerThread(){ while(running){ Req r; { std::unique_lock<std::mutex> lk(qmtx); if(q.empty()){ qcv.wait(lk); if(!running) break; continue; } r = std::move(q.front()); q.pop(); }
  StreamingTexture st; st.path = r.path;
#ifdef HAVE_STB_IMAGE
  int w,h,ch; unsigned char* data = stbi_load(r.path.c_str(), &w,&h,&ch,0);
  if(data){ st.width=w; st.height=h; st.channels=ch; st.pixels.assign(data, data + (w*h*ch)); stbi_image_free(data); }
  else { std::cerr<<"TextureStreaming: failed to load "<<r.path<<"\n"; }
#else
  std::cerr<<"TextureStreaming: stb_image not available, cannot load "<<r.path<<"\n";
#endif
  if(r.cb) r.cb(st);
}
}
void TextureStreaming::Start(){ if(running) return; running=true; worker = std::thread(workerThread); }
void TextureStreaming::Stop(){ if(!running) return; running=false; qcv.notify_all(); if(worker.joinable()) worker.join(); }
void TextureStreaming::Request(const std::string& path, std::function<void(const StreamingTexture&)> cb){ { std::unique_lock<std::mutex> lk(qmtx); q.push({path,cb}); } qcv.notify_one(); }
