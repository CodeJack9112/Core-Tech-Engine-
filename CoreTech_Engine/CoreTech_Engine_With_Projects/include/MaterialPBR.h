#pragma once
#include "Material.h"
namespace CoreTech {

class MaterialPBR : public Material {
public:
    MaterialPBR();
    virtual void BindParameters() override;
    // Metallic/Roughness/AO textures, Albedo, normal map
};

}