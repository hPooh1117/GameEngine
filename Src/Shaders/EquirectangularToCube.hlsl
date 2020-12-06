
struct VS_Input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct PS_Input_
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD0;
	float3 localPos : TEXCOORD1;
};

struct PS_Input_Deffered
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD0;
	float3 localPos : TEXCOORD1;
	float3 w_pos : TEXCOORD2;
	float3 w_normal : TEXCOORD3;
	float2 depth : TEXCOORD4;
};

struct PS_Output
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 position : SV_TARGET2;
	float4 shadow : SV_TARGET3;
	float4 depth : SV_TARGET4;
};

cbuffer CBPerMatrix : register(b0)
{
	row_major float4x4 matWVP;
	row_major float4x4 world;
	row_major float4x4 inv_projview;

};

cbuffer CBPerLight : register(b2)
{
	float4 light_color;
	float4 light_dir;
	float4 ambient_color;
	float4 eye_pos;
	float  env_alpha;
	float  reflact;
	float  time;
	float  dummy2;
};

//--------------------------------------------
//	テクスチャ
//--------------------------------------------

Texture2D equirectangular_texture : register(t0);
SamplerState decal_sampler : register(s0);

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input_ VSmain(VS_Input input)
{
	PS_Input_ output = (PS_Input_)0;
	float4 P = float4(input.position, 1);
	output.position = mul(P, matWVP);
	output.position.z = output.position.w;
	output.color = input.color;
	output.texcoord = input.texcoord;
	output.localPos = input.position;
	return output;
}

#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define INV_GAMMA  (1 / 2.2)

float2 SampleSphericalMap(float3 v)
{
	float2 uv = float2(atan2(v.z, v.x), asin(-v.y));
	uv /= float2(-TWO_PI, PI);
	uv += float2(0.5, 0.5);
	return uv;
}

PS_Input_Deffered VSmainDeffered(VS_Input input)
{
	PS_Input_Deffered output = (PS_Input_Deffered)0;

	float4 P = float4(input.position, 1);

	output.position = mul(P, matWVP);
	output.position.z = output.position.w;
	output.texcoord = input.texcoord;
	output.color = input.color;
	output.w_pos = mul(P, world).xyz;
	output.w_normal = mul(float4(input.normal, 0), world).xyz;
	output.depth = output.position.zw;
	output.localPos = input.position;

	return output;
}


//
// pixel shader
//
float4 PSmain(PS_Input_ input) : SV_TARGET
{
	float2 uv = SampleSphericalMap(normalize(input.localPos));
	float4 color = equirectangular_texture.Sample(decal_sampler, uv) * light_color;
	float3 hdrColor = color.rgb / (color.rgb + float3(1.0, 1.0, 1.0));
	hdrColor = pow(abs(hdrColor), INV_GAMMA);

	return float4(hdrColor, 1.0);
}

PS_Output PSmainDeffered(PS_Input_Deffered input)
{
	PS_Output output = (PS_Output)0;
	float3 N = normalize(input.w_normal);

	output.position = float4(input.w_pos, 1);
	float2 uv = SampleSphericalMap(normalize(input.localPos));
	output.color = equirectangular_texture.Sample(decal_sampler, uv);
	output.normal = float4(1, 0, 0, 1);
	output.shadow = float4(1, 1, 1, 1);
	output.depth = float4(input.depth.x / input.depth.y, 0, 0, 1);
	return output;
}

