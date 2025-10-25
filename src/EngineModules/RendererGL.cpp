#include "EngineModules/RendererGL.h"
#include "Engine/Engine.h"
#include <iostream>

#include "EngineModules/ECSModule.h"
#ifdef __has_include
 #if __has_include(<glm/glm.hpp>)
  #include <glm/glm.hpp>
 #endif
#endif

// Integrate existing tools renderer if available
#if __has_include("tools/renderer/include/Renderer.h")
  #include "tools/renderer/include/Renderer.h"
  #define HAVE_TOOLS_RENDERER 1
#else
  #define HAVE_TOOLS_RENDERER 0
#endif

#ifdef ENABLE_OPENGL
  #ifdef ENABLE_GLFW
    #include <GLFW/glfw3.h>
  #endif
  // glad or other loader would be used when available
  #ifdef __has_include
    #if __has_include(<glad/glad.h>)
      #include <glad/glad.h>
      #define HAVE_GLAD 1
    #endif
  #endif
#endif

struct coretech::RendererGL::Impl {
#ifdef ENABLE_GLFW
    GLFWwindow* window = nullptr;
#endif
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int shader = 0;
    bool initialized = false;
};

using namespace coretech;

RendererGL::RendererGL() : impl_(new Impl()) {}
RendererGL::~RendererGL() { shutdown(); }

bool RendererGL::init() {
    std::cout << "[RendererGL] init\n";
#ifdef ENABLE_OPENGL
  #ifdef ENABLE_GLFW
    if(!glfwInit()){
        std::cerr << "GLFW init failed\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    impl_->window = glfwCreateWindow(800,600,"CoreTechEngine",NULL,NULL);
    if(!impl_->window){
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(impl_->window);
    #ifdef HAVE_GLAD
      if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD\n";
        return false;
      }
    #endif
    // Minimal triangle setup (if GL functions available)
    #ifdef HAVE_GLAD
    float tri[] = { -0.5f,-0.5f, 0.0f,  0.5f,-0.5f,0.0f,  0.0f,0.5f,0.0f };
    glGenVertexArrays(1,&impl_->vao);
    glGenBuffers(1,&impl_->vbo);
    glBindVertexArray(impl_->vao);
    glBindBuffer(GL_ARRAY_BUFFER, impl_->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    impl_->initialized = true;
    #endif
    // If tools renderer exists, initialize it with the created context
    #if HAVE_TOOLS_RENDERER
      if(CoreTech::Renderer::instance().init(800,600)){
          CoreTech::Renderer::instance().loadDefaultResources();
          std::cout << "[RendererGL] tools renderer initialized\n";
      } else {
          std::cout << "[RendererGL] tools renderer failed to init\n";
      }
    #endif
  #else
    std::cout << "GLFW not enabled; RendererGL init skipped\n";
  #endif
#else
    std::cout << "OpenGL disabled at compile time; RendererGL init is a no-op\n";
#endif
    return true;
}

void RendererGL::shutdown() {
    std::cout << "[RendererGL] shutdown\n";
#ifdef ENABLE_OPENGL
  #ifdef HAVE_GLAD
    if(impl_->vbo) glDeleteBuffers(1,&impl_->vbo);
    if(impl_->vao) glDeleteVertexArrays(1,&impl_->vao);
  #endif
  #ifdef ENABLE_GLFW
    if(impl_->window) { glfwDestroyWindow(impl_->window); impl_->window = nullptr; }
    glfwTerminate();
  #endif
#endif
}

void RendererGL::onRender() {
    std::cout << "[RendererGL] renderFrame\n";
#ifdef ENABLE_OPENGL
  #ifdef HAVE_GLAD
    glClearColor(0.1f,0.12f,0.14f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(impl_->vao);
    glDrawArrays(GL_TRIANGLES,0,3);
    glBindVertexArray(0);
    #ifdef ENABLE_GLFW
      if(impl_->window) glfwSwapBuffers(impl_->window);
      if(impl_->window) glfwPollEvents();
    #endif
    #if HAVE_TOOLS_RENDERER
      // Use the tools renderer to draw a default mesh if available
      {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f,0.0f,5.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        CoreTech::Renderer::instance().reloadShadersIfNeeded();
        CoreTech::Renderer::instance().drawMesh(view, proj, model);
      }
    #endif

    // If ECSModule is registered, try to render entities
    {
      // find ECS module
      for(auto m : coretech::Engine::instance().modules_) {
          auto ecs = dynamic_cast<coretech::ECSModule*>(m);
          if(ecs) {
              for(auto id : ecs->entities().list()) {
                  auto rc = ecs->components().get<RenderableComponent>(id);
                  auto tc = ecs->components().get<TransformComponent>(id);
                  if(rc && tc) {
                      std::cout << "[RendererGL] draw entity " << id << " mesh=" << rc->meshPath << " pos=" << tc->position[0] << "," << tc->position[1] << "," << tc->position[2] << "\n";
                      // If tools renderer exists, call drawMesh with transforms
                      #if HAVE_TOOLS_RENDERER
                        glm::mat4 model(1.0f);
                        CoreTech::Renderer::instance().drawMesh(model, model, model);
                      #endif
                  }
              }
          }
      }
    }

  #else
    // no GL loader, fallback to placeholder
    std::cout << "[RendererGL] GL loader missing; drawing skipped\n";
  #endif
#else
    std::cout << "[RendererGL] OpenGL disabled; render skipped\n";
#endif
}

bool RendererGL::hasWindow() const {
#ifdef ENABLE_GLFW
  return impl_ && impl_->window != nullptr;
#else
  return false;
#endif
}
