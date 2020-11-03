//---------------------------------
// テクスチャデータ
//---------------------------------

//Texture2D normal_texture : register(t1);
//SamplerState normal_sampler : register(s1);
//
//Texture2D height_texture : register(t2);
//SamplerState height_sampler : register(s2);

//--------------------------------------------------
// グローバル変数
//--------------------------------------------------
static const float viewOffset = 0.01;

//-------------------------------------------------
// データフォーマット
//-------------------------------------------------
struct GS_InputBump
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float3 w_pos : TEXCOORD1;

	float3 v_tan : TEXCOORD2;
	float3 v_bin : TEXCOORD3;
	float3 v_normal : TEXCOORD4;
};

struct PS_InputBump
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float3 w_normal : TEXCOORD1;
	float3 w_pos : TEXCOORD2;
	float2 texcoord : TEXCOORD3;
	float3 v_tan : TEXCOORD4;
	float3 v_bin : TEXCOORD5;
	float3 v_normal : TEXCOORD6;
};

struct PS_InputBumpShadow
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float3 w_normal : TEXCOORD1;
	float3 w_pos : TEXCOORD2;
	float2 texcoord : TEXCOORD3;
	float3 v_tan : TEXCOORD4;
	float3 v_bin : TEXCOORD5;
	float3 v_normal : TEXCOORD6;
	float3 v_shadow : TEXCOORD7;
	float2 depth : TEXCOORD8;
};