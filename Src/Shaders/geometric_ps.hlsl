#include "geometric_primitive.hlsli"


Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(PS_Input input) : SV_TARGET
{
    return diffuse_map.Sample(diffuse_map_sampler_state, input.texcoord) * input.color;
}