#include "H_ToGBuffer.hlsli"
#include "H_DataFormat.hlsli"
#include "H_ShadowMap.hlsli"
#include "H_Functions.hlsli"
#include "H_DirectionalLight.hlsli"


Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

Texture2D shadow_texture : register(t5);
SamplerState shadow_sampler : register(s5);

float random(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}




//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input_Shadow_AO VSmain(VS_Input input)
{
	PS_Input_Shadow_AO output = (PS_Input_Shadow_AO)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);


	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color * input.color;
	output.texcoord = input.texcoord;


	output.depth = output.position.zw;
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);
	return output;
}

//
// vertex shader(SkinnedMesh)
//
PS_Input_Shadow_AO VSmainS(VS_InputS input)
{
	PS_Input_Shadow_AO output = (PS_Input_Shadow_AO)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);


	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);
	output.depth = output.position.zw;
	return output;
}


PS_Input_Shadow_AO VSmainSkinning(VS_InputS input)
{
	PS_Input_Shadow_AO output = (PS_Input_Shadow_AO)0;

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

	float3 N = mul(n, world).xyz;


	output.position = mul(p, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
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
PS_Output_AO PSmain(PS_Input_Shadow_AO input)
{
	PS_Output_AO output = (PS_Output_AO)0;
	float4 P = float4(input.w_pos, 1);
	float3 N = normalize(input.w_normal);
	

	output.color = diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
	output.normal = float4(N, 1);
	output.position = P;

	output.shadow = float4(GetShadow(shadow_texture, shadow_sampler, input.v_shadow, shadow_color, bias), 1);

	// Plan A (Failed to calcurate world position on next pass)
	//float depth = input.position.z / input.position.w;
	//output.depth = float4(depth, depth, depth, 1.0f);

	// Plan B
	output.depth = input.depth.x / input.depth.y;
	//output.depth.r = LinearizeDepth(output.depth.r, 0.1, 100.0);
	return output;
}