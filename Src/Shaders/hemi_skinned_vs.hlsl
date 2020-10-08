#include "phong.hlsli"




PS_Input main(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(input.position, 1.0);

	output.pos = mul(P, mat_WVP);

	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 1.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	return output;
}