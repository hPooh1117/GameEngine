#include "H_DataFormat.hlsli"

PS_Input VSmain(VS_Input input)
{
    PS_Input output = (PS_Input)0;
    float4 P = float4(input.position, 1);


    float3 wPos = mul(P, world).xyz;

    float4 N = float4(input.normal, 0);
    float3 wNormal = mul(N, world).xyz;
    wNormal = normalize(wNormal);


    output.color = mat_color;

    output.position = mul(P, matWVP);
    output.texcoord = input.texcoord;
    output.w_normal = wNormal;
    output.w_pos = wPos;

    return output;
}
