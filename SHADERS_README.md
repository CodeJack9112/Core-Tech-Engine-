Toon and Borderlands Stylized Shaders (added)
============================================
Added shader files (GLSL & HLSL) that implement cel-shading and Borderlands-like stylized shading,
plus an outline extrusion shader for bold ink outlines.

Files added (GLSL):
  - shaders/CelShade.vert.glsl
  - shaders/CelShade.frag.glsl
  - shaders/OutlineExtrude.vert.glsl
  - shaders/Outline.frag.glsl
  - shaders/Borderlands.frag.glsl

Files added (HLSL):
  - shaders/hlsl/CelShade.hlsl
  - shaders/hlsl/Borderlands.hlsl

How to use:
  - Use the Outline material to render a backface-extruded pass first (draw backfaces only, depth-tested),
    then render the main toon material. This creates a solid ink outline effect.
  - Set uniform parameters (uLevels, uEdgeBias, uOutlineScale) to tune the look.
  - The HLSL files are provided for DirectX 11/12 renderer pipelines in your engine; you may need to adapt
    constant buffer bindings to match your engine's renderer conventions.

Notes:
  - I did NOT remove or change any existing shaders. All new shader files are additive.
  - I added JSON material descriptors in demo_assimp/materials/ that reference the shaders. Your engine's
    material loader will need to support loading shader paths from JSON or you can manually set them in code.

Cinematic "MW-style" shaders (original, inspired)
-------------------------------------------------
I added three original cinematic fragment shaders named:
  - shaders/MW1_style.frag.glsl  (gritty, high-contrast, film grain, subtle blue tint)
  - shaders/MW2_style.frag.glsl  (cold/desaturated, color grade, chromatic aberration placeholder)
  - shaders/MW3_style.frag.glsl  (warm, high contrast, vignette)
HLSL variants are present in shaders/hlsl/ as well.

These shaders are **original implementations** inspired by cinematic shooter aesthetics.
They are not copies of any proprietary shader code. They are additive — I did not delete or modify
any of your previous shader files.

Integrated cinematic shaders into ToonIntegration:
- Registered cinematic shader variants (cinematic_mw1/mw2/mw3) during InitializeToonMaterials (best-effort).
- ImGui controls available under "Cinematic Shaders" to tweak MW1/MW2/MW3 parameters and push to global uniforms (best-effort).
- If your renderer uses different API names for shader loading or uniform setting, adapt ToonIntegration.cpp or provide a small shim mapping.
