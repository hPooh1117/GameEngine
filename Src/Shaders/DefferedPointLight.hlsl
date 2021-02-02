#include "H_ToGBuffer.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_2DSprite.hlsli"
#include "H_Functions.hlsli"
#include "HF_GlobalVariables.hlsli"
#include "H_ShadowMap.hlsli"

Texture2D albedo_texture : register(t0);
SamplerState decal_sampler : register(s0);
SamplerState border_sampler : register(s1);
Texture2D normal_texture : register(t1);
Texture2D position_texture : register(t2);
Texture2D shadow_texture : register(t2);
Texture2D<float4> depth_texture : register(t3);

struct PS_InputDeffered
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD0;
	float2 projPos : TEXCOORD1;
};



//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_InputDeffered VSmain(VS_Input input)
{
	PS_InputDeffered output = (PS_InputDeffered)0;

	float4 P = float4(input.position, 1);


	output.position = P;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.projPos = output.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);

	return output;
}


float UnLinearizeDepth(float depth, float near, float far)
{
	float output = (far + near - 2.0 * near / depth) / (far - near);
	;
	return output;
}

//
// pixel shader(for multi rtvs)
//
PS_Output_Light PSmain(PS_InputDeffered input)
{
	PS_Output_Light output = (PS_Output_Light)0;

	float  depth = depth_texture.Sample(border_sampler, input.texcoord).r;
	//depth = UnLinearizeDepth(depth, 0.1, 1000.0);
	float4 projP = float4(input.projPos.xy, depth, 1);
	float4 wP = mul(projP, inv_viewproj);
	wP.xyz /= wP.w;

	float3 P = wP.xyz;
	float3 N = normal_texture.Sample(decal_sampler, input.texcoord).xyz;
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

	[unroll]
	for (uint i = 0; i < POINTMAX; ++i)
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



	float4 shadow = shadow_texture.Sample(decal_sampler, input.texcoord);
	float4 albedo = albedo_texture.Sample(decal_sampler, input.texcoord);

	float Length = length(eye_pos.xyz - P.xyz);
	float4 skyboxAlbedo = step(450.0, Length) * albedo;
	skyboxAlbedo.a = 1;

	output.diffuse = float4(D + PD, 1) * albedo;
	output.specular = float4(S + PS, 1) * albedo;
	output.prelighting = float4(D + A + PD + S + PS + skyboxAlbedo.xyz, 1) * shadow * albedo;
	output.skybox = skyboxAlbedo * light_color;
	return output;
}

