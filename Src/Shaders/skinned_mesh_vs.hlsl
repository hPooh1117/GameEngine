#include "skinned_mesh.hlsli"


VS_OUT main(float4 pos : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 color : COLOR float4 bone_weights : WEIGHTS, uint4 bone_indices : BONES)
{
    VS_OUT output;
    output.position = mul(pos, world_view_projection);

    normal.w = 0;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-light_dir);

    // directional light
    output.color = mat_color * max(0, dot(L, N));
    output.color.a = mat_color.a;


    output.texcoord = texcoord;
    return output;
}