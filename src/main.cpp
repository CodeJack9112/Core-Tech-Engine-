\
    #include "../include/PhysicsManager.h" 
#include "../include/ConfigLoader.h"\n#include "../include/VisualScripting.h"
#include "../include/Launcher.h"
#include "../include/LauncherImGui.h"
#include <fstream>
    #include "../include/GameplayPCGManager.h"
    #include "../include/NavMesh.h"
    #include "../include/PickupManager.h"
#include "../include/AudioManager.h"
#include "../include/Renderer.h"
#ifdef _WIN32
#include "../include/DirectXRenderer.h"
#include <windows.h>
#endif
    #include <iostream>
    #include <thread>
    #include <chrono>

    
// Launcher UI: on Windows shows a simple Win32 dialog with radio buttons; on other platforms uses console prompt.
// The launcher writes the selected renderer to config/engine.cfg as 'render_api=...'
#include <string>
#include <fstream>

static std::string showLauncherAndGetChoice(){
#ifdef _WIN32
    // Simple MessageBox-based launcher (no resource dialogs to keep code minimal)
    int ret = MessageBoxA(NULL, "Choose renderer:\\nOK = OpenGL (default)\\nCancel = DirectX11 (Windows only)", "CoreTech Launcher", MB_OKCANCEL | MB_ICONQUESTION);
    if(ret == IDOK) return std::string("OpenGL");
    else return std::string("DirectX11");
#else
    // Console fallback
    std::string choice;
    std::cout << "Select renderer (type 'dx11' for DirectX11 or Enter for OpenGL): ";
    std::getline(std::cin, choice);
    if(choice.size()==0) return std::string("OpenGL");
    if(choice == "dx11" || choice == "DirectX11" || choice == "DirectX") return std::string("DirectX11");
    return std::string("OpenGL");
#endif
}

static void saveChoiceToConfig(const std::string &choice){
    std::ofstream f("config/engine.cfg", std::ios::trunc);
    if(!f) return;
    f << "render_api=" << choice << "\\n";
    f.close();
}

// --- ECS demo insertion (auto-added) --- REMOVED

#include "../include/ecs/EntityManager.h"
#include "../include/ecs/ComponentManager.h"
#include "../include/ecs/TransformComponent.h"
#include "../src/ecs/MovementSystem.cpp" // lightweight include for demo only

// create ECS managers
static EntityManager g_entityMgr;
static ComponentManager g_compMgr;
static MovementSystem g_movementSys;

