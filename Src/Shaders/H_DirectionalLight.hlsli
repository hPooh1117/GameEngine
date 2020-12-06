//----------------------------------
// ƒOƒ[ƒoƒ‹•Ï”
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
    float  tess;
    uint   light_config;
    float  shininess;
};

