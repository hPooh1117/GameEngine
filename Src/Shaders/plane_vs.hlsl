struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

cbuffer CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world_view_projection;
	row_major float4x4 world;
};

cbuffer CBufferPerLight : register(b2)
{
	float4 light_color;
	float4 light_dir;
	float4 ambient_color;
	float4 eye_pos;
	float4 mat_color;
	float env_alpha;
	float refract;
}

PS_Input main(float3 pos : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
	PS_Input output = (PS_Input)0;
	float4 P = float4(pos, 1.0f);

	output.pos = mul(P, world_view_projection);
	output.texcoord = texcoord;
	output.color = mat_color;

	return output;
}