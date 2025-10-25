// MW3-style cinematic HLSL (original)
cbuffer Params : register(b0) { float uContrast; float uVignette; float3 uWarmth; }
Texture2D albedoTex : register(t0);
SamplerState samp : register(s0);
float4 PSMain(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target {
    float3 base = albedoTex.Sample(samp, uv).rgb;
    base = ((base - 0.5) * uContrast) + 0.5;
    base *= uWarmth;
    // vignette
    float2 posc = uv - 0.5;
    float vig = smoothstep(0.8, uVignette + 0.4, length(posc));
    base *= lerp(1.0, 0.6, vig);
    return float4(saturate(base), 1.0);
}
