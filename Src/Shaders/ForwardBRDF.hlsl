#include "H_Functions.hlsli"

struct VS_Input
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct VS_InputS
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float4 bone_weights : WEIGHTS;
	uint4  bone_indices : BONES;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 w_pos : TEXCOORD1;
	float3 w_normal : TEXCOORD2;
	float3 w_tangent : TEXCOORD3;
	float3 w_binormal : TEXCOORD4;
	float4 color : COLOR;
};

Texture2D albedo_texture : register(t0);
Texture2D normal_texture : register(t1);
Texture2D height_texture : register(t2);
Texture2D metalness_texture : register(t3);
Texture2D roughness_texture : register(t4);
Texture2D ao_texture : register(t5);
Texture2D specular_texture : register(t6);
Texture2D environment_texture : register(t7);
TextureCube irradiance_texture : register(t10);
TextureCube specular_map_texture : register(t11);
Texture2D specular_LUT_texture : register(t12);

SamplerState decal_sampler : register(s0);
SamplerState clamp_sampler : register(s2);


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

	float gMetalness;
	float gRoughness;
	float gDiffuseFactor;
	float gSpecularFactor;
	int   textureConfig;
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

static float PI = 3.14159265;
static float INV_PI = 0.31830988;
static float EPSILON = 1e-6;
static float VIEW_OFFSET = 0.002;

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

	float4 T = float4(input.tangent, 0);
	T = mul(T, world);

	float4 B = float4(input.binormal, 0);
	B = mul(B, world);

	output.position = mul(P, matWVP);
	output.texcoord = input.texcoord;
	output.w_pos = wPos;
	output.w_normal = normalize(N.xyz);
	output.w_tangent = normalize(T.xyz);
	output.w_binormal = normalize(B.xyz);
	output.color = mat_color;

	return output;
}

PS_Input VSmainS(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);
	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 0);
	N = mul(N, world);

	float4 T = float4(input.tangent, 0);
	T = mul(T, world);

	float4 B = float4(input.binormal, 0);
	B = mul(B, world);

	output.position = mul(P, matWVP);
	output.texcoord = input.texcoord;
	output.w_pos = wPos;
	output.w_normal = normalize(N.xyz);
	output.w_tangent = normalize(T.xyz);
	output.w_binormal = normalize(B.xyz);
	output.color = mat_color;

	return output;
}

PS_Input VSmainSkinning(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);
	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 0);
	N = mul(N, world);

	float4 T = float4(input.tangent, 0);
	T = mul(T, world);

	float4 B = float4(input.binormal, 0);
	B = mul(B, world);

	output.position = mul(P, matWVP);
	output.texcoord = input.texcoord;
	output.w_pos = wPos;
	output.w_normal = normalize(N.xyz);
	output.w_tangent = normalize(T.xyz);
	output.w_binormal = normalize(B.xyz);
	output.color = mat_color;

	return output;
}


//Specular Factor
//float3 CalculateSchlickFresnelReflectance(float3 viewDir, float3 vHalf, float3 specular)
//{
//	return specular + (1.0 - specular) * pow(1.0 - (dot(vHalf, viewDir)), 5.0);
//}
//
//float CalculateSmithGGXGeometryTerm(float roughness, float nDotL, float nDotV)
//{
//	float roughnessActual = roughness * roughness;
//	float viewGeoTerm = nDotV + sqrt((nDotV - nDotV * roughnessActual) * nDotV + roughnessActual);
//	float lightGeoTerm = nDotL + sqrt((nDotL - nDotL * roughnessActual) * nDotL + roughnessActual);
//
//	return rcp(viewGeoTerm * lightGeoTerm);
//}
//
//float CalculateNormalDistributionTrowReitz(float roughness, float3 surfaceNormal, float3 microfacetNormal)
//{
//	float roughnessActual = roughness * roughness;
//
//	float numerator = pow(roughnessActual, 2.0);
//	float a = pow(dot(surfaceNormal, microfacetNormal), 2.0);
//	float b = numerator - 1.0;
//	float denominator = PI * pow(a * b + 1.0, 2.0);
//	return  numerator / denominator;
//}


//-------------------------------------------------------------------------------------------------------------------------------------------------
// Pixel Shader
//-------------------------------------------------------------------------------------------------------------------------------------------------
// Refference : http://alextardif.com/PhysicallyBasedRendering.html

