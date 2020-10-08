#include "H_ToGBuffer.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_2DSprite.hlsli"
#include "H_Functions.hlsli"
#include "H_PointLight.hlsli"

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


	output.position = P;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	return output;
}




//
// pixel shader(for multi rtvs)
//
PS_Output_Light PSmain(PS_Input input)
{
	PS_Output_Light output = (PS_Output_Light)0;

	float3 P = position_texture.Sample(position_sampler, input.texcoord).xyz;
	float3 N = normal_texture.Sample(normal_sampler, input.texcoord).xyz;
	float3 E = normalize(eye_pos.xyz - P);
	float3 L = normalize(light_dir.xyz);

	float3 A = ambient_color.rgb;

	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 30);

	// ポイントライト
	float3 PL;
	float3 PC;
	float influence;
	float3 PD = (float3)0;
	float3 PS = (float3)0;

	for (int i = 0; i < POINTMAX; ++i)
	{
		if (point_lights[i].type == 0) continue;
		PL = P - point_lights[i].pos.xyz;
		float d = length(PL);

		float r = point_lights[i].range;
		if (d > r) continue;

		influence = saturate(1.0f - d / r);
		PL = normalize(PL);
		PC = point_lights[i].color.rgb;
		PD += diffuse(N, PL, PC, Kd) * influence * influence;
		PS += blinnPhongSpecular(N, PL, PC, E, Ks, 20) * influence * influence;
	}

	output.diffuse = float4(D + A + PD, 1);
	output.specular = float4(S + PS, 1);
	return output;
}

