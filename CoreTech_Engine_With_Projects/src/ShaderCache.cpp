#include "ShaderCache.h"
#include <fstream>
#include <iostream>
#ifdef __has_include
#  if __has_include(<GL/glew.h>)
#    include <GL/glew.h>
#    define HAVE_GL 1
#  endif
#endif
void ShaderCache::Precompile(const std::string& path){ std::ifstream f(path); if(!f){ std::cerr<<"ShaderCache: failed to open "<<path<<"\n"; return; } std::string src((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
#ifdef HAVE_GL
  GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* s = src.c_str(); glShaderSource(shader,1,&s,nullptr); glCompileShader(shader);
  GLint ok=0; glGetShaderiv(shader, GL_COMPILE_STATUS, &ok); if(!ok){ GLint len=0; glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len); std::string log; log.resize(len); glGetShaderInfoLog(shader, len, nullptr, &log[0]); std::cerr<<"ShaderCache: compile log for "<<path<<"\n"<<log<<"\n"; }
  glDeleteShader(shader);
#endif
  cache[path]=src; }
bool ShaderCache::Has(const std::string& key) const { return cache.find(key)!=cache.end(); }