// cf.) https://qiita.com/mebiusbox2/items/8a4734ab5b0854528789
//      https://www.shadertoy.com/view/4sSfzK
//      https://learnopengl.com/PBR/Theory
//      http://project-asura.com/blog/archives/2785
//--------------------------------------------------------
// BRDF (Bidirectional Reflectance Distribution Function)
//--------------------------------------------------------

// 鏡面反射BRDF -> Cook-Torranceモデル
//
// Fr(v, l) = D(h, a) * G(v, l, a) * F(v, h, f0) / 4(n, v)(n, l)
//
// D(h, a)     : 法線分布関数(Normal Distribution Function) ->Beckman, Phong, GGX(Trowbrdige-Retiz)
// G(v, l, a)  : 幾何減衰...マイクロファセットによる光の減衰 -> Smith関数のSchlick近似とHeight-Correlated Smith関数
// F(v, h, f0) : フレネル反射...視線ベクトルと法線によって反射率が変わる現象 -> Schlickによる近似式

// refference :optimize brdf
// http://filmicworlds.com/blog/optimizing-ggx-shaders-with-dotlh/
// http://gregory-igehy.hatenadiary.com/entry/2015/02/26/145353

float3 DiffuseBRDF(float3 diffuseColor)
{
	return diffuseColor * INV_PI;
}

float3 F_Schlick(float3 specColor, float hDotV)
{
	return (specColor + (1.0 - specColor) * pow(1.0 - hDotV, 5.0));
}

float D_GGX(float roughness, float nDotH)
{
	float roughness2 = roughness * roughness;
	float nDotH2 = nDotH * nDotH;
	float d = nDotH2 * (roughness2 - 1.0) + 1.0;
	return roughness2 / (PI * d * d);
}

float G_Smith_Schlick_GGX(float a, float nDotV, float nDotL)
{
	float k = a * a * 0.5 + EPSILON;
	float gl = 1.0 / (nDotL * (1.0 - k) + k);
	float gv = 1.0 / (nDotV * (1.0 - k) + k);

	return gl * gv;
}

float VTermsOptimized(float roughness, float lDotH)
{
	float V;
	float k = roughness * roughness * 0.5;
	float k2 = k * k;
	float invK2 = 1.0 - k2;
	V = rcp(lDotH * lDotH * invK2 + k2);

	return V;
}

uint QuerySpecularTextureLevels()
{
	uint width, height, levels;
	specular_map_texture.GetDimensions(0, width, height, levels);
	return levels;
}

