
ECS (Entity-Component-System) added
-----------------------------------
Files added under include/ecs and src/ecs provide a simple component system:

- include/ecs/Entity.h
- include/ecs/Component.h
- include/ecs/ComponentManager.h
- include/ecs/EntityManager.h
- include/ecs/System.h
- include/ecs/TransformComponent.h
- src/ecs/MovementSystem.cpp  (example system)

How to use:
- Include headers via "ecs/ComponentManager.h" and "ecs/EntityManager.h".
- The system is minimal: components are stored by type and entity id.
- This is a simple, easy-to-understand ECS intended as a starting point.
Notes:
- For production performance you may want to switch to contiguous storage per component type (arrays) and a faster entity recycling scheme.
- The MovementSystem.cpp is an example showing how to iterate and update components.

Updated: storage now uses contiguous ComponentStorage per type and ScriptEngine supports simple text commands.

ScriptEngine now supports variable assignment (e.g., id = createEntity Name), $variable substitution, and named entity lookup. Host functions can be registered to expose engine APIs to other scripting languages.
