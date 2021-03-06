struct VS_Input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct PS_Input_Deffered
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD0;
	float3 localPos : TEXCOORD1;
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

TextureCube env_texture : register(t13);
SamplerState decal_sampler : register(s0);

cbuffer CBPerMatrix : register(b0)
{
	row_major float4x4 matWVP;
	row_major float4x4 world;

	row_major float4x4 inv_projview;
};


//
// vertex shader(default)
//
PS_Input_Deffered VSmain(VS_Input input)
{
	PS_Input_Deffered output = (PS_Input_Deffered)0;

	float4 P = float4(input.position, 1);

	output.position = mul(P, matWVP);
	output.position.z = output.position.w;
	output.texcoord = input.texcoord;
	output.color = input.color;
	output.w_pos = mul(P, world).xyz;
	output.w_normal = mul(float4(input.normal, 0), world).xyz;
	output.depth = output.position.zw * 0.0f;
	output.localPos = input.position;

	return output;
}

//
// pixel shader
//
PS_Output PSmain(PS_Input_Deffered input)
{
	PS_Output output = (PS_Output)0;
	float3 N = normalize(input.w_normal);

	output.position = float4(input.w_pos, 1);

	float3 envVector = normalize(input.localPos);
	//float3 tex = float3(input.texcoord, 1);
	float4 color = env_texture.SampleLevel(decal_sampler, envVector, 0);


	output.color = color;
	output.normal = float4(1, 0, 0, 1);
	output.shadow = float4(1, 1, 1, 1);
	output.depth = float4(0, 0, 0, 1);
	return output;
}
