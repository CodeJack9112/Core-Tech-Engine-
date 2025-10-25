# Physics Integration (Bullet) - Instructions

This package contains a Bullet physics integration for your Core Tech.

Files included:
- include/PhysicsManager.h
- include/PhysicsComponent.h
- src/PhysicsManager.cpp
- src/PhysicsComponent.cpp
- demo/scene_setup.cpp (example snippet to create ground + falling cube)
- CMakeLists_bullet.txt (instructions/snippet to add to your CMake)

## Using system Bullet (recommended)
On Debian/Ubuntu:
  sudo apt update
  sudo apt install libbullet-dev

Modify your project's CMakeLists.txt:
1. Add the two new source files to your sources list:
   src/PhysicsManager.cpp
   src/PhysicsComponent.cpp
2. Add the CMake snippet to find Bullet and link libraries (see CMakeLists_bullet.txt)

## Initialize physics in GameEngine
In GameEngine::initialize():
  m_physics = std::make_unique<PhysicsManager>();
  if(!m_physics->initialize()) { /* handle error */ }
  // create demo scene
  createDemoScene(*m_physics);

In GameEngine::run() main loop, step physics each frame:
  double dt = ...; // frame dt
  m_physics->stepSimulation(dt);

## Sync transforms
- To display Bullet-simulated objects, fetch their world transforms via PhysicsComponent::getTransform()
  and convert to your Core Tech's model matrix format, then set TinyModel::setModelMatrix(...).
- If you move an object via the editor gizmo, call PhysicsComponent::setKinematic(true) and setTransform(...)
  while dragging; when released, re-enable dynamic simulation by setKinematic(false) and optionally set linear velocity.

## Debug draw
- The current implementation does not include a full Bullet debug drawer adapter. You can implement a class deriving
  from btIDebugDraw and forward line draw calls to your GLRenderer to see collision shapes.

If you want I can:
- Patch your existing project directly with these files and update your CMakeLists.txt and GameEngine sources.
- Add btIDebugDraw adapter and wire debug draw into the renderer + UI toggle.
- Implement creation of physics bodies from TinyModel mesh vertex data (convex hull generation).

Which next? (I can apply the patch now to your repo and produce a zip with the full project.)


This physics integration is part of the Core Tech (CTE) demo.

Project identifier: CoreTech
Human name: Core Tech
