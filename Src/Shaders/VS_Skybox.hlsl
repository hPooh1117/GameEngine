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
	float3 localPos : TEXCOORD;
};

//--------------------------------------------------------------------------------

PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	output.localPos = input.position;
	float4 P = float4(input.position, 1);
	output.position = mul(P, matWVP);
	output.position.z = output.position.w;

	return output;
}
