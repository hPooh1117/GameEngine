#include "fog.hlsli"

PS_INPUT main(float3 pos : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    PS_INPUT output = (PS_INPUT)0;

    float4 P = float4(pos, 1.0f);

    output.pos = mul(P, mat_WVP);

    float3 wPos = mul(P, world).xyz;

    float4 N = float4(normal, 0.0f);

    float3 wNormal = mul(N, world).xyz;

    output.color = color;

    output.texcoord = texcoord;
    output.w_normal = wNormal;
    output.w_pos = wPos;

    return output;
};