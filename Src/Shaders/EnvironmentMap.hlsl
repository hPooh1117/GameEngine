#include "H_DataFormat.hlsli"
#include "H_Functions.hlsli"
#include "H_DirectionalLight.hlsli"

//--------------------------------------------------
// テクスチャ
//--------------------------------------------------

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

Texture2D environment_texture : register(t7);
SamplerState environment_sampler : register(s1);

static const float GAMMA = 2.2;
static const float EXPOSURE = 1.0;
static const float PURE_WHITE = 1.0;


//--------------------------------------------
//	データフォーマット
//--------------------------------------------
struct PS_InputEnv
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float3 w_normal : TEXCOORD1;
	float3 w_eye_dir : TEXCOORD2;
	float2 texcoord : TEXCOORD3;
};



//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_InputEnv VSmain(VS_Input input)
{
	PS_InputEnv output = (PS_Input)0;
	float4 P = float4(input.position, 1.0f);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;
	float3 wEyeDir = eye_pos.xyz - wPos;
	float eyeLen = length(wEyeDir);
	wEyeDir = normalize(wEyeDir);

	float4 N = float4(input.normal, 0);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_eye_dir = wEyeDir;

	return output;
}

//
// vertex shader(SkinnedMesh)
//
PS_InputEnv VSmainS(VS_InputS input)
{
	PS_InputEnv output = (PS_Input)0;
	float4 P = float4(input.position, 1.0f);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;
	float3 wEyeDir = eye_pos.xyz - wPos;
	float eyeLen = length(wEyeDir);
	wEyeDir = normalize(wEyeDir);

	float4 N = float4(input.normal, 0);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_eye_dir = wEyeDir;

	return output;
}

PS_InputEnv VSmainSkinning(VS_InputS input)
{
	PS_InputEnv output = (PS_Input)0;

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


	output.position = mul(p, matWVP);

	float3 wPos = mul(p, world).xyz;
	float3 wEyeDir = eye_pos.xyz - wPos;
	float eyeLen = length(wEyeDir);
	wEyeDir = normalize(wEyeDir);

	float3 wN = mul(n, world).xyz;
	wN = normalize(wN);

	output.color = input.color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_eye_dir = wEyeDir;

	return output;
}



//
// pixel shader
//
float4 PSmain(PS_InputEnv input) : SV_TARGET
{
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord);
	float3 N = normalize(input.w_normal);
	float3 E = normalize(input.w_eye_dir);
	float3 L = normalize(light_dir.xyz);

	// Ambient Light
	float3 A = ambient_color.rgb;

	// Diffuse Light
	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	// Specular Light
	float3 Ks = float3(1, 1, 1);
	float Shininess = 20;
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, Shininess);

	// Environment Map
	float3 ref = reflect(-E, N);

	ref.x = acos(ref.x) / 3.1416;
	ref.y = acos(ref.y) / 3.1416;

	ref.x *= 0.5;
	ref.x += 0.5;
	//ref.x = ref.z < 0.0 ? 1.0 - ref.x : ref.x;


	float3 envSrcColor = environment_texture.Sample(decal_sampler, ref.xy).rgb * EXPOSURE;

	// Reinhard tonemapping operator.
	// see: "Photographic Tone Reproduction for Digital Images" 2002
	// src: https://t-pot.com/program/123_ToneMapping/index.html
	float luminance = dot(envSrcColor, float3(0.2126, 0.7152, 0.0722));
	float mappedLuminance = (luminance * (1.0 + luminance / (PURE_WHITE * PURE_WHITE))) / (1.0 + luminance);

	// 平均輝度でスケーリング
	float3 mappedColor = (mappedLuminance / luminance) * envSrcColor;

	// ガンマ補正
	float4 envColor = float4(pow(abs(mappedColor), 1.0 / GAMMA), 1.0);

	//envColor = pow(abs(envColor), 1 / 2.2);
	color = color * (1.0 - env_alpha) + envColor * env_alpha;

	color *= input.color * float4(A + D + S, 1.0f);

	return color;
}