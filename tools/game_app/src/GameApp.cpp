#include "GameApp.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../gameplay/include/SceneLoader.h"
#include "../renderer/include/Frustum.h"
#include "../renderer/include/AssetManager.h"
#include "../renderer/include/MaterialComponent.h"
#include "../renderer/include/LightComponent.h"

#include <chrono>
#include <thread>
#include "InputManager.h"
#include "../gameplay/include/Entity.h" // camera component
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace CoreTech;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    // Simple mapping: map WASD and ESC to keys
    if(key==GLFW_KEY_W) CoreTech::InputManager::setKeyState("W", action!=GLFW_RELEASE);
    if(key==GLFW_KEY_S) CoreTech::InputManager::setKeyState("S", action!=GLFW_RELEASE);
    if(key==GLFW_KEY_A) CoreTech::InputManager::setKeyState("A", action!=GLFW_RELEASE);
    if(key==GLFW_KEY_D) CoreTech::InputManager::setKeyState("D", action!=GLFW_RELEASE);
    if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    // map left click
    if(button==GLFW_MOUSE_BUTTON_LEFT) CoreTech::InputManager::setKeyState("MOUSE_LEFT", action!=GLFW_RELEASE);
}

void CoreTech::runGameApp(int width, int height, const char* title){
    if(!glfwInit()){
        std::cerr << "Failed to init GLFW\n"; return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!window){ std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); return; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);

    using clock = std::chrono::high_resolution_clock;
    auto last = clock::now();
    while(!glfwWindowShouldClose(window)){
        auto now = clock::now();
        std::chrono::duration<float> dt = now - last;
        last = now;
        float delta = dt.count();

        // Update camera controllers if any (simple movement in gameplay module uses InputManager)

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
    // initialize glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD
";
    }

    // initialize renderer

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    CoreTech::Renderer::instance().init(width, height);

    using clock = std::chrono::high_resolution_clock;
    auto last = clock::now();
    while(!glfwWindowShouldClose(window)){
        auto now = clock::now();
        std::chrono::duration<float> dt = now - last;
        last = now;
        float delta = dt.count();

        // Update camera controllers if any
        // (CameraController in gameplay module reads InputManager and updates transforms)

        // Render
        CoreTech::Renderer::instance().beginFrame();

        // build simple view/proj using first camera entity if any via SceneLoader (global scene)
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)width/(float)height, 0.1f, 100.0f);
        // Draw entities from scene with frustum culling
        CoreTech::Frustum fr = CoreTech::Frustum::fromViewProj(proj * view);
        if(globalScene){
            for(auto &e : globalScene->getAllEntities()){
                auto t = e->getComponent<CoreTech::TransformComponent>();
                if(!t) continue;
                glm::vec3 pos(t->x,t->y,t->z);
                float radius = std::max({t->sx,t->sy,t->sz}) * 0.6f;
                if(!fr.sphereInFrustum(pos, radius)) continue;
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                model = glm::scale(model, glm::vec3(t->sx,t->sy,t->sz));
                CoreTech::Renderer::instance().drawMesh(view, proj, model);
            }
        }

        CoreTech::Renderer::instance().endFrame();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // show editor UI (uses SceneLoader global scene if loaded by Lua earlier)
        extern std::shared_ptr<CoreTech::Scene> globalScene; // declared in LuaBindings.cpp
        showEditorUI(globalScene);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        // audio maintenance
        CoreTech::audioUpdate();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
(std::chrono::milliseconds(1));
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
