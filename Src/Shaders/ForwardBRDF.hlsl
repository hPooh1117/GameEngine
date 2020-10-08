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

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

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

	float4 mat_color;

};

cbuffer CBufferPerLight : register(b2)
{
	float4 light_color;
	float4 light_dir;
	float4 ambient_color;
	float4 eye_pos;
	float  env_alpha;
	float  roughness;
	float  metallic;
	float  time;
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


//-------------------------------------------------------------------------------------------------------------------------------------------------
// Pixel Shader
//-------------------------------------------------------------------------------------------------------------------------------------------------
float4 PSmain(PS_Input input) : SV_TARGET
{
	float3 albedo = diffuse_texture.Sample(decal_sampler, input.texcoord).rgb * input.color.rgb;
	float  metalness = metallic;
	float  perceptualRoughness = roughness;

	float3 wNormal = normalize(input.w_normal);
	float3 viewDir = normalize(eye_pos.xyz - input.w_pos);

	float4 color = BRDF(albedo, metalness, perceptualRoughness, wNormal, viewDir, light_dir.xyz, light_color.rgb);
	return color;
}

// TODO : http://alextardif.com/PhysicallyBasedRendering.html