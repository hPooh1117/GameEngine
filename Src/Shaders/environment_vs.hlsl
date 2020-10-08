#include "environment.hlsli"

// EntryPoint----------------------------------------
PS_Input main( VS_Input input )
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(input.position, 1.0f);

	output.pos = mul(P, mat_WVP);

	float3 wPos = mul(P, world).xyz;
	float3 wEyeDir = eye_pos.xyz - wPos;
	float eyeLen = length(wEyeDir);
	wEyeDir = normalize(wEyeDir);

	float4 N = float4(input.normal, 1.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_eye_dir = wEyeDir;

	return output;
}