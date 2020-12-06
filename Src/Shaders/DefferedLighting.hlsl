#include "H_ToGBuffer.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_2DSprite.hlsli"
#include "H_Functions.hlsli"
#include "H_Light.hlsli"
#include "H_ShadowMap.hlsli"

Texture2D albedo_texture : register(t0);
SamplerState decal_sampler : register(s0);
SamplerState border_sampler : register(s1);
SamplerState clamp_sampler : register(s2);

Texture2D normal_texture : register(t1);
Texture2D position_texture : register(t2);
Texture2D shadow_texture : register(t3);
Texture2D<float4> depth_texture : register(t4);


struct PS_InputD
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float2 projPos : TEXCOORD1;
};

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_InputD VSmain(VS_Input input)
{
	PS_InputD output = (PS_InputD)0;

	float4 P = float4(input.position, 1);


	output.position = P;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.projPos.x = output.texcoord.x * 2.0 - 1.0;
	output.projPos.y = (1.0 - output.texcoord.y) * 2.0 - 1.0;
	return output;
}


float LinearizeDepth(float depth, float near, float far)
{
	return (2.0 * near) / (far + near - depth * (far - near));
}


//
// pixel shader
//
PS_Output_Light PSmain(PS_InputD input)
{
	PS_Output_Light output = (PS_Output_Light)0;

	const float depth = depth_texture.Sample(decal_sampler, input.texcoord).r;
	float2 uv = float2(input.texcoord.x * 2.0 - 1.0, (1.0 - input.texcoord.y) * 2.0 - 1.0);
	//float4 projP = float4(input.projPos.xy, depth, 1);
	float4 projP = float4(uv, depth, 1);

	//float4 viewP = mul(projP, inv_proj);
	//viewP /= viewP.w;

	float4 wPos = projP;
	//wPos = mul(inv_viewproj, wPos);
	//wPos.xyz /= wPos.w;
	//wPos = mul(wPos, inv_proj_mat);
	//wPos = mul(wPos, inv_view_mat);
	//wPos.xyz /= wPos.w;
	wPos = mul(inv_proj_mat, wPos);
	//wPos.xyz /= wPos.w;
	wPos = mul(inv_view_mat, wPos);

	wPos.xyz /= wPos.w;

	float3 P = position_texture.Sample(decal_sampler, input.texcoord).xyz;/*wPos.xyz;*/
	float3 N = normal_texture.Sample(decal_sampler, input.texcoord).xyz;
	float3 E = normalize(eye_pos.xyz - P);
	float3 L = normalize(light_dir.xyz);

	float3 A = ambient_color.rgb;
	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, shininess);

	// ポイントライト
	float3 PL;
	float3 PC;
	float influence;
	float3 PD = (float3)0;
	float3 PS = (float3)0;

	int i;
	float distance = 0.0;
	float range = 0.0;

	for (i = 0; i < POINTMAX; ++i)
	{
		if ((light_config & 2) == 0) break;
		if (point_lights[i].type == 0) continue;
		PL = P - point_lights[i].pos.xyz;
		distance = length(PL);

		range = point_lights[i].range;
		if (distance > range) continue;

		influence = saturate(1.0f - distance / range);
		PL = normalize(PL);
		PC = point_lights[i].color.rgb;
		PD += diffuse(N, PL, PC, Kd) * influence * influence;
		PS += blinnPhongSpecular(N, PL, PC, E, Ks, 20) * influence * influence;
	}

	//スポットライト
	//float3 SpotLight = (float3)0;
	//float3 SpotLightColor = (float3)0;
	//float3 SpotDiffuse = (float3)0;
	//float3 SpotSpecular = (float3)0;

	//for (i = 0; i < SPOTMAX; ++i)
	//{
	//	if ((light_config & 4) == 0) break;

	//	if (spot_lights[i].type == 0.0f) continue;

	//	SpotLight = P - spot_lights[i].pos.xyz;
	//	float d = length(SpotLight);

	//	float r = spot_lights[i].range;
	//	if (d > r) continue;

	//	float3 SpotFront = normalize(spot_lights[i].dir.xyz);
	//	SpotLight = normalize(SpotLight);

	//	float angle = dot(SpotLight, SpotFront);
	//	float area = spot_lights[i].inner_corn - spot_lights[i].outer_corn;

	//	float influence2 = spot_lights[i].inner_corn - angle;
	//	influence2 = saturate(1.0 - influence2 / area);
	//	influence = 1;
	//	SpotLightColor = spot_lights[i].color.rgb;
	//	SpotDiffuse += diffuse(N, SpotLight, SpotLightColor, Kd) * influence * influence * influence2;
	//	SpotSpecular += blinnPhongSpecular(N, SpotLight, SpotLightColor, E, Ks, 20) * influence * influence * influence2;
	//}


	float4 shadow = HasShadowMap(gTextureConfig) > 0 ? shadow_texture.Sample(decal_sampler, input.texcoord) : 1.0;
	float4 albedo = albedo_texture.Sample(decal_sampler, input.texcoord);

	float4 skyboxAlbedo = (depth < 0.1) * albedo;
	skyboxAlbedo.a = 1;


	float4 reflect = float4(1, 1, 1, 1);

	output.diffuse = float4(D + PD/* + SpotDiffuse*/, 1) * reflect;
	output.specular = float4(S + PS/* + SpotSpecular*/, 1) * reflect;
	output.prelighting = float4(output.diffuse.rgb + output.specular.rgb + A * reflect.rgb, 1) * albedo * shadow + skyboxAlbedo * light_color;
	

	//output.prelighting = float4(P, 1);
	output.skybox = skyboxAlbedo;
	return output;
}


