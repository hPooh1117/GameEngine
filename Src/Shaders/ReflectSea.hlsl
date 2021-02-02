#include "H_DataFormat.hlsli"
#include "H_Functions.hlsli"
#include "HF_GlobalVariables.hlsli"

Texture2D diffuse_texture : register(t0);
TextureCube cube_texture : register(t9);

Texture2D normal_texture : register(t1);
Texture2D height_texture : register(t2);

SamplerState decal_sampler : register(s0);
SamplerState clamp_sampler : register(s2);

//----------------------------------
// グローバル変数
//----------------------------------
static const float2 WAVE1 = { 0.005, 0.001 };
static const float2 WAVE2 = { -0.005, 0.016 };
static const float WAVE_SIZE = 1.0f;
static const float FLUCTUATION = 0.01f;

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
PS_InputB VSmain(VS_InputB input)
{
	PS_InputB output = (PS_InputB)0;
	float4 P = float4(input.position, 1);

	float3 wPos = mul(P, world).xyz;

	float3 wN = mul(input.normal, (float3x3)world);
	wN = normalize(wN);
	float3 wT = mul(input.tangent, (float3x3)world);
	wT = normalize(wT);
	float3 wB = mul(input.binormal, (float3x3)world);
	wB = normalize(wB);

	output.position = mul(P, matWVP);
	output.color = input.color * mat_color;
	output.texcoord = input.texcoord;
	output.w_pos = wPos;
	output.w_normal = wN;
	output.w_tangent = wT;
	output.w_binormal = wB;
	return output;
}


//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSmain(PS_InputB input) : SV_TARGET
{
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	float3 E = normalize(input.w_pos - eye_pos.xyz);
	float3 N = normalize(input.w_normal);
	float3 L = normalize(light_dir.xyz);
	float3 C = light_color.rgb;

	float2 tex = input.texcoord;

	float3 N1 = normal_texture.Sample(decal_sampler, tex + WAVE1 * time).xyz;
	N1 = N1 * 2.0 - 1.0;
	float3 N2 = normal_texture.Sample(decal_sampler, tex + WAVE2 * time).xyz;
	N2 = N2 * 2.0 - 1.0;

	float3 vx = normalize(input.w_tangent);
	float3 vy = normalize(input.w_binormal);
	float3 vz = normalize(input.w_normal);
	float3x3 mat = { vx, vy, vz };

	float3 waveNormal = normalize(mul(N1 + N2, mat));

	float3 Ks = { 1,1,1 };
	float3 S = CalculatePhongSpecular(waveNormal, L, C, -E, Ks, 20);

	float3 R1 = reflect(E, N);

	float ratio = saturate(dot(-E, N) + 0.2);
	R1 = R1 + waveNormal * FLUCTUATION;
	color.rgb = color.rgb * ratio + cube_texture.Sample(decal_sampler, R1).rgb * (1.0 - ratio);
	color.rgb += S;

	return color;
}