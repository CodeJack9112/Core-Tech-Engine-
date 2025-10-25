#pragma once
#include <string>
#include <unordered_map>
struct ShaderCache { std::unordered_map<std::string, std::string> cache; void Precompile(const std::string& path); bool Has(const std::string& key) const; };
