#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
struct RGResource { std::string name; int width, height; };
struct RGNode { std::string name; std::vector<int> deps; std::function<void()> execute; std::vector<std::string> reads; std::vector<std::string> writes; };
struct RenderGraph { std::vector<RGNode> nodes; std::unordered_map<std::string, RGResource> resources;
  int AddResource(const RGResource& r); int AddNode(const RGNode& n); bool Compile(std::vector<int>& outOrder); void Execute(const std::vector<int>& order);
};
