#include "H_ShadowMap.hlsli"
#include "H_Functions.hlsli"
#include "HF_GlobalVariables.hlsli"

//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D albedo_texture : register(t0);
SamplerState decal_sampler : register(s0);
Texture2D shadow_texture: register(t3);
Texture2D diffuse_texture : register(t5);
Texture2D specular_texture : register(t6);
Texture2D skybox_texture : register(t7);

Texture2D ambient_texture : register(t10);

Texture2D blurredAO_texture : register(t15);


static const int BLUR_SIZE = 5;

//--------------------------------------------
//	データレイアウト
//--------------------------------------------
struct VS_Input
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct PS_Output
{
	float4 result : SV_TARGET0;
	float4 postEffect : SV_TARGET1;
};

//************************************************************
//	Vertex Shader
//************************************************************
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(input.position, 1);
	
	output.position = P;
	output.color = input.color;
	output.texcoord = input.texcoord;

	return output;
}


//************************************************************
//	Pixel Shader
//************************************************************
//PS_Output PSmain(PS_Input input)
//{
//	PS_Output output = (PS_Output)0;
//
//	
//	float3 D = diffuse_texture.Sample(decal_sampler, input.texcoord).rgb;
//	float3 S = specular_texture.Sample(decal_sampler, input.texcoord).rgb;
//	float3 A = ambient_texture.Sample(decal_sampler, input.texcoord).rgb;
//
//	float2 textureInfo;
//	ambient_texture.GetDimensions(textureInfo.x, textureInfo.y);
//	float2 texelSize = 1.0 / textureInfo;
//	float result = 0.0;
//	float2 hlim = float2(-blurSize * 0.5 + 0.5, -blurSize * 0.5 + 0.5);
//	for (int i = 0; i < (int)blurSize; ++i)
//	{
//		for (int j = 0; j < (int)blurSize; ++j)
//		{
//			float2 offset = (hlim + float2(float(i), float(j))) * texelSize;
//			result += ambient_texture.Sample(decal_sampler, input.texcoord + offset.xy).r;
//		}
//	}
//	result /= float(BLUR_SIZE * BLUR_SIZE);
//
//	
//
//
//	float3 albedo = albedo_texture.Sample(decal_sampler, input.texcoord).xyz;
//	float3 shadow = shadow_texture.Sample(decal_sampler, input.texcoord).xyz;
//	float3 skybox = skybox_texture.Sample(decal_sampler, input.texcoord).xyz;
//
//
//	//output.result = float4(D * result + S * result, 1) * color;
//	output.result = float4((D * result * albedo * shadow + S * result * albedo * shadow + skybox), 1);
//	output.postEffect = float4(result, result, result, 1);
//	return output;
//}


PS_Output PSmain(PS_Input input)
{
	PS_Output output = (PS_Output)0;


	float3 D = diffuse_texture.Sample(decal_sampler, input.texcoord).rgb;
	float3 S = specular_texture.Sample(decal_sampler, input.texcoord).rgb;
	//float3 A = ambient_texture.Sample(decal_sampler, input.texcoord).rgb;

	//float2 textureInfo;
	//ambient_texture.GetDimensions(textureInfo.x, textureInfo.y);
	//float2 texelSize = 1.0 / textureInfo;
	//float result = 0.0;
	//float2 hlim = float2(-blurSize * 0.5 + 0.5, -blurSize * 0.5 + 0.5);
	//for (int i = 0; i < (int)blurSize; ++i)
	//{
	//	for (int j = 0; j < (int)blurSize; ++j)
	//	{
	//		float2 offset = (hlim + float2(float(i), float(j))) * texelSize;
	//		result += ambient_texture.Sample(decal_sampler, input.texcoord + offset.xy).r;
	//	}
	//}
	//result /= float(BLUR_SIZE * BLUR_SIZE);

	float3 A = blurredAO_texture.Sample(decal_sampler, input.texcoord).rgb;
	float result = A.r;


	float3 albedo = albedo_texture.Sample(decal_sampler, input.texcoord).xyz;
	float3 shadow = HasShadowMap(gTextureConfig) > 0 ? shadow_texture.Sample(decal_sampler, input.texcoord).xyz : 1.0;
	float3 skybox = skybox_texture.Sample(decal_sampler, input.texcoord).xyz;


	//output.result = float4(D * result + S * result, 1) * color;
	output.result = float4((D + S) * result * albedo * shadow + skybox * light_color.rgb, 1);
	output.postEffect = float4(A, 1);
	return output;
}