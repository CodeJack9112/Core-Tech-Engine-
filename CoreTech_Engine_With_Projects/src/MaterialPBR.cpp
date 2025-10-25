#include "MaterialPBR.h"
#include <iostream>

namespace CoreTech {

MaterialPBR::MaterialPBR() {
    // default properties
}

void MaterialPBR::BindParameters() {
    // Bind textures and parameters to the shader used in geometry pass
    // Implementation is API-specific and left as a hook for integrator
    std::cout << "Binding PBR material parameters (stub)\n";
}

}