#include "H_Functions.hlsli"

Texture2D diffuse_texture : register(t0);
Texture2D environment_texture : register(t1);
Texture2D metalness_texture : register(t2);
Texture2D roughness_texture : register(t3);
Texture2D a0_texture : register(t4);

SamplerState decal_sampler : register(s0);

struct VS_Input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD0;
	float3 w_pos : TEXCOORD1;
	float3 w_normal : TEXCOORD2;
};

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

//----------------------------------------------
// Vertex Shader
//----------------------------------------------
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1.0);

	float3 wPos = mul(P, world).xyz;

	float3 wN = mul(input.normal, (float3x3)world);
	wN = normalize(wN);

	output.position = mul(P, matWVP);
	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_pos = wPos;
	output.w_normal = wN;

	return output;
}

//----------------------------------------------
// Pixel Shader
//----------------------------------------------
float4 PSmain(PS_Input input) : SV_TARGET
{
	// フォンシェーダーの計算
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	float4 base = color;
	float3 N = normalize(input.w_normal);
	float3 E = normalize(eye_pos.xyz - input.w_pos.xyz);
	float3 L = normalize(light_dir.xyz);

	float3 A = ambient_color.rgb/*ao_texture.Sample(decal_sampler, input.texcoord).rgb*/;

	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, 20);

	color *= input.color * float4(A + D + S, 1.0);

	// 視線の反射
	float3 R = reflect(-E, N);
	R = normalize(R);

	// 映り込みUV設定
	float2 euv = R.xy;
	
	float3 R2 = R;
	R2.y = 0;

	euv.x = acos(R2.x) / 3.141592 * 0.5;
	euv.y = acos(euv.y) / 3.141592;

	// 裏面
	if (R.z < 0) euv.x = 1 - euv.x;

	float metalness = metalness_texture.Sample(decal_sampler, input.texcoord).r;
	float smoothness = 1.0 - roughness_texture.Sample(decal_sampler, input.texcoord).r;
	smoothness = pow(smoothness, 2);

	float4 env = environment_texture.SampleLevel(decal_sampler, euv, (1 - smoothness) * 9);

	// トーンマップ
	//env.rgb *= 0.1;
	//env.rgb = env.rgb / (1.0 + env.rgb) * 1.0;
	//env.rgb = pow(abs(env.rgb), 1.0 / 2.2);

	// 合成のバランス調整
	env *= (metalness + 0.1) * (base * 0.6 + 0.3);
	env.rgb *= (1 - metallic);
	color.rgb = color.rgb + env.rgb;
	//color.rgb = color.rgb * ( 1.0 - env_alpha ) + env.rgb * env_alpha;

	// フレネル
	float fresnel_rate = max(0, dot(-E, R));
	fresnel_rate = pow(fresnel_rate, 5);
	fresnel_rate *= 1.0;

	float3 fresnel = env.rgb * fresnel_rate;
	color.rgb += fresnel;
	return color;
}