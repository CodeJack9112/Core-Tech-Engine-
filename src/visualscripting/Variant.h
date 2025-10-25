#pragma once
// Variant.h - simple typed variant for Visual Scripting
#include <string>
#include <variant>
#include <sstream>

namespace CoreTech { namespace VSEnhanced {

using VariantValue = std::variant<std::monostate, int64_t, double, std::string, bool>;

struct Variant {
    VariantValue v;
    Variant() : v(std::monostate{}) {}
    Variant(int64_t i): v(i) {}
    Variant(double d): v(d) {}
    Variant(const std::string& s): v(s) {}
    Variant(const char* s): v(std::string(s)) {}
    Variant(bool b): v(b) {}

    std::string ToString() const {
        struct Visitor {
            std::string operator()(std::monostate) const { return "null"; }
            std::string operator()(int64_t i) const { return std::to_string(i); }
            std::string operator()(double d) const { std::ostringstream ss; ss<<d; return ss.str(); }
            std::string operator()(const std::string& s) const { return s; }
            std::string operator()(bool b) const { return b?"true":"false"; }
        };
        return std::visit(Visitor{}, v);
    }

    bool IsNull() const { return std::holds_alternative<std::monostate>(v); }
};

} } // namespace
