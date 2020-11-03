

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerMatrix : register(b0)
{
	row_major float4x4 matWVP;
	row_major float4x4 world;

	float4 mat_color;
	row_major float4x4 inv_projview;

};

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------

struct VS_Input
{
	float3 position : POSITION;
	float4 color    : COLOR;
	float2 texcoord      : TEXCOORD;
};

struct PS_Input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};


