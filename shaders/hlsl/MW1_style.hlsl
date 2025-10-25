// MW1-style cinematic HLSL (original)
cbuffer Params : register(b0) { float uContrast; float uSaturation; float uGrain; float3 uTint; }
Texture2D albedoTex : register(t0);
SamplerState samp : register(s0);

float4 PSMain(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target {
    float3 base = albedoTex.Sample(samp, uv).rgb;
    float3 lit = base; // lit would be computed by lighting stage in full renderer
    lit = ((lit - 0.5) * uContrast) + 0.5;
    float luma = dot(lit, float3(0.2126,0.7152,0.0722));
    lit = lerp(float3(luma,luma,luma), lit, uSaturation);
    lit *= uTint;
    // simple grain via hash
    float g = frac(sin(dot(uv, float2(12.9898,78.233))) * 43758.5453) * 2 - 1;
    lit += g * uGrain;
    return float4(saturate(lit), 1.0);
}
