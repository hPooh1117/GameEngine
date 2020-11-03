#include "H_DataFormat.hlsli"
#include "H_Functions.hlsli"
#include "H_DirectionalLight.hlsli"

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(input.position, 1.0);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 1.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	return output;
}

//
// vertex shader(SkinnedMesh)
//
PS_Input VSmainS(VS_InputS input)
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(input.position, 1.0);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;

	float4 N = float4(input.normal, 1.0f);
	float3 wN = mul(N, world).xyz;
	wN = normalize(wN);

	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	return output;
}

PS_Input VSmainSkinning(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

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

	float3 wN = mul(n, world).xyz;
	wN = normalize(wN);

	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	return output;
}




//
// pixel shader
//

static const float3 fogColor = { 0.8,0.8,0.6 };
static const float fogNear = 20.0;
static const float fogFar = 100.0;

static const float3 skyColor = { 0.4,0.6,0.8 };
static const float3 groundColor = { 0.6,0.4,0.2 };

float4 PSmain(PS_Input input) : SV_TARGET
{
	float4 color = diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
	float3 N = normalize(input.w_normal);
	float3 E = normalize(eye_pos.xyz - input.w_pos);
	float3 L = normalize(light_dir.xyz);

	float3 A = hemiSphereLight(N, skyColor, groundColor);

	float3 C = light_color.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = diffuse(N, L, C, Kd);

	float3 Ks = float3(1, 1, 1);
	float Shininess = 20;
	float3 S = blinnPhongSpecular(N, L, C, E, Ks, Shininess);

	color *= float4(A + D + S, 1.0);

	color.rgb = fog(color.rgb, eye_pos.xyz, input.w_pos, fogColor, fogNear, fogFar);

	return color;
}