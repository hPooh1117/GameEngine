Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);

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

float4 main(PS_Input input) : SV_TARGET
{
	return diffuse_texture.Sample(decal_sampler, input.texcoord) * input.color;
}