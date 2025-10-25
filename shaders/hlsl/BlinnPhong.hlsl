\
// Blinn-Phong HLSL shader for DirectX (compatible with shader model 5.0)
cbuffer PerObject : register(b0)
{
    matrix uModel;
    matrix uView;
    matrix uProj;
    float3 uViewPos;
    float pad0;
};

cbuffer MaterialCB : register(b1)
{
    float3 uAlbedo;
    float uSpecPower;
    float uSpecIntensity;
    float uRoughness;
    float pad1[3];
};

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

struct PSInput {
    float4 posH : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

PSInput VSMain(VSInput vin) {
    PSInput o;
    float4 worldPos = mul(float4(vin.position,1.0f), uModel);
    o.worldPos = worldPos.xyz;
    o.normal = mul((float3x3)uModel, vin.normal);
    o.uv = vin.texcoord;
    o.posH = mul(mul(float4(vin.position,1.0f), uModel), mul(uView, uProj)); // simplified, for demo purposes
    return o;
}

// Texture samplers (bind manually in engine)
Texture2D albedoTex : register(t0);
SamplerState samp : register(s0);

float3 BlinnPhong(float3 N, float3 V, float3 L, float shininess, float intensity){
    float3 H = normalize(V + L);
    float NdotL = saturate(dot(N,L));
    float NdotH = saturate(dot(N,H));
    float spec = 0.0f;
    if(NdotL>0.0f) spec = pow(NdotH, shininess) * intensity;
    spec *= 1.0f / max(0.0001f, shininess * 0.02f);
    return float3(spec, spec, spec);
}

float4 PSMain(PSInput inp) : SV_TARGET {
    float3 N = normalize(inp.normal);
    float3 V = normalize(uViewPos - inp.worldPos);
    float3 L = normalize(float3(0.0f,-1.0f,0.0f)); // directional light from above (demo)
    float3 base = albedoTex.Sample(samp, inp.uv).rgb * uAlbedo;
    float3 diffuse = base * saturate(dot(N,L));
    float3 spec = BlinnPhong(N,V,L,uSpecPower,uSpecIntensity);
    float3 ambient = 0.08f * base;
    float3 color = ambient + diffuse + spec;
    return float4(pow(color, 1.0/2.2), 1.0f);
}
