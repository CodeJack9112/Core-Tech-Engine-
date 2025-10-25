\
    // Demo: initialize toon materials and show ImGui controls.
    #include <iostream>
    #ifdef __has_include
    #  if __has_include(\"ToonIntegration.h\")
    #    include \"ToonIntegration.h\"
    #  endif
    #endif
    int main(int argc, char** argv) {
        std::cout << \"Demo Toon Integration: starting...\" << std::endl;
    #ifdef coretech
        // nothing
    #endif
    #ifdef __has_include
    #  if __has_include(\"ToonIntegration.h\")
        coretech::InitializeToonMaterials();
        std::cout << \"Initialized toon materials (if renderer API matched).\" << std::endl;
        // Note: ToonImGuiControls() should be called from your engine's UI loop every frame.
    #  else
        std::cout << \"ToonIntegration.h not available at compile time.\" << std::endl;
    #  endif
    #endif
        return 0;
    }
