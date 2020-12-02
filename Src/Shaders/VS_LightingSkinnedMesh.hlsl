#include "H_DataFormat.hlsli"

PS_Input VSmainSkinning(VS_InputS input)
{
    PS_Input output = (PS_Input)0;

    // スキニング処理
    float4 p = { 0, 0, 0, 0 };
    float4 n = { 0, 0, 0, 0 };
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        p += (input.bone_weights[i] * mul(float4(input.position, 1), bone_transforms[input.bone_indices[i]]));
        n += (input.bone_weights[i] * mul(float4(input.normal, 0), bone_transforms[input.bone_indices[i]]));
    }
    p.w = 1.0f;
    n.w = 0.0f;
    float4 N = normalize(mul(n, world));
    float4 L = normalize(-light_dir);


    float3 wPos = mul(p, world).xyz;

    float3 wNormal = mul(n, world).xyz;
    wNormal = normalize(wNormal);


    output.color = mat_color;

    output.position = mul(p, matWVP);
    output.texcoord = input.texcoord;
    output.w_normal = wNormal;
    output.w_pos = wPos;

    return output;

}
