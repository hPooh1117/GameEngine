#include "plane_batch.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState decal_sampler : register(s0);

float4 main(PS_Input input) : SV_TARGET
{
	return diffuse_map.Sample(decal_sampler, input.texcoord) * input.color;
}