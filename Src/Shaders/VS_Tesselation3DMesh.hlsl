#include "HF_GlobalVariables.hlsli"

//--------------------------------------------------------------------------------

struct VS_Input
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
};

struct HS_Input
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
	float3 w_pos    : TEXCOORD1;
	float3 w_normal : TEXCOORD2;
};

//--------------------------------------------------------------------------------

HS_Input VSmain(VS_Input input)
{
	HS_Input output = (HS_Input)0;

	// èoóÕílê›íË
	float4 P = float4(input.position, 1.0);
	float3 wP = mul(P, world).xyz;

	float3 N = mul(input.normal, (float3x3)world);

	output.position = P.xyz;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = normalize(N);
	output.w_pos = wP;
	return output;
}
