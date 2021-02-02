static const uint MAX_BONES = 150;

//----------------------------------
// ポイントライト構造体
//----------------------------------
struct POINTLIGHT
{
    float index;
    float range;
    float type;
    float dummy;
    float4 pos;
    float4 color;
};
static const uint POINTMAX = 64;

//----------------------------------
// スポットライト構造体
//----------------------------------
struct SPOTLIGHT
{
    float index;
    float range;
    float type;
    float inner_corn;
    float outer_corn;
    float dummy0;
    float dummy1;
    float dummy2;
    float4 pos;
    float4 color;
    float4 dir;
};
static const uint SPOTMAX = 32;


//----------------------------------
// From CPU
//----------------------------------
cbuffer CBPerMatrix : register(b0)
{
    row_major float4x4 matWVP;
    row_major float4x4 world;


    float4x4 inv_viewproj;
    float4x4 inv_view_mat;
    float4x4 inv_proj_mat;

    float4 param;

};

cbuffer CBPerMaterial : register(b1)
{
    float4 mat_color;

    float3 specColor;

    float gMetalness;
    float gRoughness;
    float gDiffuse;
    float gSpecular;
    int   gTextureConfig;
}

cbuffer CBPerLight : register(b2)
{
    float4 light_color;
    float4 light_dir;
    float4 ambient_color;
    float4 eye_pos;
    float  env_alpha;
    float  reflact;
    float  time;
    float  tess_factor;
    uint   light_config;
    float  shininess;
    float  dummy1;
    float  dummy2;
    POINTLIGHT point_lights[POINTMAX];
    SPOTLIGHT spot_lights[SPOTMAX];
};

cbuffer CBufferForCubemap : register(b3)
{
    row_major float4x4 view[6];
    row_major float4x4 proj;
};

cbuffer CBPerBone : register(b4)
{
    row_major float4x4 bone_transforms[MAX_BONES];
}

cbuffer CBPerShadow : register(b6)
{
    column_major float4x4 light_view_projection;
}

cbuffer CBPerPostProcess : register(b7)
{
    float3 correctedColor;
    float contrast;
    float saturation;
    float vignette;
};
