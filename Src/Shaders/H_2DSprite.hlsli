

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
//cbuffer CBPerMatrix : register(b0)
//{
//	row_major float4x4 matWVP;
//	row_major float4x4 world;
//
//	float4x4 inv_viewproj;
//	float4x4 inv_view_mat;
//	float4x4 inv_proj_mat;
//
//};
//
//cbuffer CBPerMeshMat : register(b1)
//{
//	float4 mat_color;
//
//	float3 specColor;
//
//	float gMetalness;
//	float gRoughness;
//	float gDiffuse;
//	float gSpecular;
//	int   gTextureConfig;
//}

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


