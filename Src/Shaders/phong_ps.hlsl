#include "phong_obj.hlsli"
#include "function.hlsli"
//struct PS_Input
//{
//    float4 pos : SV_POSITION;
//    float4 color : COLOR;
//    float2 texcoord : TEXCOORD1;
//    float3 w_pos : TEXCOORD2;
//    float3 w_normal : TEXCOORD3;
//};
//
//cbuffer CBPerMatrix : register(bo)
//{
//    row_major float4x4 world;
//    row_major float4x4 mat_WVP;
//};
//
//cbuffer CBPerLight : register(b2)
//{
//    float4 light_dir;
//    float4 material_color;
//    float4 ambient_color;
//    float4 light_color;
//    float4 eye_pos;
//};


Texture2D diffuse_texture:register(t0);
SamplerState decal_sampler:register(s0);


float4 main(PS_Input input) : SV_TARGET
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
    float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

    color *= input.color * float4(A + D + S, 1.0f);
    return color;

};