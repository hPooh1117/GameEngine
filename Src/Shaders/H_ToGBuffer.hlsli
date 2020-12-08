//---------------------------------
// テクスチャデータ
//---------------------------------
//Texture2D position_texture : register(t3);
//SamplerState position_sampler : register(s3);
//
//Texture2D depth_texture : register(t7);
//SamplerState depth_sampler : register(s7);


struct PS_Input_Shadow_AO
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 w_normal : TEXCOORD1;
	float3 w_pos : TEXCOORD2;
	float2 texcoord : TEXCOOORD3;
	float3 v_shadow : TEXCOORD4;
	float2 depth : TEXCOORD5;
};

struct PS_Output
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 position : SV_TARGET2;
	float4 shadow : SV_TARGET3;

};

struct PS_Output_AO
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 position : SV_TARGET2;
	float4 shadow : SV_TARGET3;
	float4 depth : SV_TARGET4;
};


struct PS_Output_Light
{
	float4 diffuse : SV_TARGET0;
	float4 specular : SV_TARGET1;
	float4 skybox : SV_TARGET2;
	float4 prelighting : SV_TARGET3;
};

struct PS_Output_SSAO
{
	float4 result : SV_TARGET0;
	float4 ambient : SV_TARGET1;
};

