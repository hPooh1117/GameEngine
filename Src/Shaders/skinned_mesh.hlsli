struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

#define MAX_BONES 100
cbuffer CBuffer_World : register(b0)
{
    row_major float4x4 world_view_projection;
    row_major float4x4 world;

    //row_major float4x4 bone_transforms[MAX_BONES];
};

cbuffer CBuffer_Lighting : register(b2)
{
    float4 light_color;
    float4 light_dir;
    float4 ambient_color;
    float4 eye_pos;
    float4 mat_color;
    float4 param;
};

cbuffer CBuffer_Bone : register(b4)
{
    row_major float4x4 bone_transforms[MAX_BONES];
}