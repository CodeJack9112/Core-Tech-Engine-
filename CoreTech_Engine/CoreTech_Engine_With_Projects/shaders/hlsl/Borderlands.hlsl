// Borderlands-like stylized shader (HLSL)
cbuffer Matrices : register(b0) { matrix uModel; matrix uView; matrix uProj; matrix uNormalMatrix; }
cbuffer Params : register(b1) { float uLevels; float3 uLightDir; float3 uLightColor; float3 uBaseColor; float uEdgeBias; float3 uViewPos; }

struct VS_IN { float3 pos : POSITION; float3 normal : NORMAL; float2 uv : TEXCOORD0; };
struct VS_OUT { float4 pos : SV_POSITION; float3 worldPos : TEXCOORD0; float3 normal : TEXCOORD1; float2 uv : TEXCOORD2; };

VS_OUT VS(VS_IN input) {
  VS_OUT o;
  float4 wp = mul(float4(input.pos,1), uModel);
  o.worldPos = wp.xyz;
  o.normal = mul((float3x3)uNormalMatrix, input.normal);
  o.uv = input.uv;
  o.pos = mul(mul(mul(float4(input.pos,1), uModel), uView), uProj);
  return o;
}

float4 PS(VS_OUT input) : SV_Target {
  float NdotL = saturate(dot(normalize(input.normal), -normalize(uLightDir)));
  float quant = floor(NdotL * uLevels) / max(uLevels - 1.0, 1.0);
  float3 viewDir = normalize(uViewPos - input.worldPos);
  float edge = pow(1.0 - abs(dot(viewDir, normalize(input.normal))), 1.5);
  float edgeFactor = saturate(edge * uEdgeBias);
  float3 color = uBaseColor * uLightColor * quant * (1.0 - 0.6*edgeFactor);
  return float4(color, 1.0);
}

technique11 BorderTech { pass P0 { SetVertexShader( CompileShader( vs_5_0, VS() ) ); SetPixelShader( CompileShader( ps_5_0, PS() ) ); } }
