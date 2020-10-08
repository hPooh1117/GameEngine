#include "sprite_batch.hlsli"
VS_OUT main(float4 pos : POSITION, float2 texcoord : TEXCOORD, column_major float4x4 ndc_transform : NDC_TRANSFORM, float4 texcoord_transform : TEXCOORD_TRANSFORM, float4 color : COLOR)
{
    VS_OUT  vout;
    vout.pos = mul(pos, ndc_transform);
    vout.texcoord = texcoord * texcoord_transform.zw + texcoord_transform.xy;
    vout.color = color;

    return vout;
}