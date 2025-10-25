// Borderlands-like HLSL (outline + posterize)
cbuffer Common : register(b0) {
  matrix uModel; matrix uView; matrix uProj;
  float uEdgeThreshold; float uToneLevels; float3 uCameraPos; float3 uLightDir;
};
struct VS_IN { float3 pos : POSITION; float3 normal : NORMAL; float2 uv : TEXCOORD0; };
struct VS_OUT { float4 pos : SV_POSITION; float3 worldPos : TEXCOORD0; float3 normal : NORMAL; float2 uv : TEXCOORD1; };
VS_OUT vs_main(VS_IN IN) {
  VS_OUT OUT;
  float4 world = mul(float4(IN.pos,1), uModel);
  OUT.worldPos = world.xyz;
  OUT.pos = mul(world, mul(uView, uProj));
  OUT.normal = mul((float3x3)uModel, IN.normal);
  OUT.uv = IN.uv;
  return OUT;
}
Texture2D albedoTex : register(t0);
SamplerState samp : register(s0);
float4 ps_main(VS_OUT IN) : SV_Target {
  float3 N = normalize(IN.normal);
  float3 V = normalize(uCameraPos - IN.worldPos);
  float diff = max(dot(N, normalize(uLightDir)), 0);
  float tone = floor(diff * uToneLevels) / (uToneLevels - 1);
  float3 base = albedoTex.Sample(samp, IN.uv).rgb * tone;
  float rim = pow(max(0, 1 - dot(N, V)), 2);
  float inkMask = step(0.35, rim) * (1 - tone);
  base = lerp(base, float3(0,0,0), inkMask);
  return float4(base,1);
}
