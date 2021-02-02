//--------------------------------------------
//	グローバル変数
//--------------------------------------------
static const float bias = 0.001;
static const float3 shadow_color = { 0.6f, 0.6f, 0.6f };



//--------------------------------------------
//	データ
//--------------------------------------------
struct PS_Input_Shadow
{
	float4 position : SV_POSITION;
	float2 depth : TEXCOORD1;
};

struct PS_Input_Last
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 w_normal : TEXCOORD1;
	float3 w_pos : TEXCOORD2;
	float2 texcoord : TEXCOOORD3;
	float3 v_shadow : TEXCOORD4;
	float2 depth : TEXCOORD5;
};

