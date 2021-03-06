#include "H_DataFormat.hlsli"


Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// Vertex Shader(default)
//
PS_Input VSmain( VS_Input input )
{
	PS_Input output = (PS_Input)0;
	output.position = mul(float4(input.position, 1), matWVP);
	output.texcoord = input.texcoord;
	output.color = input.color;
	return output;
}

float4 PSmain(PS_Input input) : SV_TARGET
{
	return input.color;
}