void runECSDemo() {
    auto e = g_entityMgr.createEntity("Player");
    auto t = g_compMgr.addComponent<TransformComponent>(e);
    for(int i=0;i<5;i++){
        g_movementSys.update(0.016f, g_compMgr);
        printf(\"Epos frame %d: x=%.3f\\n\", i, t->position.x);
    }
}
// --- end ECS demo insertion ---


#include "../include/ecs/EntityManager.h"
#include "../include/ecs/ComponentManager.h"
#include "../include/ecs/TransformComponent.h"
#include "../include/ecs/System.h"
#include "../src/ecs/MovementSystem.cpp" // example system
#include "../include/ecs/Component.h"
#include "../src/ecs/ScriptEngine.cpp" // script engine implementation (for simplicity)

static EntityManager g_entityMgr;
static ComponentManager g_compMgr;
static MovementSystem g_movementSys;

void runFullDemo() {
    // create script engine and run demo script (assets/scripts/demo.txt)
    ScriptEngine se(g_entityMgr, g_compMgr);
    // create a starter entity
    auto id = g_entityMgr.createEntity("Starter");
    g_compMgr.addComponent<TransformComponent>(id);
    auto t = g_compMgr.getComponent<TransformComponent>(id);
    if(t) t->position.x = 0.0f;
    // run script file if exists
    se.executeScriptFile(\"assets/scripts/demo.txt\");
    // After script, run movement system a bit and print positions
    for(int i=0;i<5;i++){
        g_movementSys.update(0.016f, g_compMgr);
        auto comp = g_compMgr.getComponent<TransformComponent>(id);
        if(comp) printf(\"After frame %d, Starter.x = %.3f\\n\", i, comp->position.x);
    }
}


#include "../include/InputManager.h"
#include "../include/ResourceManager.h"
#include "../src/ecs/PhysicsSystem.cpp"
#include "../src/ecs/RenderSystem.cpp"
#include "../src/ecs/SceneManager.cpp"

void runFullDemo() {
    InputManager input;
    ResourceManager res;
    PhysicsSystem phys;
    RenderSystem renderer;

    // create entities via ECS managers already present
    auto e1 = g_entityMgr.createEntity("Camera");
    g_compMgr.addComponent<CameraComponent>(e1);
    auto e2 = g_entityMgr.createEntity("Player");
    g_compMgr.addComponent<TransformComponent>(e2);
    g_compMgr.addComponent<MeshComponent>(e2).meshPath = \"assets/models/player.obj\";

    SceneManager scene(g_entityMgr, g_compMgr);
    scene.saveScene(\"assets/scenes/test_scene.json\");
    scene.loadScene(\"assets/scenes/test_scene.json\");

    // simple loop, exit on ESC
    for(int frame=0; frame<300; ++frame){
        float dt = 1.0f/60.0f;
        if(input.isKeyDown(VK_ESCAPE)) { printf(\"ESC pressed, exiting demo\\n\"); break; }
        // physics
        phys.update(dt, g_compMgr);
        // systems
        g_movementSys.update(dt, g_compMgr);
        renderer.update(dt, g_compMgr);
    }
}

int main(int argc, char** argv){
        std::cout << "Starting Core Tech demo (Core Tech)..." << std::endl;
        PhysicsManager phys;
        if(!phys.initialize()){
            std::cerr << "Failed to initialize physics." << std::endl;
            return -1;
        }

        // Create demo scene using gameplay generator
        AudioManager audio;
        if(!audio.initialize()) std::cout << "Warning: audio failed to initialize or is stubbed." << std::endl;

        // Renderer (loads Blinn-Phong shaders)
        Renderer renderer;
        bool shadersLoaded = renderer.loadShaders("shaders/BlinnPhong.vert.glsl", "shaders/BlinnPhong.frag.glsl");
        if(shadersLoaded) renderer.compileShaders();
        // try loading normal-mapped variant
        bool nmLoaded = renderer.loadShaders("shaders/BlinnPhongNormalMapped.vert.glsl", "shaders/BlinnPhongNormalMapped.frag.glsl");
        if(nmLoaded) renderer.compileShaders();
        // set a demo material
        Material demoMat; demoMat.name = "DemoMat"; demoMat.albedo = {0.8f,0.6f,0.4f}; demoMat.specPower = 48.0f; demoMat.specIntensity = 1.2f;
        renderer.setMaterial(demoMat);
        // Load HLSL shader sources for DirectX backends (stored only)
        renderer.loadHLSLShader("shaders/hlsl/BlinnPhong.hlsl");
        renderer.loadHLSLShader("shaders/hlsl/PBR.hlsl");

        GameplayPCGManager gpcg(12345);
        // ground
        btTransform groundTx; groundTx.setIdentity(); groundTx.setOrigin(btVector3(0,-0.5,0));
        phys.createStaticBox(btVector3(50,0.5f,50), groundTx);

        // player marker
        btTransform ptx; ptx.setIdentity(); ptx.setOrigin(btVector3(0,1.0f,-5.0f));
        auto player = phys.createDynamicSphere(0.4f, ptx, 0.0f);
        if(player) player->setKinematic(true);
        if(player) player->setUserTag(PhysicsComponent::TAG_PLAYER);

        // pickup manager
        PickupManager pm;
        phys.registerPickupCallback([&phys,&pm](std::shared_ptr<PhysicsComponent> pickup, std::shared_ptr<PhysicsComponent> other){
            pm.onPickupCollected(pickup);
            if(pickup) phys.removeComponent(pickup);
        });

        gpcg.generateBuilding(btVector3(0,2,0), 4, 3, 4.0f, phys);
        gpcg.placeLoot(btVector3(-8,0.5f,-6), btVector3(8,3.0f,6), 12, phys);
        auto sp = gpcg.createEnemySpawns(btVector3(-10,0.5f,-10), btVector3(10,3.0f,10), 6, phys);
        if(!sp.empty()){
            gpcg.dropSupplyCrate(sp[0], phys);
            // try to load and play a sample (file should exist in working dir)
            int s = audio.loadSound("demo/audio/demo/audio/sample_drop.wav");
            if(s>=0) audio.playSound(s);
        }

        // run simulation for a few seconds

// --- Runtime renderer hot-swap support ---
auto readRenderAPIFromConfig = [](const std::string& path)->std::string{
    std::ifstream f(path);
    if(!f) return std::string("OpenGL");
    std::string line;
    while(std::getline(f,line)){
        auto pos = line.find('=');
        if(pos==std::string::npos) continue;
        std::string k = line.substr(0,pos);
        std::string v = line.substr(pos+1);
        for(auto &c:k) c = std::tolower(c);
        if(k == "render_api") return v;
    }
    return std::string("OpenGL");
};\n
        const int steps = 300;
        std::string currentAPI = api;
        for(int i=0;i<steps;i++){
            // check config for changes every 60 frames
            if(i % 60 == 0){
                std::string newAPI = readRenderAPIFromConfig("config/engine.cfg");
                if(newAPI != currentAPI){
                    std::cout << "Renderer change detected: " << newAPI << ", switching...\n";
                    // simple strategy: restart renderer if possible (shutdown init)
                    if(renderer){ renderer->shutdown(); }
                    // select new renderer
                    if(newAPI == "DirectX11" || newAPI == "DirectX" || newAPI == "dx11"){
#ifdef _WIN32
                        static RendererDX11 dxr_local;
                        renderer = &dxr_local;
                        if(!renderer->init(1280,720)){
                            std::cout << "Failed to init DirectX renderer, falling back to OpenGL.\n";
                            renderer = &oglRenderer;
                            renderer->init(1280,720);
                        }
#else
                        std::cout << "DirectX selected but not available on this platform; staying on OpenGL.\n";
                        renderer = &oglRenderer;
#endif
                    } else {
                        renderer = &oglRenderer;
                        renderer->init(1280,720);
                    }
                    currentAPI = newAPI;
                }
            }
            phys.dynamicsWorld()->stepSimulation(1.0f/60.0f);
            phys.processCollisionCallbacks();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // navmesh example
        auto nodes = gpcg.generateNavMeshNodes(btVector3(-20,0,-20), btVector3(20,10,20), 2.0f, phys);
        std::cout << \"Nav nodes: \" << nodes.size() << std::endl;
        NavMesh nm; nm.build(nodes, 3.0f);
        if(!nodes.empty()){
            btVector3 start = player ? player->getTransform().getOrigin() : btVector3(0,0,0);
            auto path = nm.findPath(start, nodes.back());
            std::cout << \"Path length: \" << path.size() << std::endl;
        }

        std::cout << \"Pickups collected: \" << pm.collected() << std::endl;    // Renderer selection based on config
    
// --- Simple launcher UI (console) ---
auto showLauncherAndSetConfig = [](const std::string& cfgPath){
    std::cout << "CoreTech Launcher\n";
    std::cout << "Select renderer backend:\n";
    std::cout << "  1) OpenGL (cross-platform)\n";
    std::cout << "  2) DirectX11 (Windows only)\n";
    std::cout << "Enter choice (1 or 2) or press Enter to use config: ";
    std::string choice;
    std::getline(std::cin, choice);
    if(choice == "1"){
        std::ofstream cfg(cfgPath);
        cfg << "render_api=OpenGL\n";
        cfg.close();
        std::cout << "Configured renderer: OpenGL\n";
    } else if(choice == "2"){
        std::ofstream cfg(cfgPath);
        cfg << "render_api=DirectX11\n";
        cfg.close();
        std::cout << "Configured renderer: DirectX11\n";
    } else {
        std::cout << "Using existing configuration file or default.\n";
    }
};\n
ConfigLoader cfg; showLauncherAndSetConfig("config/engine.cfg"); cfg.load("config/engine.cfg");
    std::string api = cfg.get("render_api", "OpenGL");
    std::cout << "Configured render_api=" << api << std::endl;

    if(api == "DirectX11" || api == "DirectX" || api == "dx11"){
#ifdef _WIN32
        // If DirectX selected and we're on Windows, run DirectX renderer
        HINSTANCE hInstance = GetModuleHandle(NULL);
        DirectXRenderer dxr;
        if(dxr.initialize(hInstance, SW_SHOWDEFAULT, 1280, 720)){
            dxr.compileHLSL(L"shaders/hlsl/BlinnPhong.hlsl"); // set shader path
            dxr.setMaterial(renderer.currentMaterial());
            dxr.runMainLoop();
        } else {
            std::cout << "DirectX renderer failed to initialize or not available.\n";
            // fallback: run simple OpenGL frame loop
            const int steps = 300;
            for(int i=0;i<steps;i++){
                phys.dynamicsWorld()->stepSimulation(1.0f/60.0f);
                phys.processCollisionCallbacks();
                renderer.renderFrame();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
#else
        std::cout << "DirectX selected in config but not available on this platform. Falling back to OpenGL.\n";

// --- Runtime renderer hot-swap support ---
auto readRenderAPIFromConfig = [](const std::string& path)->std::string{
    std::ifstream f(path);
    if(!f) return std::string("OpenGL");
    std::string line;
    while(std::getline(f,line)){
        auto pos = line.find('=');
        if(pos==std::string::npos) continue;
        std::string k = line.substr(0,pos);
        std::string v = line.substr(pos+1);
        for(auto &c:k) c = std::tolower(c);
        if(k == "render_api") return v;
    }
    return std::string("OpenGL");
};\n
        const int steps = 300;
        std::string currentAPI = api;
        for(int i=0;i<steps;i++){
            // check config for changes every 60 frames
            if(i % 60 == 0){
                std::string newAPI = readRenderAPIFromConfig("config/engine.cfg");
                if(newAPI != currentAPI){
                    std::cout << "Renderer change detected: " << newAPI << ", switching...\n";
                    // simple strategy: restart renderer if possible (shutdown init)
                    if(renderer){ renderer->shutdown(); }
                    // select new renderer
                    if(newAPI == "DirectX11" || newAPI == "DirectX" || newAPI == "dx11"){
#ifdef _WIN32
                        static RendererDX11 dxr_local;
                        renderer = &dxr_local;
                        if(!renderer->init(1280,720)){
                            std::cout << "Failed to init DirectX renderer, falling back to OpenGL.\n";
                            renderer = &oglRenderer;
                            renderer->init(1280,720);
                        }
#else
                        std::cout << "DirectX selected but not available on this platform; staying on OpenGL.\n";
                        renderer = &oglRenderer;
#endif
                    } else {
                        renderer = &oglRenderer;
                        renderer->init(1280,720);
                    }
                    currentAPI = newAPI;
                }
            }
            phys.dynamicsWorld()->stepSimulation(1.0f/60.0f);
            phys.processCollisionCallbacks();
            renderer.renderFrame();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
#endif
    } else {
        // Default: OpenGL path

// --- Runtime renderer hot-swap support ---
auto readRenderAPIFromConfig = [](const std::string& path)->std::string{
    std::ifstream f(path);
    if(!f) return std::string("OpenGL");
    std::string line;
    while(std::getline(f,line)){
        auto pos = line.find('=');
        if(pos==std::string::npos) continue;
        std::string k = line.substr(0,pos);
        std::string v = line.substr(pos+1);
        for(auto &c:k) c = std::tolower(c);
        if(k == "render_api") return v;
    }
    return std::string("OpenGL");
};\n
        const int steps = 300;
        std::string currentAPI = api;
        for(int i=0;i<steps;i++){
            // check config for changes every 60 frames
            if(i % 60 == 0){
                std::string newAPI = readRenderAPIFromConfig("config/engine.cfg");
                if(newAPI != currentAPI){
                    std::cout << "Renderer change detected: " << newAPI << ", switching...\n";
                    // simple strategy: restart renderer if possible (shutdown init)
                    if(renderer){ renderer->shutdown(); }
                    // select new renderer
                    if(newAPI == "DirectX11" || newAPI == "DirectX" || newAPI == "dx11"){
#ifdef _WIN32
                        static RendererDX11 dxr_local;
                        renderer = &dxr_local;
                        if(!renderer->init(1280,720)){
                            std::cout << "Failed to init DirectX renderer, falling back to OpenGL.\n";
                            renderer = &oglRenderer;
                            renderer->init(1280,720);
                        }
#else
                        std::cout << "DirectX selected but not available on this platform; staying on OpenGL.\n";
                        renderer = &oglRenderer;
#endif
                    } else {
                        renderer = &oglRenderer;
                        renderer->init(1280,720);
                    }
                    currentAPI = newAPI;
                }
            }
            phys.dynamicsWorld()->stepSimulation(1.0f/60.0f);
            phys.processCollisionCallbacks();
            renderer.renderFrame();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
        // cleanup
        gpcg.destroyAll(phys);
        phys.shutdown();
        std::cout << \"Demo finished.\" << std::endl;
        return 0;
    }
