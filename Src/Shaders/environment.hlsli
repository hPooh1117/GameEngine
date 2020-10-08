
//--------------------------------------------------
// Constants
//--------------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	row_major float4x4 mat_WVP;

	row_major float4x4 world;
};

cbuffer CBPerLight : register(b2)
{
	float4 light_color;
	float4 light_dir;
	float4 ambient_color;
	float4 eye_pos;
	float4 mat_color;
	float env_alpha;
	float refract;
};

//-------------------------------------------------
// Data Format
//-------------------------------------------------
struct VS_Input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float3 w_normal : TEXCOORD1;
	float3 w_eye_dir : TEXCOORD2;
	float2 texcoord : TEXCOORD3;
};