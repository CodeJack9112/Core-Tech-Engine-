Additional Stylized Shaders (added)
==================================
The following stylized shaders were added to the engine's shaders/ folder without removing existing shaders:

GLSL:
  - Toon.vert.glsl / Toon.frag.glsl        : Cel / anime-style quantized lighting + rim light.
  - Borderlands.vert.glsl / Borderlands.frag.glsl : Posterized comic/ink style with simple silhouette-based edge darkening and hatch-ish effect.
  - Outline.vert.glsl / Outline.frag.glsl  : Inverted-hull outline shader (expand along normals) for solid black outline rendering.

HLSL (DirectX):
  - Toon.hlsl
  - Borderlands.hlsl

Usage notes:
  - Toon: set uniform 'uLevels' to integer number of tone bands (e.g. 3.0), 'uRimPower' to 2-6 for rim strength.
  - Borderlands: tune 'uToneLevels' and 'uEdgeThreshold' for stronger comic effect.
  - Outline: render a separate pass with depth or inverted-hull geometry using Outline.* and a solid black color.

Integration:
  - The shaders are provided as example GLSL/HLSL sources. You need to load/compile them with your renderer backend (RendererGL / DirectXRenderer).
  - For a full Borderlands effect, a multi-pass approach (albedo+normal/depth edge detect/post-process) yields better results; the fragment shader here is a simplified single-pass approximation.
