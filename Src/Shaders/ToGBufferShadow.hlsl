#include "H_ToGBuffer.hlsli"
#include "H_DataFormat.hlsli"
#include "H_ShadowMap.hlsli"
#include "H_Functions.hlsli"
//--------------------------------------------
//	エントリーポイント
//--------------------------------------------
//
// vertex shader(default)
//
PS_Input_Last VSmain(VS_Input input)
{
	PS_Input_Last output = (PS_Input_Last)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);


	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);
	return output;
}

//
// vertex shader(SkinnedMesh)
//
PS_Input_Last VSmainS(VS_InputS input)
{
	PS_Input_Last output = (PS_Input_Last)0;
	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	float3 N = mul(input.normal, (float3x3)world);


	output.position = mul(P, matWVP);
	output.w_pos = wPos.xyz;
	output.w_normal = N;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	output.v_shadow = GetShadowTex(light_view_projection, wPos.xyz);

	return output;
}


PS_Input_Last VSmainSkinning(VS_InputS input)
{
	PS_Input_Last output = (PS_Input_Last)0;

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

	return output;
}


//
// pixel shader
//
PS_Output PSmain(PS_Input_Last input)
{
	PS_Output output = (PS_Output)0;
	float4 P = float4(input.w_pos, 1);
	float3 N = normalize(input.w_normal);

	output.color = diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
	output.normal = float4(N, 1);
	output.position = P;
	output.shadow = float4(GetShadow(shadow_texture, shadow_sampler, input.v_shadow, shadow_color, bias), 1);
	return output;
}