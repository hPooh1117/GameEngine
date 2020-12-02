//--------------------------------------------
//	テクスチャ
//--------------------------------------------

//Texture2D ambient_texture : register(t6);
//SamplerState ambient_sampler : register(s6);
//

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
static const float bias = 0.001;
static const float3 shadow_color = { 0.6f, 0.6f, 0.6f };

//cbuffer CBPerAO : register(b5)
//{
//	row_major float4x4 inv_proj;
//	row_major float4x4 inv_view;
//	row_major float4x4 proj;
//	row_major float4x4 view;
//	float2   screen_size;
//	float2   noise_scale;
//	float    radius;
//	float    power;
//	float    kernelSize;
//	float    ambient_bias;
//	float4   sample_pos[MAX_SAMPLES];
//	float    blurSize;
//	float3   dummyVector;
//}


cbuffer CBPerShadow : register(b6)
{
	column_major float4x4 light_view_projection;
}


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

