struct PS_Input
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 w_pos : TEXCOORD2;
    float3 w_normal : TEXCOORD3;
};

#define MAX_BONES 100
cbuffer CBPerMatrix : register(b0)
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
    float4 param;
};


