#include "skinned_mesh.hlsli"


VS_OUT main(float4 pos : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 color : COLOR float4 bone_weights : WEIGHTS, uint4 bone_indices : BONES)
{
    VS_OUT output;


    normal.w = 0;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-light_dir);

    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        p += (bone_weights[i] * mul(pos, bone_transforms[bone_indices[i]])).xyz;
        n += (bone_weights[i] * mul(float4(normal.xyz, 0), bone_transforms[bone_indices[i]])).xyz;
    }
    pos = float4(p, 1.0f);
    normal = float4(n, 0.0f);
    output.position = mul(pos, world_view_projection);


    // Unit19
    //float4 influence = { 0, 0, 0, 1 };
    //for (i = 0; i < 4; ++i)
    //{
    //    float weight = bone_weights[i];
    //    if (weight > 0.0f)
    //    {
    //        switch (bone_indices[i])
    //        {
    //        case 0: influence.r = weight; break;
    //        case 1: influence.g = weight; break;
    //        case 2: influence.b = weight; break;
    //        }
    //    }
    //}
    //output.color = influence;
    //output.color = material_color;


    // ì_åıåπ
    output.color = mat_color * max(0, dot(L, N));
    output.color.a = mat_color.a;

    // ïΩçsåıåπ
    //output.color = material_color * saturate(dot(L, N));
    //output.color = output.color * 0.5f + 0.5f;
    //output.color.a = material_color.a;

    output.texcoord = texcoord;
    return output;
}