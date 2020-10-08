#include "static_mesh.hlsli"


Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT input) : SV_TARGET
{
    return diffuse_map.Sample(diffuse_map_sampler_state, input.texcoord) * input.color;
}