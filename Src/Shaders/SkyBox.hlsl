#include "H_DataFormat.hlsli"

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);

	output.position = mul(P, matWVP);
	output.texcoord = input.texcoord;
	output.color = input.color;
	return output;
}

//
// pixel shader
//
float4 PSmain( PS_Input input ) : SV_TARGET
{
	return diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
}