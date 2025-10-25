#pragma once
#include "Entity.h"
#include <vector>
#include <string>
namespace CoreTech {
struct Bone { std::string name; int parent = -1; };
struct SkeletonComponent : public Component {
    std::vector<Bone> bones;
    std::string getType() const override { return "Skeleton"; }
};
}