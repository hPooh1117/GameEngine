#include "geometric_primitive.hlsli"


VS_OUT main( float4 pos : POSITION, float4 normal : NORMAL , float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    VS_OUT output;
    output.position = mul(pos, world_view_projection);

    normal.w = 0;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-light_dir);

    output.color = light_color * max(0, dot(L, N));
    output.color.a = light_color.a;
    return output;
}