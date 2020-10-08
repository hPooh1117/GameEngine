struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
    float3 w_normal : TEXCOORD1;	//ワールド法線
    //float3 wEyeDir : TEXCOORD2;	//ワールド視線
    float2 texcoord : TEXCOORD3;
    float3 w_pos : TEXCOORD4;	    //ワールド位置

};


static const float3 fogColor = { 0.8,0.8,0.6 };
static const float fogNear = 20.0;
static const float fogFar = 100.0;

cbuffer CBufferMatrix : register(b0)
{
    row_major float4x4 mat_WVP;
    row_major float4x4 world;
};

cbuffer CBufferLight : register(b2)
{
    float4 light_color;
    float4 light_dir;
    float4 ambient_color;
    float4 eye_pos;
    float4 mat_color;
    float4 param;
};

