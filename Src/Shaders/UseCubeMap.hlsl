#include "H_DataFormat.hlsli"
#include "HF_GlobalVariables.hlsli"

Texture2D diffuse_texture : register(t0);
TextureCube cube_texture : register(t9);
SamplerState decal_sampler : register(s0);
SamplerState clamp_sampler : register(s2);

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
PS_Input VSmain(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(input.position, 1.0);

	output.position = mul(P, matWVP);

	float3 wPos = mul(P, world).xyz;

	float3 wN = mul(input.normal, (float3x3)world).xyz;
	wN = normalize(wN);

	output.color = input.color;

	//テクスチャー座標
	output.texcoord = input.texcoord;
	output.w_normal = wN;
	output.w_pos = wPos;

	return output;
}

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 PSmain(PS_Input input) : SV_TARGET
{
	float4 color = diffuse_texture.Sample(decal_sampler,input.texcoord);
	float3 E = normalize(input.w_pos - eye_pos.xyz);
	float3 N = normalize(input.w_normal);
	//反射
	float3 R1 = (float3)0;
	R1 = reflect(E, N);

	//屈折・透過
	float3 R2 = (float3)0;
	R2 = refract(E, N, 1.0 / 1.5);  //
	//フレネルぽいもの
	float ratio = pow(dot(-E, N),2) * 0.5f;
	color.rgb += color.rgb * ratio + cube_texture.Sample(clamp_sampler,R1).rgb * (1.0 - ratio);
	color.rgb += cube_texture.Sample(clamp_sampler,R2).rgb * ratio;

	return color;
}



