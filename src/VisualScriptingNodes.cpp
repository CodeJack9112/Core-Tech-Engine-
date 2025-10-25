#include "../include/VisualScripting.h"
#include "../include/AudioManager.h"
#include "../include/PhysicsManager.h"
#include <iostream>

using namespace VisualScripting;

// SpawnEntity node: spawns a simple physics object at position (x,y,z)
// This node requires PhysicsManager to be accessible via the ExecutionContext (we'll expect ctx.log to be used for demo)
std::shared_ptr<Node> createSpawnEntityNode(const std::string& name){
    auto n = std::make_shared<Node>();
    n->id = std::string("node_") + name;
    n->type = "SpawnEntity";
    n->title = "SpawnEntity";
    n->inputs["X"] = Value::makeFloat(0.0f);
    n->inputs["Y"] = Value::makeFloat(0.0f);
    n->inputs["Z"] = Value::makeFloat(0.0f);
    n->exec = [](Node& node, ExecutionContext& ctx){
        float x = ctx.resolveInput(node, "X").f;
        float y = ctx.resolveInput(node, "Y").f;
        float z = ctx.resolveInput(node, "Z").f;
        // In a full integration, we'd call PhysicsManager to create a rigid body here. For demo, print.
        if(ctx.log) {
            ctx.log(std::string("SpawnEntity: ") + node.id + " at (" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")");
        }
    };
    return n;
}

// PlaySound node: triggers AudioManager to play a sound
std::shared_ptr<Node> createPlaySoundNode(const std::string& soundPath){
    auto n = std::make_shared<Node>();
    n->id = std::string("node_sound_") + soundPath;
    n->type = "PlaySound";
    n->title = "PlaySound";
    n->inputs["Path"] = Value::makeString(soundPath);
    n->exec = [](Node& node, ExecutionContext& ctx){
        std::string path = ctx.resolveInput(node, "Path").s;
        if(ctx.log) ctx.log(std::string("PlaySound: ") + path);
        // If AudioManager available, one would call manager->loadSound / playSound. We simulate.
    };
    return n;
}

// SetTransform node: sets transform on an entity (stub)
std::shared_ptr<Node> createSetTransformNode(const std::string& entityId){
    auto n = std::make_shared<Node>();
    n->id = std::string("node_set_") + entityId;
    n->type = "SetTransform";
    n->title = "SetTransform";
    n->inputs["Tx"] = Value::makeFloat(0.0f);
    n->inputs["Ty"] = Value::makeFloat(0.0f);
    n->inputs["Tz"] = Value::makeFloat(0.0f);
    n->exec = [](Node& node, ExecutionContext& ctx){
        float x = ctx.resolveInput(node, "Tx").f;
        float y = ctx.resolveInput(node, "Ty").f;
        float z = ctx.resolveInput(node, "Tz").f;
        if(ctx.log) ctx.log(std::string("SetTransform on ") + node.id + " => (" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")");
    };
    return n;
}
// Additional node types: Event emitter/listener and Undo/Redo placeholders
#include "../include/VisualScripting.h"
using namespace VisualScripting;

std::shared_ptr<Node> createEventEmitNode(const std::string& eventName){
    auto n = std::make_shared<Node>();
    n->id = std::string("event_emit_") + eventName;
    n->type = "EventEmit"; n->title = "EventEmit";
    n->inputs["Trigger"] = Value::makeBool(false);
    n->inputs["Payload"] = Value::makeString("");
    n->exec = [eventName](Node& node, ExecutionContext& ctx){
        if(ctx.log) ctx.log(std::string("EventEmit: ") + eventName + " payload=" + ctx.resolveInput(node, "Payload").s);
        // In a full engine, this would notify listeners via an event bus.
    };
    return n;
}

std::shared_ptr<Node> createEventListenerNode(const std::string& eventName){
    auto n = std::make_shared<Node>();
    n->id = std::string("event_listen_") + eventName;
    n->type = "EventListener"; n->title = "EventListener";
    n->outputs["Payload"] = Value::makeString(""); // will be filled when event occurs (not implemented in demo)
    n->exec = [eventName](Node& node, ExecutionContext& ctx){
        // demo: just log that listener exists
        if(ctx.log) ctx.log(std::string("EventListener registered for: ") + eventName);
    };
    return n;
}

// Undo/Redo placeholders (no full implementation; hooks provided)
std::shared_ptr<Node> createUndoNode(){
    auto n = std::make_shared<Node>();
    n->id = std::string("undo_node_") + std::to_string(rand());
    n->type = "Undo"; n->title = "Undo";
    n->exec = [](Node& node, ExecutionContext& ctx){ if(ctx.log) ctx.log(std::string("Undo requested (placeholder)")); };
    return n;
}

std::shared_ptr<Node> createRedoNode(){
    auto n = std::make_shared<Node>();
    n->id = std::string("redo_node_") + std::to_string(rand());
    n->type = "Redo"; n->title = "Redo";
    n->exec = [](Node& node, ExecutionContext& ctx){ if(ctx.log) ctx.log(std::string("Redo requested (placeholder)")); };
    return n;
}
