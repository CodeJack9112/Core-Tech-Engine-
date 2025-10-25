
Added Features Summary
----------------------
- SceneManager: save/load simple scenes (serializes Transform components to a JSON-like file).
- PhysicsSystem: simple gravity and naive collision demo (no external physics library required).
- InputManager: Win32 GetAsyncKeyState-based input wrapper (works on Windows).
- ResourceManager: simple text file cache for resources.
- RenderSystem: stub renderer that prints camera/mesh entities (placeholder for real renderer).
- Main demo updated to run a full demo loop using the above systems.

Limitations & Next Steps
- Scene serialization handles only TransformComponent currently. Extend to more components.
- Physics is a simple demo; replace with Bullet or other physics engine for production.
- RenderSystem is a stub; integrate with GPU renderer (OpenGL/DirectX) to draw meshes.
- InputManager uses Win32 API; for cross-platform, integrate SDL or GLFW.
- ResourceManager currently caches raw text; add binary loaders (textures/models).

Added sample model: assets/models/sample_cube.obj (with sample_cube.mtl and sample_cube_tex.bmp) and extended AssimpLoader to load all meshes/materials.

Added PBR shader (Cook-Torrance) and material mapping: albedo, normal, ORM (occlusion, roughness, metallic). Generated sample PBR textures for sample cube.
