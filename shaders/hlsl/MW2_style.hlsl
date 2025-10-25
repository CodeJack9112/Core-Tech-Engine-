// MW2-style cinematic HLSL (original)
cbuffer Params : register(b0) { float uDesat; float uContrast; float uChromAb; float3 uColorGrade; }
Texture2D albedoTex : register(t0);
SamplerState samp : register(s0);
float4 PSMain(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target {
    float3 base = albedoTex.Sample(samp, uv).rgb;
    float l = dot(base, float3(0.2126,0.7152,0.0722));
    base = lerp(base, float3(l,l,l), uDesat);
    base = ((base - 0.5) * uContrast) + 0.5;
    base = lerp(base, base * uColorGrade, 0.12);
    // chromatic aberration omitted for performance in simple shader; requires multi-sample
    return float4(saturate(base), 1.0);
}
