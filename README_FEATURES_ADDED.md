
Added prototype implementations for the following engine systems:
- Bullet Physics integration (FetchContent) and PhysicsComponent + PhysicsSystem prototypes.
- miniaudio integration (FetchContent) and AudioManager implementation (uses miniaudio if available).
- Scene serialization extended (basic) and Save/Load UI in ImGui editor.
- Lua REPL in gameplay_demo for interactive scripting at runtime.
- AnimationComponent and Animator stub.

These are designed as starter implementations for contributors to extend into full systems.
