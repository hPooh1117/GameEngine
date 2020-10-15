#include "H_DataFormat.hlsli"
#include "H_Functions.hlsli"
#include "H_DirectionalLight.hlsli"

//
// pixel shader
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
    float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

    color *= input.color * float4(A + D + S, 1.0f);
    return color;

};