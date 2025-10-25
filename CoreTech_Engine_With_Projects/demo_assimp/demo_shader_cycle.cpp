\
    // Demo: Shader cycle sequencer
    // Loads a model (or cube.obj fallback), cycles through cinematic shaders (MW1/MW2/MW3),
    // applies each for N frames, and attempts to request the renderer to save screenshots.
    //
    // This demo uses best-effort calls to the engine's renderer API:
    // - Renderer::Get() to fetch renderer singleton
    // - r->SetMaterialForEntity(entityId, materialName) to apply materials (hypothetical)
    // - r->SaveScreenshot(path) to save frame (hypothetical)
    //
    // If your renderer uses different functions, adapt the calls or provide small wrapper shims.

    #include <iostream>
    #include <thread>
    #include <chrono>
    #include <string>

    #ifdef __has_include
    #  if __has_include("Renderer.h")
    #    include "Renderer.h"
    #  endif
    #endif

    int main(int argc, char** argv) {
        std::string modelPath = "../assets/CesiumMan.glb";
        if(argc > 1) modelPath = argv[1];
        std::cout << "Shader cycle demo: model=" << modelPath << std::endl;
    #ifdef __has_include
    #  if __has_include("Renderer.h")
        Renderer* r = nullptr;
        try { r = Renderer::Get(); } catch(...) { r = nullptr; }
        if(!r) {
            std::cout << "Renderer not available. This demo requires engine renderer. Exiting." << std::endl;
            return 1;
        }
        // create entity and load model if API exists (best-effort)
        int entity = -1;
        try {
            entity = r->CreateEntityFromModel(modelPath); // hypothetical convenience function
        } catch(...) {
            std::cout << "CreateEntityFromModel not supported by renderer. Try loading model in editor and use demo target." << std::endl;
        }
        const char* materials[] = {"Mat_Cinematic_MW1","Mat_Cinematic_MW2","Mat_Cinematic_MW3"};
        int cycles = 3;
        int framesPerMaterial = 60; // show ~60 frames per material
        for(int c=0;c<cycles;c++) {
            for(int m=0;m<3;m++) {
                std::string mat = materials[m];
                std::cout << "Applying material: " << mat << std::endl;
                try {
                    if(entity>=0) r->SetMaterialForEntity(entity, mat);
                } catch(...) {}
                // Show for framesPerMaterial frames (sleeping as placeholder)
                for(int f=0; f<framesPerMaterial; ++f) {
                    // allow renderer to tick - best-effort
                    try { r->TickFrame(); } catch(...) {}
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                    // attempt screenshot every 30 frames
                    if(f % 30 == 0) {
                        try {
                            std::string out = std::string("screenshots/") + mat + "_c" + std::to_string(c) + "_f" + std::to_string(f) + ".png";
                            r->SaveScreenshot(out);
                            std::cout << "Saved screenshot: " << out << std::endl;
                        } catch(...) {}
                    }
                }
            }
        }
        std::cout << "Demo finished." << std::endl;
    #else
        std::cout << "Renderer.h not available at compile time. This demo is a placeholder demonstrating intended workflow." << std::endl;
    #endif
        return 0;
    }
