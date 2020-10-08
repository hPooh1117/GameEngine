#include "static_mesh.hlsli"


VS_OUT main(float4 pos : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    VS_OUT output;
    output.position = mul(pos, world_view_projection);

    normal.w = 0;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-light_dir);

    // ì_åıåπ
    float diffuseShade = 0.5f;
    output.color = material_color * max(0, dot(L, N) * diffuseShade + (1 - diffuseShade));
    output.color.a = material_color.a;

    // ïΩçsåıåπ
    //output.color = material_color * saturate(dot(L, N));
    //output.color = output.color * 0.5f + 0.5f;
    //output.color.a = material_color.a;

    output.texcoord = texcoord;
    return output;
}