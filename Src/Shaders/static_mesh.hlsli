struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer CBufferPerMatrix : register(b0)
{
	row_major float4x4 world_view_projection;
	row_major float4x4 world;
};

cbuffer CBufferPerLight : register(b2)
{
	float4 material_color;
	float4 light_dir;
	//float4 ambient_color;
	//float4 eye_pos;
	//float env_alpha;
	//float refract;

}