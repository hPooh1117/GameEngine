#include "H_Functions.hlsli"

struct VS_Input
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 w_pos : TEXCOORD1;
	float3 w_normal : TEXCOORD2;
	float4 color : COLOR;
};

Texture2D albedo_texture : register(t0);
SamplerState decal_sampler : register(s0);
Texture2D environment_texture : register(t1);
Texture2D metalness_texture : register(t2);
Texture2D roughness_texture : register(t3);

//----------------------------------
// ポイントライト構造体
//----------------------------------
struct POINTLIGHT
{
	float index;
	float range;
	float type;
	float dummy;
	float4 pos;
	float4 color;
};
#define POINTMAX 4

//----------------------------------
// スポットライト構造体
//----------------------------------
struct SPOTLIGHT
{
	float index;
	float range;
	float type;
	float inner_corn;
	float outer_corn;
	float dummy0;
	float dummy1;
	float dummy2;
	float4 pos;
	float4 color;
	float4 dir;
};
#define SPOTMAX 4

//----------------------------------
// グローバル変数
//----------------------------------
cbuffer CBPerMatrix : register(b0)
{
	row_major float4x4 matWVP;
	row_major float4x4 world;
	row_major float4x4 inv_projview;

};

cbuffer CBPerMeshMat : register(b1)
{
	float4 mat_color;

	float3 specColor;
	float brdfFactor;

	float gMetalness;
	float gRoughness;
	float dummy0;
	float dummy1;
}


cbuffer CBufferPerLight : register(b2)
{
	float4 light_color;
	float4 light_dir;
	float4 ambient_color;
	float4 eye_pos;
	float  env_alpha;
	float  reflact;
	float  time;
	float  tess;
	POINTLIGHT point_lights[POINTMAX];
	SPOTLIGHT spot_lights[SPOTMAX];
};



//-------------------------------------------------------------------------------------------------------------------------------------------------
// Vertex Shader
//-------------------------------------------------------------------------------------------------------------------------------------------------
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	
	float4 P = float4(input.position, 1);
	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 0);
	N = mul(N, world);

	output.position = mul(P, matWVP);
	output.texcoord = input.texcoord;
	output.w_pos = wPos;
	output.w_normal = normalize(N.xyz);
	output.color = mat_color;

	return output;
}

//Specular Factor
float3 CalculateSchlickFresnelReflectance(float3 viewDir, float3 vHalf, float3 specular)
{
	return specular + (1.0 - specular) * pow(1.0 - (dot(vHalf, viewDir)), 5.0);
}

float CalculateSmithGGXGeometryTerm(float roughness, float nDotL, float nDotV)
{
	float roughnessActual = roughness * roughness;
	float viewGeoTerm = nDotV + sqrt((nDotV - nDotV * roughnessActual) * nDotV + roughnessActual);
	float lightGeoTerm = nDotL + sqrt((nDotL - nDotL * roughnessActual) * nDotL + roughnessActual);

	return rcp(viewGeoTerm * lightGeoTerm);
}

