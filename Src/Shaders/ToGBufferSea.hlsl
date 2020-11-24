#include "H_ToGBuffer.hlsli"
#include "H_DataFormat.hlsli"
#include "H_BumpMapping.hlsli"
#include "H_DirectionalLight.hlsli"
#include "H_ShadowMap.hlsli"
#include "H_Functions.hlsli"

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

Texture2D normal_texture : register(t1);
Texture2D height_texture : register(t2);
Texture2D shadow_texture : register(t5);

// 波の速さ、向き
static const float2 wave1 = { 0.01, 0.005 };
static const float2 wave2 = { -0.005, -0.004 };

// 波の大きさ
static const float wave_size = 5.0f;

// 水面の全反射の臨界角
static const float limit = 0.248;

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_InputBumpShadow VSmain(VS_Input input)
{
	PS_InputBumpShadow output = (PS_InputBumpShadow)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);

	// 接空間行列
	float3 vy = { 0.0, 1.0, 0.001 };
	float3 vz = N;
	float3 vx = cross(vy, vz);
	vx = normalize(vx);
	vy = cross(vz, vx);
	vy = normalize(vy);


	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_tan = vx;
	output.v_bin = vy;
	output.v_normal = vz;
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);
	output.depth = output.position.zw;

	return output;
}

//
// vertex shader(SkinnedMesh)
//
PS_InputBumpShadow VSmainS(VS_InputS input)
{
	PS_InputBumpShadow output = (PS_InputBumpShadow)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);

	// 接空間行列
	float3 vy = { 0.0, 1.0, 0.001 };
	float3 vz = N;
	float3 vx = cross(vy, vz);
	vx = normalize(vx);
	vy = cross(vz, vx);
	vy = normalize(vy);


	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_tan = vx;
	output.v_bin = vy;
	output.v_normal = vz;
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);
	output.depth = output.position.zw;

	return output;
}


PS_InputBumpShadow VSmainSkinning(VS_InputS input)
{
	PS_InputBumpShadow output = (PS_InputBumpShadow)0;

	// スキニング処理
	float4 p = { 0, 0, 0, 0 };
	float4 n = { 0, 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
		n += (input.bone_weights[i] * mul(float4(input.normal, 0), bone_transforms[input.bone_indices[i]]));
	}
	p.w = 1.0f;
	n.w = 0.0f;

	float4 wPos = mul(p, world);

	float3 N = n.xyz;

	// 接空間行列
	float3 vy = { 0.0, 1.0, 0.001 };
	float3 vz = N;
	float3 vx = cross(vy, vz);
	vx = normalize(vx);
	vy = cross(vz, vx);
	vy = normalize(vy);


	output.position = mul(p, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_tan = vx;
	output.v_bin = vy;
	output.v_normal = vz;
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);
	output.depth = output.position.zw;

	return output;
}


float LinearizeDepth(float depth, float near, float far)
{
	return (2.0 * near) / (far + near - depth * (far - near));
}

//
// pixel shader
//
PS_Output_AO PSmain(PS_InputBumpShadow input)
{
	PS_Output_AO output = (PS_Output_AO)0;
	float4 P = float4(input.w_pos, 1);

	float3 vx = normalize(input.v_tan);
	float3 vy = normalize(input.v_bin);
	float3 vz = normalize(input.v_normal);

	float3 E = normalize(eye_pos.xyz - input.w_pos);

	// 接空間行列
	float3x3 vMat = { {vx}, {vy}, {vz} };

	// 接空間からワールドへ変換
	float3 invE = normalize(mul(vMat, E));

	float H = height_texture.Sample(decal_sampler, input.texcoord).r;
	H = H * 2.0f - 1.0f;
	input.texcoord.x -= invE.x * H * viewOffset;
	input.texcoord.y -= invE.y * H * viewOffset;

	// テクスチャサイズ調整
	float2 tex = input.texcoord * wave_size;

	// 法線マップから法線取得
	float3 N1 = normal_texture.Sample(decal_sampler, input.texcoord + wave1 * time).xyz;
	N1 = N1 * 2.0 - 1.0;
	float3 N2 = normal_texture.Sample(decal_sampler, input.texcoord + wave2 * time).xyz;
	N2 = N2 * 2.0 - 1.0;

	// 接空間からワールドへ変換
	float3 N = normalize(N1 + N2);
	N = normalize(mul(vMat, N));

	output.color = diffuse_texture.Sample(decal_sampler, tex) * input.color;
	output.normal = float4(N, 1);
	output.position = P;
	output.shadow = float4(GetShadow(shadow_texture, decal_sampler, input.v_shadow, shadow_color, bias), 1);
	output.depth = float4(input.depth.x / input.depth.y, 0, 0, 1);
	//output.depth.r = LinearizeDepth(output.depth.r, 0.1, 1000.0);

	return output;
}