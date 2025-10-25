#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <queue>
#include <atomic>

namespace VisualScripting {

using NodeID = std::string;

// Lightweight value type with small-string optimization avoided for simplicity
struct Value {
    enum Type { NIL=0, FLOAT=1, INT=2, BOOL=3, STRING=4 } type = NIL;
    union {
        float f;
        int i;
        bool b;
    };
    std::string s; // used only when type==STRING
    Value(): type(NIL), f(0.0f) {}
    static Value makeFloat(float v){ Value x; x.type=FLOAT; x.f=v; return x; }
    static Value makeInt(int v){ Value x; x.type=INT; x.i=v; return x; }
    static Value makeBool(bool v){ Value x; x.type=BOOL; x.b=v; return x; }
    static Value makeString(const std::string& v){ Value x; x.type=STRING; x.s=v; return x; }
    void clear(){ if(type==STRING) s.clear(); type=NIL; f=0.0f; }
};

struct Node {
    NodeID id;
    std::string type; // e.g., "Print", "Add", "Spawn"
    std::string title;
    std::vector<std::pair<std::string, Value>> inputs;  // ordered to allow fast iteration
    std::vector<std::pair<std::string, Value>> outputs;
    // runtime callback invoked when node executes
    // signature should avoid allocations: receives preallocated buffers
    std::function<void(Node&, class ExecutionContext&)> exec;
    // flags
    bool pure = false; // side-effect free
    Node(): pure(false) {}
};

struct Link {
    size_t fromNodeIndex; // indices into Graph::nodesCompiled
    int fromOutputIndex;
    size_t toNodeIndex;
    int toInputIndex;
};

// Object pool for reuse (simple)
template<typename T>
class ObjectPool {
public:
    ObjectPool() = default;
    std::shared_ptr<T> acquire(){
        if(!pool.empty()){
            auto p = pool.back(); pool.pop_back();
            return p;
        }
        return std::make_shared<T>();
    }
    void release(std::shared_ptr<T> obj){
        if(obj) pool.push_back(obj);
    }
private:
    std::vector<std::shared_ptr<T>> pool;
};

struct Graph {
    std::string name;
    // nodesCompiled is the execution order after compile()
    std::vector<std::shared_ptr<Node>> nodesCompiled;
    std::vector<Link> links; // links refer to indices in nodesCompiled and input/output indices
    // original nodes for editing (not used during fast runtime)
    std::vector<std::shared_ptr<Node>> nodesEditable;

    // Compile the graph into an execution order and resolve link indices.
    // Returns true on success, false on cycle or error.
    bool compile();

    // Serialize/deserialize (JSON-light)
    std::string serialize() const;
    static Graph deserialize(const std::string& jsonStr);

    // Clear compiled runtime data (for editing)
    void clearCompiled(){ nodesCompiled.clear(); links.clear(); }
};

// Execution context and runtime
struct ExecutionContext {
    const Graph* graph = nullptr;
    // fast event bus and scheduler hooks
    std::function<void(const std::string&)> log = [](const std::string&){ };
    // schedule a delayed callback (seconds)
    std::function<void(std::function<void()>, float)> schedule = nullptr;
    // event emit: name -> payload (string)
    std::function<void(const std::string&, const std::string&)> emitEvent = nullptr;
    // resolve input given node index and input index (fast lookup)
    Value resolveInputIndex(size_t nodeIndex, int inputIndex) const;
};

// Runtime execute compiled graph (fast path)
// Executes nodes in nodesCompiled order using links mapping (no dynamic allocations)
bool executeCompiledGraph(const Graph& graph, ExecutionContext& ctx);

// Utility node factories (optimized node signatures)
std::shared_ptr<Node> makePrintNode(const std::string& message);
std::shared_ptr<Node> makeAddNode(float a=0.0f, float b=0.0f);
std::shared_ptr<Node> makeDelayNode(float seconds);

// Event-driven and gameplay nodes
std::shared_ptr<Node> makeEventEmitNode(const std::string& eventName);
std::shared_ptr<Node> makeEventListenerNode(const std::string& eventName);
std::shared_ptr<Node> makePlaySoundNode(const std::string& soundPath);

} // namespace VisualScripting
