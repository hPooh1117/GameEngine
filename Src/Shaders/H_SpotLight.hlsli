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
#define SPOTMAX 32

//----------------------------------
// グローバル変数
//----------------------------------
cbuffer CBPerLight : register(b2)
{
    float4 light_color;
    float4 light_dir;
    float4 ambient_color;
    float4 eye_pos;
    float  env_alpha;
    float  reflact;
    float  time;
    float  dummy2;
    SPOTLIGHT spot_lights[SPOTMAX];
};
