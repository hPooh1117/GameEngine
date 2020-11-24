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
	ref = ref * 0.5 + 0.5;
	ref.y = -ref.y;
	float4  envColor = environment_texture.Sample(decal_sampler, ref.xy);
	envColor = pow(abs(envColor), 1 / 2.2);
	color = color * (1.0 - env_alpha) + envColor * env_alpha;

	color *= input.color * float4(A + D + S, 1.0f);

	return color;
}