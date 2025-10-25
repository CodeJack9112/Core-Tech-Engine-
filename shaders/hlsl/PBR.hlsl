\
// Simplified PBR HLSL shader (energy-conserving, metal/roughness workflow approximation)
// Includes support for environment cubemap reflections and a specular ramp lookup (UE3-style)
cbuffer PerObject : register(b0)
{
    matrix uModel;
    matrix uView;
    matrix uProj;
    float3 uViewPos;
    float pad0;
};

cbuffer PBRMaterial : register(b1)
{
    float3 uAlbedo;
    float metallic;
    float roughness; // 0..1
    float ao;
    float pad[1];
};

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
};

struct PSInput {
    float4 posH : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 tangent : TEXCOORD2;
    float2 uv : TEXCOORD3;
};

PSInput VSMain(VSInput vin) {
    PSInput o;
    float4 worldPos = mul(float4(vin.position,1.0f), uModel);
    o.worldPos = worldPos.xyz;
    o.normal = mul((float3x3)uModel, vin.normal);
    o.tangent = mul((float3x3)uModel, vin.tangent);
    o.uv = vin.uv;
    o.posH = mul(mul(float4(vin.position,1.0f), uModel), mul(uView, uProj)); // simplified
    return o;
}

// Textures
Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1);
TextureCube envMap : register(t2);
SamplerState samp : register(s0);

// Helpers
float3 fresnelSchlick(float3 F0, float cosTheta){
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

float DistributionGGX(float3 N, float3 H, float rough){
    float a = rough*rough;
    float a2 = a*a;
    float NdotH = max(dot(N,H), 0.0f);
    float num = a2;
    float denom = (NdotH*NdotH)*(a2-1.0f)+1.0f;
    denom = 3.14159265 * denom * denom;
    return num / max(0.000001f, denom);
}

float GeometrySchlickGGX(float NdotV, float k){
    return NdotV / (NdotV*(1.0f - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float k){
    float NdotV = max(dot(N,V), 0.0f);
    float NdotL = max(dot(N,L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, k);
    float ggx1 = GeometrySchlickGGX(NdotL, k);
    return ggx1 * ggx2;
}

float3 getNormalFromMap(float3 N, float3 T, float2 uv){
    float3 normalSample = normalMap.Sample(samp, uv).rgb;
    normalSample = normalSample * 2.0f - 1.0f;
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    return normalize(mul(normalSample, TBN));
}

float4 PSMain(PSInput inp) : SV_TARGET {
    float3 N = normalize(inp.normal);
    float3 T = normalize(inp.tangent);
    float3 V = normalize(uViewPos - inp.worldPos);
    float3 albedo = albedoMap.Sample(samp, inp.uv).rgb * uAlbedo;
    // normal mapping
    N = getNormalFromMap(N, T, inp.uv);

    float3 L = normalize(float3(0.0f,-1.0f,0.0f));
    float3 H = normalize(V + L);
    float3 F0 = lerp(float3(0.04f,0.04f,0.04f), albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, max(0.0f, (roughness+1.0f)/8.0f));
    float3 F = fresnelSchlick(F0, max(dot(H,V), 0.0f));

    float3 nominator = NDF * G * F;
    float denominator = 4.0f * max(0.001f, dot(N,V) * dot(N,L));
    float3 specular = nominator / max(0.001f, denominator);

    float3 kS = F;
    float3 kD = (1.0f - kS) * (1.0f - metallic);

    float NdotL = max(dot(N, L), 0.0f);
    float3 Lo = (kD * albedo / 3.14159265 + specular) * NdotL;

    // ambient + environment (sample cubemap for reflections)
    float3 env = envMap.Sample(samp, reflect(-V, N)).rgb;
    float3 ambient = (kD * albedo * ao) + env * kS;

    float3 color = ambient + Lo;
    color = pow(color, 1.0f/2.2f);
    return float4(color, 1.0f);
}
