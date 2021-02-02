#include "HF_GlobalVariables.hlsli"

//--------------------------------------------------------------------------------

struct VS_Input
{
	float3 position : POSITION;
	float4 color    : COLOR;
	float2 texcoord : TEXCOORD;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR;
};

//--------------------------------------------------------------------------------

PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	output.position = mul(float4(input.position, 1), matWVP);
	output.texcoord = input.texcoord;
	output.color = input.color;
	return output;
}