// Texturing PBR
float4 PSmain2(PS_Input input) : SV_TARGET
{

	float3 V = normalize(eye_pos.xyz - input.w_pos);

	float2 Tex = input.texcoord;

	float3 N = normalize(input.w_normal);
	float3 T = normalize(input.w_tangent);
	float3 B = normalize(input.w_binormal);
	float3x3 Mat = { T, B, N };

	float3 InvV = normalize(mul(Mat, V));
	float Height = HasHeightMap(textureConfig) > 0 ? height_texture.Sample(decal_sampler, Tex).r : 0;
	Height = Height * 2.0 - 1.0;

	Tex.x -= InvV.x / InvV.z * Height * VIEW_OFFSET;
	Tex.y -= InvV.y / InvV.z * Height * VIEW_OFFSET;

	float3 NormalMap = normal_texture.Sample(decal_sampler, Tex).rgb;
	NormalMap = 2.0 * NormalMap - 1.0;
	NormalMap = normalize(NormalMap);

	N = HasNormalMap(textureConfig) > 0 ? mul(Mat, NormalMap) : N;


	float4 albedo = HasColorMap(textureConfig) > 0 ? pow(abs(albedo_texture.Sample(decal_sampler, Tex)), 2.2) * input.color : input.color;
	float metallic = HasMetallicMap(textureConfig) > 0 ? metalness_texture.Sample(decal_sampler, Tex).r : gMetalness;
	float roughness = HasRoughnessMap(textureConfig) > 0 ? roughness_texture.Sample(decal_sampler, Tex).r : gRoughness;
	roughness = max(roughness * roughness, 0.01);

	float3 L = normalize(-light_dir.xyz);
	float3 H = normalize(V + L);
	float3 C = light_color.rgb;

	float nDotL = saturate(dot(N, L));
	float nDotV = saturate(dot(N, V));
	float nDotH = saturate(dot(N, H));
	//float vDotH = saturate(dot(V, H));
	//float lDotV = saturate(dot(L, V));
	float lDotH = saturate(dot(L, H));

	// Specular
	float3 surfaceSpec = lerp(float3(0.04, 0.04, 0.04), albedo.rgb, metallic);

	float3 directionalLight = (float3)0;
	{
		float D = D_GGX(roughness, nDotH);
		//float G = G_Smith_Schlick_GGX(roughness, nDotV, nDotL);
		float G = VTermsOptimized(roughness, lDotH);
		float3 F = F_Schlick(surfaceSpec, lDotH);
		//float3 specularTerm = (F * (G * D)) / (4.0 * nDotL * nDotV + EPSILON) * gSpecularFactor * nDotL;
		float3 specularTerm = (F * (G * D)) * gSpecularFactor * nDotL;


		// Energy Conservation
		float3 Ks = F;
		float3 Kd = lerp((1.0 - F), 0.0, metallic);

		// Diffuse
		float3 diffuseTerm = Kd * DiffuseBRDF(albedo.rgb) * gDiffuseFactor * nDotL * PI;
		directionalLight = diffuseTerm + specularTerm;
	}

	float3 F = F_Schlick(surfaceSpec, dot(V, H));
	float3 Kd = lerp(1.0 - F, 0.0, metallic);

	// Indirect Diffuse
	float3 irradiance = irradiance_texture.Sample(decal_sampler, N).rgb;
	float3 indirectDiffuse = irradiance * albedo.rgb * Kd;

	// Indirect Specular
	float3 Lr = 2.0 * nDotV * N - V;
	uint specularTextureLevels = QuerySpecularTextureLevels();
	float3 specularIrradiance = specular_map_texture.SampleLevel(decal_sampler, Lr, roughness * specularTextureLevels).rgb;

	float2 specularBRDF = specular_LUT_texture.Sample(clamp_sampler, float2(nDotV, roughness)).rg;

	float3 specularIBL = (surfaceSpec * specularBRDF.x + specularBRDF.y) * specularIrradiance;

	// Ambient
	float ao = HasAOMap(textureConfig) > 0 ? ao_texture.Sample(decal_sampler, Tex).r : 1;
	float3 ambientTerm = (indirectDiffuse + specularIBL) * ao
		
		;
	//float3 ambientTerm = ao * float3(0.03, 0.03, 0.03) * albedo.rgb;

	float3 output = directionalLight + ambientTerm;
	//output = float3(specularBRDF.y, 0,0);
	//output = specularIrradiance;
	//float3 output = ambientTerm;

	// Tone Mapping
	output = output / (output + float3(1.0, 1.0, 1.0));
	output = pow(abs(output), 1.0 / 2.2);
	

	return float4(output, 1);

}

//float4 PSmainSetting2(PS_Input input) : SV_TARGET
//{
//	float4 albedo = albedo_texture.Sample(decal_sampler, input.texcoord) * input.color;
//	float metallic = gMetalness;
//	float roughness = max(gRoughness * gRoughness, 0.01);
//
//	float3 surfaceSpec = lerp(float3(0.04, 0.04, 0.04), albedo.rgb, metallic);
//
//	float3 N = normalize(input.w_normal);
//	float3 L = normalize(-light_dir.xyz);
//	float3 V = normalize(eye_pos.xyz - input.w_pos);
//	float3 H = normalize(V + L);
//	float3 C = light_color.rgb;
//
//	float nDotL = saturate(dot(N, L));
//	float nDotV = saturate(dot(N, V));
//	float nDotH = saturate(dot(N, H));
//	float vDotH = saturate(dot(V, H));
//	float lDotV = saturate(dot(L, V));
//
//	// Irradiance
//	float3 irradiance = nDotL * C;
//	irradiance *= PI;
//
//	// Specular
//	float D = D_GGX(roughness, nDotH);
//	float G = G_Smith_Schlick_GGX(roughness, nDotV, nDotL);
//	float3 F = F_Schlick(surfaceSpec, V, H);
//	float3 specularTerm = (F * (G * D)) / (4.0 * nDotL * nDotV + EPSILON) * irradiance * gSpecularFactor;
//
//	// Energy Conservation
//	float3 Ks = F;
//	float3 Kd = (float3(1.0, 1.0, 1.0) - F) * (1.0 - metallic);
//	
//	// Diffuse
//	float3 diffuseTerm = Kd * DiffuseBRDF(albedo.rgb) *irradiance * gDiffuseFactor;
//
//	// Ambient
//	float3 ambientTerm = float3(0.03, 0.03, 0.03) * albedo.rgb;
//
//	float4 output = float4(diffuseTerm + specularTerm + ambientTerm, 1);
//	//float4 output = float4(D, D, D, 1);
//	//float4 output = float4(G, G, G, 1);
//	//float4 output = float4(F, 1);
//	return output;
//}