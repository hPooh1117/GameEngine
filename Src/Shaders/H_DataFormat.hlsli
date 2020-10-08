//----------------------------------
// �O���[�o���ϐ�
//----------------------------------
#define MAX_BONES 150
cbuffer CBPerMatrix : register(b0)
{
    row_major float4x4 matWVP;
    row_major float4x4 world;

    float4 mat_color;

};


cbuffer CBPerBone : register(b4)
{
    row_major float4x4 bone_transforms[MAX_BONES];
    //row_major float4x4 motion_transforms[MAX_BONES];
    //row_major float4x4 bone_offsets[MAX_BONES];
}

//---------------------------------
// �f�[�^�t�H�[�}�b�g
//---------------------------------
struct VS_Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct VS_InputS
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float4 bone_weights : WEIGHTS;
    uint4  bone_indices : BONES;
    float3 tangent : TANGENT;
};

struct HS_Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 w_pos : TEXCOORD1;
};

// �������
struct HSConstantOutput
{
    float factor[3] : SV_TessFactor;
    float inner_factor : SV_InsideTessFactor;
};

struct DS_Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 w_pos : TEXCOORD1;
};


struct GS_Input
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord :TEXCOORD;
    float3 w_pos : TEXCOORD2;
    float3 normal : TEXCOORD3;
};

struct PS_Input
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 w_pos : TEXCOORD2;
    float3 w_normal : TEXCOORD3;
};




//---------------------------------
// �e�N�X�`���f�[�^
//---------------------------------
// 0 -> Albedo, ColorMap
// 1 -> NormalMap, EnvironmentMap
// 2 -> HeightMap
// 3 -> PositionMap
// 4 -> 
// 5 -> ShadowMap
// 6 -> AmbientMap
// 7 -> Depth

Texture2D diffuse_texture : register(t0);
SamplerState decal_sampler : register(s0);
