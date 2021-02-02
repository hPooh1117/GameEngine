#include "HF_GlobalVariables.hlsli"

//--------------------------------------------------------------------------------

struct VS_Input
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};

struct HS_Input
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
	float3 w_pos    : TEXCOORD1;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};

//--------------------------------------------------------------------------------

HS_Input VSmain(VS_Input input)
{
	HS_Input output = (HS_Input)0;
	output.position = input.position;
	output.normal = input.normal;
	output.texcoord = input.texcoord;
	output.color = float4(1, 1, 1, 1);
	output.w_pos = input.position;
	output.tangent = input.tangent;
	output.binormal = input.binormal;
	return output;
}
