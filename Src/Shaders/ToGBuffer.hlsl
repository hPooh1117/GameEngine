#include "H_ToGBuffer.hlsli"
#include "H_DataFormat.hlsli"


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

	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	P = mul(P, matWVP);

	output.position = P;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	return output;
}

//
// vertex shader(SkinnedMesh)
//
PS_Input VSmainS(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

	float4 P = float4(input.position, 1);

	float4 wPos = mul(P, world);

	P = mul(P, matWVP);

	output.position = P;
	output.color = mat_color;
	output.texcoord = input.texcoord;
	return output;
}

PS_Input VSmainSkinning(VS_InputS input)
{
	PS_Input output = (PS_Input)0;

	// スキニング処理
	float4 p = { 0, 0, 0, 0 };
	//float4 n = { 0, 0, 0, 0 };
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
		//n += (input.bone_weights[i] * mul(float4(input.normal, 0), bone_transforms[input.bone_indices[i]]));
	}
	p.w = 1.0f;
	//n.w = 0.0f;


	float4 wPos = mul(p, world);


	output.position = mul(p, matWVP);
	output.color = mat_color;
	output.texcoord = input.texcoord;
	return output;
}



//
// pixel shader
//
PS_Output PSmain(PS_Input input)
{
	PS_Output output = (PS_Output)0;

	// MRT
	output.color = diffuse_texture.Sample(decal_sampler, input.texcoord) * float4(1, 0, 0, 1);
	output.normal = diffuse_texture.Sample(decal_sampler, input.texcoord) * float4(0, 1, 0, 1);
	output.position = diffuse_texture.Sample(decal_sampler, input.texcoord) * float4(0, 0, 1, 1);

	return output;
}