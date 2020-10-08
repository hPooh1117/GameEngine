#include "fog.hlsli"
#include "Functions.hlsli"

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);

    float3 N = normalize(input.w_normal);
    float3 E = normalize(eye_pos.xyz - input.w_normal);
    float3 L = normalize(light_dir.xyz);

    float3 A = ambient_color.rgb * hemiSphereLight(N, float3(0, 0, 1), float3(1, 0, 0));

    float3 C = light_color.rgb;
    float3 Kd = float3(1, 1, 1);
    float3 D = diffuse(N, L, C, Kd);

    float3 Ks = float3(1, 1, 1);
    float shininess = 20;
    float3 S = blinnPhongSpecular(N, L, C, E, Ks, shininess);

    color *= input.color * float4(A + D + S, 1.0f);

    color.rgb = fog(color.rgb, eye_pos.xyz, input.w_pos, fogColor, fogNear, fogFar);
    return color;
}