float CalculateNormalDistributionTrowReitz(float roughness, float3 surfaceNormal, float3 microfacetNormal)
{
	float PI = 3.14159265f;
	float roughnessActual = roughness * roughness;

	float numerator = pow(roughnessActual, 2.0);
	float a = pow(dot(surfaceNormal, microfacetNormal), 2.0);
	float b = numerator - 1.0;
	float denominator = PI * pow(a * b + 1.0, 2.0);
	return  numerator / denominator;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------
// Pixel Shader
//-------------------------------------------------------------------------------------------------------------------------------------------------
//float4 PSmain(PS_Input input) : SV_TARGET
//{
//	float3 albedo = albedo_texture.Sample(decal_sampler, input.texcoord).rgb * input.color.rgb;
//	float  metalness = metalness_texture.Sample(decal_sampler, input.texcoord).r;
//	float  roughness = roughness_texture.Sample(decal_sampler, input.texcoord).r;
//	float  perceptualRoughness = roughness;
//
//	float3 wNormal = normalize(input.w_normal);
//	float3 viewDir = normalize(eye_pos.xyz - input.w_pos);
//
//	float4 color = BRDF(albedo, metalness, perceptualRoughness, wNormal, viewDir, light_dir.xyz, light_color.rgb);
//	return color;
//}

float4 PSmain(PS_Input input) : SV_TARGET
{
	float4 albedo = albedo_texture.Sample(decal_sampler, input.texcoord) * input.color;
	float metallic = metalness_texture.Sample(decal_sampler, input.texcoord).r;
 	float roughness = roughness_texture.Sample(decal_sampler, input.texcoord).r;

	float3 N = normalize(input.w_normal);
	float3 viewDir = normalize(eye_pos.xyz - input.w_pos);
	float3 vHalf = normalize(viewDir + -light_dir.xyz);
	float nDotL = max(0, dot(N, -light_dir.xyz));
	float nDotV = dot(N, viewDir);

	// Specular
	float specMetallic = max(metallic, 0.1);
	float3 surfaceSpec = lerp(float3(1.0, 1.0, 1.0), specColor.rgb, metallic);

	float3 F = CalculateSchlickFresnelReflectance(viewDir, vHalf, float3(specMetallic, specMetallic, specMetallic) * surfaceSpec * brdfFactor);
	float G = CalculateSmithGGXGeometryTerm(roughness, nDotL, nDotV);
	float D = CalculateNormalDistributionTrowReitz(roughness, N, vHalf);
	float3 specularTerm = F * G * D * nDotL/* * shadowContrib*/;

	// Diffuse
	float3 diffuseFactor = albedo.rgb *  nDotL * (1.0 - metallic)/* * shadowContrib*/;

	// Ambient
	float3 ambientFactor = ambient_color.rgb * lerp(albedo.rgb, float3(0.2, 0.2, 0.2), metallic);

	// Environment
	float3 ref = reflect(-viewDir, N);
	ref = ref * 0.5 + 0.5;
	ref.y = -ref.y;
	float3  envColor = environment_texture.Sample(decal_sampler, ref.xy).xyz;
	float3 environmentFactor = albedo.xyz * envColor * nDotL * metallic * (1.0 - roughness);


	return float4(specularTerm + diffuseFactor + ambientFactor + environmentFactor, 1);
}

// Refference : http://alextardif.com/PhysicallyBasedRendering.html

float4 PSmainSetting(PS_Input input) : SV_TARGET
{

	float4 albedo = albedo_texture.Sample(decal_sampler, input.texcoord) * input.color;
	float metallic = gMetalness;
	float roughness = gRoughness;

	float3 N = normalize(input.w_normal);
	float3 viewDir = normalize(eye_pos.xyz - input.w_pos);
	float3 vHalf = normalize(viewDir + -light_dir.xyz);
	float nDotL = max(0, dot(N, -light_dir.xyz));
	float nDotV = dot(N, viewDir);

	// Specular
	float specMetallic = max(metallic, 0.03);
	float3 surfaceSpec = lerp(float3(1.0, 1.0, 1.0), specColor.rgb, metallic);

	float3 F = CalculateSchlickFresnelReflectance(viewDir, vHalf, float3(specMetallic, specMetallic, specMetallic) * surfaceSpec * brdfFactor);
	float G = CalculateSmithGGXGeometryTerm(roughness, nDotL, nDotV);
	float D = CalculateNormalDistributionTrowReitz(roughness, N, vHalf);
	float3 specularTerm = F * G * D * nDotL/* * shadowContrib*/;

	// Diffuse
	float3 diffuseFactor = albedo.rgb * nDotL * (1.0 - metallic)/* * shadowContrib*/;

	// Ambient
	float3 ambientFactor = ambient_color.rgb * lerp(albedo.rgb, float3(0.2, 0.2, 0.2), metallic);

	// Environment
	float3 ref = reflect(-viewDir, N);
	ref = ref * 0.5 + 0.5;
	ref.y = -ref.y;
	float3  envColor = environment_texture.Sample(decal_sampler, ref.xy).xyz;
	float3 environmentFactor = albedo.xyz * envColor * nDotL * metallic * (1.0 - roughness);


	return float4(specularTerm + diffuseFactor + ambientFactor + environmentFactor, 1);
}