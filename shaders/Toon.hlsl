// Simple HLSL pixel/vertex combo for toon shading (minimal)
cbuffer Transform : register(b0) {
  matrix uModel; matrix uView; matrix uProj; float uLevels;
};
struct VS_IN { float3 pos : POSITION; float3 normal : NORMAL; float2 uv : TEXCOORD0; };
struct VS_OUT { float4 pos : SV_POSITION; float3 normal : NORMAL; float3 viewPos : TEXCOORD0; float2 uv : TEXCOORD1; };
VS_OUT vs_main(VS_IN IN) {
  VS_OUT OUT;
  float4 worldPos = mul(float4(IN.pos,1), uModel);
  OUT.pos = mul(worldPos, mul(uView, uProj));
  OUT.viewPos = mul(worldPos, uView).xyz;
  OUT.normal = mul((float3x3)uModel, IN.normal);
  OUT.uv = IN.uv;
  return OUT;
}
Texture2D albedoTex : register(t0);
SamplerState samp : register(s0);
float3 uLightDir : register(b1);
float3 uLightColor : register(b2);
float3 uAmbient : register(b3);
float uRimPower : register(b4);
float3 uRimColor : register(b5);
float4 ps_main(VS_OUT IN) : SV_Target {
  float3 N = normalize(IN.normal);
  float ndotl = max(dot(N, -normalize(uLightDir)), 0);
  float quant = floor(ndotl * uLevels) / (uLevels - 1);
  float3 al = albedoTex.Sample(samp, IN.uv).rgb;
  float3 col = al * (uAmbient + uLightColor * quant);
  float3 V = normalize(-IN.viewPos);
  float rim = pow(max(0, 1 - dot(N, V)), uRimPower);
  col += uRimColor * rim;
  return float4(col, 1.0);
}
