#include "H_ToGBuffer.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_2DSprite.hlsli"
#include "H_Functions.hlsli"
#include "H_DirectionalLight.hlsli"
#include "H_ShadowMap.hlsli"

Texture2D albedo_texture : register(t0);
SamplerState decal_sampler : register(s0);

Texture2D normal_texture : register(t1);
Texture2D position_texture : register(t2);
Texture2D shadow_texture : register(t3);

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
// pixel shader
//
PS_Output_Light PSmain(PS_Input input)
{
	PS_Output_Light output = (PS_Output_Light)0;

	float3 P = position_texture.Sample(position_sampler, input.texcoord).xyz;
	float3 N = normal_texture.Sample(normal_sampler, input.texcoord).xyz;
	float3 E = normalize(eye_pos.xyz - P);
	float3 L = normalize(light_dir.xyz);

	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 30);

	output.diffuse = float4(D, 1);
	output.specular = float4(S, 1);
	return output;
}


float4 PSmain2(PS_Input input) : SV_TARGET
{
	float3 DS = light_texture.Sample(light_sampler, input.texcoord).xyz;



	return float4(DS, 1);
}