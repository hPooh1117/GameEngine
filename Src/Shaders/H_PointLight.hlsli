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
#define POINTMAX 32

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
    POINTLIGHT point_lights[POINTMAX];
};
