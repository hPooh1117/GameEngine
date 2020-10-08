#include "phong_obj.hlsli"

//struct PS_Input
//{
//    float4 pos : POSITION;
//    float4 color : COLOR;
//    float2 texcoord : TEXCOORD1;
//    float3 w_pos : TEXCOORD2;
//    float3 w_normal : TEXCOORD3;
//};


PS_Input main(float3 pos : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    PS_Input output = (PS_Input)0;
    float4 P = float4(pos, 1);

    output.pos = mul(P, mat_WVP);

    float3 wPos = mul(P, world).xyz;

    float4 N = float4(normal, 0.0f);
    float3 wNormal = mul(N, world).xyz;
    wNormal = normalize(wNormal);


    output.color = color;


    output.texcoord = texcoord;
    output.w_normal = wNormal;
    output.w_pos = wPos;

    return output;

}