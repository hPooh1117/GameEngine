#include "H_DataFormat.hlsli"
#include "H_DirectionalLight.hlsli"

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

struct PS_Input_Deffered
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float3 w_pos : TEXCOORD2;
	float3 w_normal : TEXCOORD3;
	float2 depth : TEXCOORD4;
};

struct PS_Output
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 position : SV_TARGET2;
	float4 shadow : SV_TARGET3;
	float4 depth : SV_TARGET4;
};

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

	output.position = mul(P, matWVP);
	output.texcoord = input.texcoord;
	output.color = input.color;
	//output.w_pos = mul(P, world).xyz;
	//output.w_normal = mul(float4(input.normal, 0), world).xyz;
	return output;

	//float4 P = float4(input.position.x, input.position.y, 1, 1);

	//output.position = P;
	//P = mul(P, invProj);
	//P.xyz /= P.w;
	//output.w_pos = P.xyz;
	//return output;
}

PS_Input_Deffered VSmainDeffered(VS_Input input)
{
	PS_Input_Deffered output = (PS_Input_Deffered)0;

	float4 P = float4(input.position, 1);

	output.position = mul(P, matWVP);
	output.texcoord = input.texcoord;
	output.color = input.color;
	output.w_pos = mul(P, world).xyz;
	output.w_normal = mul(float4(input.normal, 0), world).xyz;
	output.depth = output.position.zw;
	return output;

	//float4 P = float4(input.position.x, input.position.y, 1, 1);

	//output.position = P;
	//P = mul(P, invProj);
	//P.xyz /= P.w;
	//output.w_pos = P.xyz;
	//return output;
}


//
// pixel shader
//
float4 PSmain( PS_Input input ) : SV_TARGET
{
	return diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
}

PS_Output PSmainDeffered(PS_Input_Deffered input)
{
	PS_Output output = (PS_Output)0;
	float3 N = normalize(input.w_normal);

	output.position = float4(input.w_pos, 1);
	output.color = diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
	output.normal = float4(N, 1);
	output.shadow = float4(1, 1, 1, 1);
	output.depth = float4(input.depth.x / input.depth.y, 0, 0, 1);
	return output;
}