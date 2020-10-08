#include "H_2DSprite.hlsli"

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	output.position = float4(input.position, 1);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

//
// pixel shader
//
float4 PSmain(PS_Input input): SV_TARGET
{
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
	return color;
}