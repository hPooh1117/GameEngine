//----------------------------------
// グローバル変数
//----------------------------------
#define MAX_BONES 150
cbuffer CBPerMatrix : register(b0)
{
    row_major float4x4 matWVP;
    row_major float4x4 world;


    float4x4 inv_viewproj;
    float4x4 inv_view_mat;
    float4x4 inv_proj_mat;

};

cbuffer CBPerMeshMat : register(b1)
{
    float4 mat_color;

    float3 specColor;

    float gMetalness;
    float gRoughness;
    float gDiffuse;
    float gSpecular;
    int   gTextureConfig;
}

cbuffer CBPerBone : register(b4)
{
    row_major float4x4 bone_transforms[MAX_BONES];
    //row_major float4x4 motion_transforms[MAX_BONES];
    //row_major float4x4 bone_offsets[MAX_BONES];
}

//---------------------------------
// データフォーマット
//---------------------------------
struct VS_Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct VS_InputB
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;

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
    float3 binormal : BINORMAL;
};

struct HS_Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 w_pos : TEXCOORD1;
};

// 分割情報
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

struct PS_InputB
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 w_pos : TEXCOORD2;
    float3 w_normal : TEXCOORD3;
    float3 w_tangent : TEXCOORD4;
    float3 w_binormal : TEXCOORD5;
};


//---------------------------------
// テクスチャデータ
//---------------------------------
// 0 -> Albedo, ColorMap
// 1 -> NormalMap, EnvironmentMap
// 2 -> HeightMap
// 3 -> PositionMap
// 4 -> 
// 5 -> ShadowMap
// 6 -> AmbientMap
// 7 -> Depth


