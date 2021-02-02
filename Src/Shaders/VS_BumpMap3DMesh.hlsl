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

struct PS_Input
{
	float4 position   : SV_POSITION;
	float4 color      : COLOR;
	float2 texcoord   : TEXCOORD0;
	float3 w_pos      : TEXCOORD1;
	float3 w_normal   : TEXCOORD2;
	float3 w_tangent  : TEXCOORD3;
	float3 w_binormal : TEXCOORD4;
};

//--------------------------------------------------------------------------------

PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);
	N = normalize(N);
	float3 T = mul(input.tangent, (float3x3)world);
	T = normalize(T);
	float3 B = mul(input.binormal, (float3x3)world);
	B = normalize(B);

	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_tangent = T;
	output.w_binormal = B;

	return output;
}

//--------------------------------------------------------------------------------
