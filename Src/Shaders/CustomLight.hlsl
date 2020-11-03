#include "H_DataFormat.hlsli"
#include "H_Functions.hlsli"

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

//----------------------------------
// スポットライト構造体
//----------------------------------
struct SPOTLIGHT
{
    float index;
    float range;
    float type;
    float inner_corn;
    float outer_corn;
    float dummy0;
    float dummy1;
    float dummy2;
    float4 pos;
    float4 color;
    float4 dir;
};
#define SPOTMAX 4

//----------------------------------
// ポイントライト構造体
//----------------------------------
struct POINTLIGHT
{
    float index;
    float range;
    float type;
    float dummy;
    float4 pos;
    float4 color;
};
#define POINTMAX 4

//----------------------------------
// グローバル変数
//----------------------------------
cbuffer CBPerLight : register(b2)
{
    float4 light_color;
    float4 light_dir;
    float4 ambient_color;
    float4 eye_pos;
    float  env_alpha;
    float  reflact;
    float  time;
    float  dummy2;
    POINTLIGHT point_lights[POINTMAX];
    SPOTLIGHT spot_lights[SPOTMAX];
};

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// Vertex Shader(default)
//
PS_Input VSmain(VS_Input input)
{
    PS_Input output = (PS_Input)0;
    float4 P = float4(input.position, 1);


    float3 wPos = mul(P, world).xyz;

    float4 N = float4(input.normal, 0);
    float3 wNormal = mul(N, world).xyz;
    wNormal = normalize(wNormal);


    output.color = mat_color;

    output.position = mul(P, matWVP);
    output.texcoord = input.texcoord;
    output.w_normal = wNormal;
    output.w_pos = wPos;

    return output;

}
//
// vertex shader(SkinnedMesh)
//
PS_Input VSmainS(VS_InputS input)
{
    PS_Input output = (PS_Input)0;
    float4 P = float4(input.position, 1);


    float3 wPos = mul(P, world).xyz;

    float4 N = float4(input.normal, 0);
    float3 wNormal = mul(N, world).xyz;
    wNormal = normalize(wNormal);


    output.color = mat_color;

    output.position = mul(P, matWVP);
    output.texcoord = input.texcoord;
    output.w_normal = wNormal;
    output.w_pos = wPos;

    return output;

}

PS_Input VSmainSkinning(VS_InputS input)
{
    PS_Input output = (PS_Input)0;

    // スキニング処理
    float4 p = { 0, 0, 0, 0 };
    float4 n = { 0, 0, 0, 0 };
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
        n += (input.bone_weights[i] * mul(float4(input.normal, 0), bone_transforms[input.bone_indices[i]]));
    }
    p.w = 1.0f;
    n.w = 0.0f;
    float4 N = normalize(mul(n, world));
    float4 L = normalize(-light_dir);


    float3 wPos = mul(p, world).xyz;

    float3 wNormal = mul(n, world).xyz;
    wNormal = normalize(wNormal);


    output.color = mat_color;

    output.position = mul(p, matWVP);
    output.texcoord = input.texcoord;
    output.w_normal = wNormal;
    output.w_pos = wPos;

    return output;

}


//
// Pixel Shader
//
float4 PSmain(PS_Input input) : SV_TARGET
{
    float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
    float3 N = normalize(input.w_normal);
    float3 E = normalize(eye_pos.xyz - input.w_pos);
    float3 L = normalize(light_dir.xyz);

    float3 A = ambient_color.rgb;

    float3 C = light_color.rgb;
    float3 Kd = float3(1, 1, 1);
    float3 D = diffuse(N, L, C, Kd);

    float3 Ks = float3(1, 1, 1);
    //float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

    // ポイントライト
    float3 PL;
    float3 PC;
    float influence;
    float3 PD = (float3)0;
    float3 PS = (float3)0;

    for (int i = 0; i < SPOTMAX; ++i)
    {
        if (point_lights[i].type == 0) continue;
        PL = input.w_pos - point_lights[i].pos.xyz;
        float d = length(PL);

        float r = point_lights[i].range;
        if (d > r) continue;

        influence = saturate(1.0f - d / r);
        PL = normalize(PL);
        PC = point_lights[i].color.rgb;
        PD += diffuse(N, PL, PC, Kd) * influence * influence;
        PS += blinnPhongSpecular(N, PL, PC, E, Ks, 20) * influence * influence;
    }

    //スポットライト
    float3 SpotLight;
    float3 SpotLightColor;
    float3 SpotDiffuse = (float3)0;
    float3 SpotSpecular = (float3)0;

    for (i = 0; i < SPOTMAX; ++i)
    {
        if (spot_lights[i].type == 0.0f) continue;

        SpotLight = input.w_pos - spot_lights[i].pos.xyz;
        float d = length(SpotLight);

        float r = spot_lights[i].range;
        if (d > r) continue;

        float3 SpotFront = normalize(spot_lights[i].dir.xyz);
        SpotLight = normalize(SpotLight);

        float angle = dot(SpotLight, SpotFront);
        float area = spot_lights[i].inner_corn - spot_lights[i].outer_corn;

        float influence2 = spot_lights[i].inner_corn - angle;
        influence2 = saturate(1.0 - influence2 / area);
        influence = 1;
        SpotLightColor = spot_lights[i].color.rgb;
        SpotDiffuse += diffuse(N, SpotLight, SpotLightColor, Kd) * influence * influence * influence2;
        SpotSpecular += blinnPhongSpecular(N, SpotLight, SpotLightColor, E, Ks, 30) * influence * influence * influence2;
    }

    color *= input.color * float4(A + D + /*S +*/ PD + PS + SpotDiffuse + SpotSpecular, 1.0f);
    return color;

};